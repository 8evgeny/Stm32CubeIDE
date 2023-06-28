/* tls-server-size.c
 *
 * Copyright (C) 2006-2020 wolfSSL Inc.
 *
 * This file is part of wolfSSL. (formerly known as CyaSSL)
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdio.h>
#include "w5500.h"
#include "socket.h"
#include "main.h"
extern void Printf(const char* fmt, ...);
extern void wep_define_func(void);
extern uint8_t socknumlist[];

#ifndef WOLFSSL_USER_SETTINGS
    #include <wolfssl/options.h>
#endif
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

#include "certs.h"
#include "HexTrans.h"
#include "httpServer.h"
WOLFSSL_CTX* server_ctx = NULL;
WOLFSSL*     server_ssl = NULL;
#if !defined(NO_WOLFSSL_SERVER) && !defined(WOLFSSL_NO_TLS12)

/* Size of static buffer for dynamic memory allocation. */
#ifdef WOLFSSL_STATIC_MEMORY_SMALL
    #define STATIC_MEM_SIZE       (24*1024)
#elif defined(HAVE_ECC)
    #define STATIC_MEM_SIZE       (144*1024)
#else
    #define STATIC_MEM_SIZE       (96*1024)
#endif


#ifdef WOLFSSL_STATIC_MEMORY
    /* Hint pointers for dynamic memory allocating from buffer. */
    static WOLFSSL_HEAP_HINT* HEAP_HINT_SERVER;

    /* Static buffers to dynamically allocate from. */
    static byte gTestMemoryServer[STATIC_MEM_SIZE];
#else
    #define HEAP_HINT_SERVER NULL
#endif /* WOLFSSL_STATIC_MEMORY */
void UART_Printf(const char* fmt, ...);

/* Buffer for server connection to allocate dynamic memory from. */
unsigned char client_buffer[BUFFER_SIZE];
int client_buffer_sz = 0;
unsigned char server_buffer[BUFFER_SIZE];
int server_buffer_sz = 0;
int Handshake = 0;

void w5500_packetReceive_forTLS(uint8_t sn)
{
    uint16_t len;
    len = getSn_RX_RSR(sn);
    if(!len) return; //Если пришел пустой пакет, то уходим из функции
    printf("socket %d len_data: %d\n", sn, len);
    recv(sn, server_buffer, len);
    server_buffer_sz = len;
}

void w5500_packetSend_forTLS(uint8_t sn)
{
    send(sn, client_buffer, client_buffer_sz);
    client_buffer_sz = 0;
}

/* Server attempts to read data from client. */
int recv_server(WOLFSSL* ssl, char* buff, int sz, void* ctx)
{
    if (server_buffer_sz <= 0)
        w5500_packetReceive_forTLS(0);

    if (server_buffer_sz > 0)
    {
        if (sz > server_buffer_sz)
        {
            sz = server_buffer_sz;
        }
        XMEMCPY(buff, server_buffer, sz);
        if (sz <= server_buffer_sz)
        {
            XMEMMOVE(server_buffer, server_buffer + sz, server_buffer_sz - sz);
        }
        server_buffer_sz -= sz;
    }
    else
        sz = WOLFSSL_CBIO_ERR_WANT_READ;
    return sz;
}

/* Server attempts to write data to client. */
int send_server(WOLFSSL* ssl, char* buff, int sz, void* ctx)
{
    printf("-- send_server %d byte --\n", sz);
    if (client_buffer_sz < BUFFER_SIZE)
    {
        if (sz > BUFFER_SIZE - client_buffer_sz)
            sz = BUFFER_SIZE - client_buffer_sz;
        XMEMCPY(client_buffer + client_buffer_sz, buff, sz);
        client_buffer_sz += sz;
    }
    else
        sz = WOLFSSL_CBIO_ERR_WANT_WRITE;

     if (client_buffer_sz > 0)
         w5500_packetSend_forTLS(0);
//string_print_Hex((unsigned char *)buff, sz);
    return sz;
}

/* Create a new wolfSSL server with a certificate for authentication. */
static int wolfssl_server_new(WOLFSSL_CTX** ctx, WOLFSSL** ssl)
{
    Printf("wolfssl_server_new\n");
    int          ret = 0;
    server_ctx = NULL;
    server_ssl = NULL;

    /* Create and initialize WOLFSSL_CTX */
    if ((server_ctx = wolfSSL_CTX_new_ex(wolfTLSv1_2_server_method(),
                                                   HEAP_HINT_SERVER)) == NULL) {
        Printf("ERROR: failed to create WOLFSSL_CTX\n");
        ret = -1;
    }

    if (ret == 0) {
        /* Load client certificates into WOLFSSL_CTX */
        if (wolfSSL_CTX_use_certificate_buffer(server_ctx, SERVER_CERT,
                SERVER_CERT_LEN, WOLFSSL_FILETYPE_ASN1) != WOLFSSL_SUCCESS) {
            Printf("ERROR: failed to load server certificate\n");
            ret = -1;
        }
    }

    if (ret == 0) {
        /* Load client certificates into WOLFSSL_CTX */
        if (wolfSSL_CTX_use_PrivateKey_buffer(server_ctx,
                SERVER_KEY, SERVER_KEY_LEN, WOLFSSL_FILETYPE_ASN1) !=
                WOLFSSL_SUCCESS) {
            Printf("ERROR: failed to load server key\n");
            ret = -1;
        }
    }

    if (ret == 0) {
        /* Register callbacks */
        wolfSSL_SetIORecv(server_ctx, recv_server);
        wolfSSL_SetIOSend(server_ctx, send_server);
    }

    if (ret == 0) {
        /* Create a WOLFSSL object */
        if ((server_ssl = wolfSSL_new(server_ctx)) == NULL) {
            Printf("ERROR: failed to create WOLFSSL object\n");
            ret = -1;
        }
    }

    if (ret == 0) {
        *ctx = server_ctx;
        *ssl = server_ssl;
    }
    else {
        if (server_ssl != NULL)
            wolfSSL_free(server_ssl);
        if (server_ctx != NULL)
            wolfSSL_CTX_free(server_ctx);
    }
    return ret;
}

/* Server accepting a client using TLS */
static int wolfssl_server_accept(WOLFSSL* server_ssl)
{
    int ret = 0;

    if (wolfSSL_accept(server_ssl) != WOLFSSL_SUCCESS) {
        if (wolfSSL_want_read(server_ssl)) {
        }
        else if (wolfSSL_want_write(server_ssl)) {
            Printf("Server waiting for buffer\n");
        }
        else
            ret = -1;
    }
    return ret;
}

/* Send application data. */
int wolfssl_send(WOLFSSL* ssl, const char* msg)
{
    printf("-- wolfssl_send --\n");
    int ret;

    printf("\nfrom server:\n%s\n", msg);
    ret = wolfSSL_write(ssl, msg, XSTRLEN(msg));
    if (ret < XSTRLEN(msg))
        ret = -1;
    else
        ret = 0;

    return ret;
}

/* Receive application data. */
int wolfssl_recv(WOLFSSL* ssl)
{
printf("-- wolfssl_recv --\n");

    int ret;
    byte reply[2048];

    ret = wolfSSL_read(ssl, reply, sizeof(reply)-1);
    if (ret > 0) {
        reply[ret] = '\0';
printf("\nfrom client:\n%s\n", reply);
        ret = 0;
    }

    return ret;
}


/* Free the WOLFSSL object and context. */
static void wolfssl_free(WOLFSSL_CTX* ctx, WOLFSSL* ssl)
{
    Printf("wolfssl_free\n");
    if (ssl != NULL)
        wolfSSL_free(ssl);
    if (ctx != NULL)
        wolfSSL_CTX_free(ctx);
}


/* Main entry point. */
void tls_server_Handshake()
{
    Printf("tls_server_Handshake\n");
    int ret = 0;
//    WOLFSSL_CTX* server_ctx = NULL;
//    WOLFSSL*     server_ssl = NULL;

#if defined(DEBUG_WOLFSSL)
    wolfSSL_Debugging_ON();
#endif
    /* Initialize wolfSSL library. */
    wolfSSL_Init();
#ifdef WOLFSSL_STATIC_MEMORY
    if (wc_LoadStaticMemory(&HEAP_HINT_SERVER, gTestMemoryServer,
                               sizeof(gTestMemoryServer),
                               WOLFMEM_GENERAL | WOLFMEM_TRACK_STATS, 1) != 0) {
        Printf("unable to load static memory");
        ret = -1;
#endif

    if (ret == 0)
        ret = wolfssl_server_new(&server_ctx, &server_ssl);

    /* Loop to perform SSL handshake. */
    while (ret == 0)
    {
            ret = wolfssl_server_accept(server_ssl);
        if (ret == 0 && wolfSSL_is_init_finished(server_ssl)) {
            break;
        }
    }

    if (ret == 0)
    {
        Handshake = 1;
        Printf("\nHANDSHAKE COMPLETE\n\n");
    }
}

#else

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    Printf("Must build wolfSSL with TLS v1.2 and server enabled for this example\n");
    return 0;
}

#endif
