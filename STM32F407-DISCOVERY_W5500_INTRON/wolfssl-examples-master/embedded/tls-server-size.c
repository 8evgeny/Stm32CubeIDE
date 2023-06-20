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
/* Application data to send. */
static const char msgHTTPIndex[] =
    "HTTP/1.1 200 OK\n"
    "Content-Type: text/html\n"
    "Connection: close\n"
    "\n"
    "<html>\n"
    "<head>\n"
    "<title>Welcome to wolfSSL!</title>\n"
    "</head>\n"
    "<body>\n"
    "<p>wolfSSL has successfully performed handshake!</p>\n"
    "</body>\n"
    "</html>\n";

static const char msgHTTPIndex2[] =
//        "HTTP/1.1 200 OK\n"
//        "Content-Type: text/html\n"
//        "Connection: close\n"
//        "\n"
                                       "<!DOCTYPE html> "
                                       "<html lang='ru'>"
                                       "  <head> "
                                       "  <meta charset='utf-8'>"
                                       "  <title>Авторизация</title>"
                                       "  </head>"
                                       "  <body> "
                                       "  <br><form onsubmit='return false;' id='frm1' >"
                                       "    <label for='login'>Логин:&nbsp;&nbsp;&nbsp;</label> "
                                       "    <input type='text' id='login' name='login' value=''><br>"
                                       "  </form><br>"
                                       "  <form onsubmit='return false;' id='frm2' >"
                                       "    <label for='password'>Пароль:&nbsp;</label>"
                                       "    <input type='password' id='password' name='password' value=''><br>"
                                       "  </form> <br>"
                                       "<button onclick='send()'>Вход</button>"
                                       "<script>"
                                       "function sleep(milliseconds) "
                                       "{"
                                       "  const date = Date.now();"
                                       "  let currentDate = null;"
                                       "  do {"
                                       "    currentDate = Date.now();"
                                       "  } while (currentDate - date < milliseconds);"
                                       "}"
                                       ""
                                       "function send() "
                                       "{"
                                       "  let x1 = document.getElementById('login').value;"
                                       "  let step1 = new XMLHttpRequest();"
                                       "  step1.open('GET', 'l'+x1);"
                                       "  step1.send();"
                                       "  sleep(300);"
                                       ""
                                       "  let x2 = document.getElementById('password').value;"
                                       "  let step2 = new XMLHttpRequest();"
                                       "  var hash = md5(x2);"
                                       "  step2.open('GET', 'p'+hash);"
                                       "  step2.send();"
                                       "  sleep(300);"
                                       ""
                                       "  let step3 = new XMLHttpRequest();"
                                       "  step3.open('GET', '/ ');"
                                       "  location.reload();"
                                       "}"
                                       ""
                                       "var md5 = function (str) {"
                                       "  /*"
                                       "   * Add integers, wrapping at 2^32. This uses 16-bit operations internally"
                                       "   * to work around bugs in some JS interpreters."
                                       "   */"
                                       "  function safe_add(x, y) {"
                                       "    var lsw = (x & 0xFFFF) + (y & 0xFFFF),"
                                       "      msw = (x >> 16) + (y >> 16) + (lsw >> 16);"
                                       "    return (msw << 16) | (lsw & 0xFFFF);"
                                       "  }"
                                       ""
                                       "  /*"
                                       "   * Bitwise rotate a 32-bit number to the left."
                                       "   */"
                                       "  function bit_rol(num, cnt) {"
                                       "    return (num << cnt) | (num >>> (32 - cnt));"
                                       "  }"
                                       ""
                                       "  /*"
                                       "   * These functions implement the four basic operations the algorithm uses."
                                       "   */"
                                       "  function md5_cmn(q, a, b, x, s, t) {"
                                       "    return safe_add(bit_rol(safe_add(safe_add(a, q), safe_add(x, t)), s), b);"
                                       "  }"
                                       ""
                                       "  function md5_ff(a, b, c, d, x, s, t) {"
                                       "    return md5_cmn((b & c) | ((~b) & d), a, b, x, s, t);"
                                       "  }"
                                       ""
                                       "  function md5_gg(a, b, c, d, x, s, t) {"
                                       "    return md5_cmn((b & d) | (c & (~d)), a, b, x, s, t);"
                                       "  }"
                                       ""
                                       "  function md5_hh(a, b, c, d, x, s, t) {"
                                       "    return md5_cmn(b ^ c ^ d, a, b, x, s, t);"
                                       "  }"
                                       ""
                                       "  function md5_ii(a, b, c, d, x, s, t) {"
                                       "    return md5_cmn(c ^ (b | (~d)), a, b, x, s, t);"
                                       "  }"
                                       ""
                                       "  /*"
                                       "   * Calculate the MD5 of an array of little-endian words, and a bit length."
                                       "   */"
                                       "  function binl_md5(x, len) {"
                                       "    /* append padding */"
                                       "    x[len >> 5] |= 0x80 << ((len) % 32);"
                                       "    x[(((len + 64) >>> 9) << 4) + 14] = len;"
                                       ""
                                       "    var i, olda, oldb, oldc, oldd,"
                                       "      a = 1732584193,"
                                       "      b = -271733879,"
                                       "      c = -1732584194,"
                                       "      d = 271733878;"
                                       ""
                                       "    for (i = 0; i < x.length; i += 16) {"
                                       "      olda = a;"
                                       "      oldb = b;"
                                       "      oldc = c;"
                                       "      oldd = d;"
                                       ""
                                       "      a = md5_ff(a, b, c, d, x[i], 7, -680876936);"
                                       "      d = md5_ff(d, a, b, c, x[i + 1], 12, -389564586);"
                                       "      c = md5_ff(c, d, a, b, x[i + 2], 17, 606105819);"
                                       "      b = md5_ff(b, c, d, a, x[i + 3], 22, -1044525330);"
                                       "      a = md5_ff(a, b, c, d, x[i + 4], 7, -176418897);"
                                       "      d = md5_ff(d, a, b, c, x[i + 5], 12, 1200080426);"
                                       "      c = md5_ff(c, d, a, b, x[i + 6], 17, -1473231341);"
                                       "      b = md5_ff(b, c, d, a, x[i + 7], 22, -45705983);"
                                       "      a = md5_ff(a, b, c, d, x[i + 8], 7, 1770035416);"
                                       "      d = md5_ff(d, a, b, c, x[i + 9], 12, -1958414417);"
                                       "      c = md5_ff(c, d, a, b, x[i + 10], 17, -42063);"
                                       "      b = md5_ff(b, c, d, a, x[i + 11], 22, -1990404162);"
                                       "      a = md5_ff(a, b, c, d, x[i + 12], 7, 1804603682);"
                                       "      d = md5_ff(d, a, b, c, x[i + 13], 12, -40341101);"
                                       "      c = md5_ff(c, d, a, b, x[i + 14], 17, -1502002290);"
                                       "      b = md5_ff(b, c, d, a, x[i + 15], 22, 1236535329);"
                                       "      a = md5_gg(a, b, c, d, x[i + 1], 5, -165796510);"
                                       "      d = md5_gg(d, a, b, c, x[i + 6], 9, -1069501632);"
                                       "      c = md5_gg(c, d, a, b, x[i + 11], 14, 643717713);"
                                       "      b = md5_gg(b, c, d, a, x[i], 20, -373897302);"
                                       "      a = md5_gg(a, b, c, d, x[i + 5], 5, -701558691);"
                                       "      d = md5_gg(d, a, b, c, x[i + 10], 9, 38016083);"
                                       "      c = md5_gg(c, d, a, b, x[i + 15], 14, -660478335);"
                                       "      b = md5_gg(b, c, d, a, x[i + 4], 20, -405537848);"
                                       "      a = md5_gg(a, b, c, d, x[i + 9], 5, 568446438);"
                                       "      d = md5_gg(d, a, b, c, x[i + 14], 9, -1019803690);"
                                       "      c = md5_gg(c, d, a, b, x[i + 3], 14, -187363961);"
                                       "      b = md5_gg(b, c, d, a, x[i + 8], 20, 1163531501);"
                                       "      a = md5_gg(a, b, c, d, x[i + 13], 5, -1444681467);"
                                       "      d = md5_gg(d, a, b, c, x[i + 2], 9, -51403784);"
                                       "      c = md5_gg(c, d, a, b, x[i + 7], 14, 1735328473);"
                                       "      b = md5_gg(b, c, d, a, x[i + 12], 20, -1926607734);"
                                       ""
                                       "      a = md5_hh(a, b, c, d, x[i + 5], 4, -378558);"
                                       "      d = md5_hh(d, a, b, c, x[i + 8], 11, -2022574463);"
                                       "      c = md5_hh(c, d, a, b, x[i + 11], 16, 1839030562);"
                                       "      b = md5_hh(b, c, d, a, x[i + 14], 23, -35309556);"
                                       "      a = md5_hh(a, b, c, d, x[i + 1], 4, -1530992060);"
                                       "      d = md5_hh(d, a, b, c, x[i + 4], 11, 1272893353);"
                                       "      c = md5_hh(c, d, a, b, x[i + 7], 16, -155497632);"
                                       "      b = md5_hh(b, c, d, a, x[i + 10], 23, -1094730640);"
                                       "      a = md5_hh(a, b, c, d, x[i + 13], 4, 681279174);"
                                       "      d = md5_hh(d, a, b, c, x[i], 11, -358537222);"
                                       "      c = md5_hh(c, d, a, b, x[i + 3], 16, -722521979);"
                                       "      b = md5_hh(b, c, d, a, x[i + 6], 23, 76029189);"
                                       "      a = md5_hh(a, b, c, d, x[i + 9], 4, -640364487);"
                                       "      d = md5_hh(d, a, b, c, x[i + 12], 11, -421815835);"
                                       "      c = md5_hh(c, d, a, b, x[i + 15], 16, 530742520);"
                                       "      b = md5_hh(b, c, d, a, x[i + 2], 23, -995338651);"
                                       ""
                                       "      a = md5_ii(a, b, c, d, x[i], 6, -198630844);"
                                       "      d = md5_ii(d, a, b, c, x[i + 7], 10, 1126891415);"
                                       "      c = md5_ii(c, d, a, b, x[i + 14], 15, -1416354905);"
                                       "      b = md5_ii(b, c, d, a, x[i + 5], 21, -57434055);"
                                       "      a = md5_ii(a, b, c, d, x[i + 12], 6, 1700485571);"
                                       "      d = md5_ii(d, a, b, c, x[i + 3], 10, -1894986606);"
                                       "      c = md5_ii(c, d, a, b, x[i + 10], 15, -1051523);"
                                       "      b = md5_ii(b, c, d, a, x[i + 1], 21, -2054922799);"
                                       "      a = md5_ii(a, b, c, d, x[i + 8], 6, 1873313359);"
                                       "      d = md5_ii(d, a, b, c, x[i + 15], 10, -30611744);"
                                       "      c = md5_ii(c, d, a, b, x[i + 6], 15, -1560198380);"
                                       "      b = md5_ii(b, c, d, a, x[i + 13], 21, 1309151649);"
                                       "      a = md5_ii(a, b, c, d, x[i + 4], 6, -145523070);"
                                       "      d = md5_ii(d, a, b, c, x[i + 11], 10, -1120210379);"
                                       "      c = md5_ii(c, d, a, b, x[i + 2], 15, 718787259);"
                                       "      b = md5_ii(b, c, d, a, x[i + 9], 21, -343485551);"
                                       ""
                                       "      a = safe_add(a, olda);"
                                       "      b = safe_add(b, oldb);"
                                       "      c = safe_add(c, oldc);"
                                       "      d = safe_add(d, oldd);"
                                       "    }"
                                       "    return [a, b, c, d];"
                                       "  }"
                                       ""
                                       "  /*"
                                       "   * Convert a raw string to an array of little-endian words"
                                       "   * Characters >255 have their high-byte silently ignored."
                                       "   */"
                                       "  function rstr2binl(input) {"
                                       "    var i,"
                                       "      output = [];"
                                       "    output[(input.length >> 2) - 1] = undefined;"
                                       "    for (i = 0; i < output.length; i += 1) {"
                                       "      output[i] = 0;"
                                       "    }"
                                       "    for (i = 0; i < input.length * 8; i += 8) {"
                                       "      output[i >> 5] |= (input.charCodeAt(i / 8) & 0xFF) << (i % 32);"
                                       "    }"
                                       "    return output;"
                                       "  }"
                                       ""
                                       "  /*"
                                       "   * Convert an array of little-endian words to a string"
                                       "   */"
                                       "  function binl2rstr(input) {"
                                       "    var i,"
                                       "      output = '';"
                                       "    for (i = 0; i < input.length * 32; i += 8) {"
                                       "      output += String.fromCharCode((input[i >> 5] >>> (i % 32)) & 0xFF);"
                                       "    }"
                                       "    return output;"
                                       "  }"
                                       ""
                                       "  /*"
                                       "   * Convert a raw string to a hex string"
                                       "   */"
                                       "  function rstr2hex(input) {"
                                       "    var hex_tab = '0123456789abcdef',"
                                       "      output = '',"
                                       "      x,"
                                       "      i;"
                                       "    for (i = 0; i < input.length; i += 1) {"
                                       "      x = input.charCodeAt(i);"
                                       "      output += hex_tab.charAt((x >>> 4) & 0x0F) +"
                                       "        hex_tab.charAt(x & 0x0F);"
                                       "    }"
                                       "    return output;"
                                       "  }"
                                       "  return function (str) {"
                                       "    str = unescape(encodeURIComponent(str))"
                                       "    str = binl_md5(rstr2binl(str), str.length * 8)"
                                       "    str = binl2rstr(str)"
                                       "    return rstr2hex(str)"
                                       "  }"
                                       "}()"
                                       ""
                                       ""
                                       "</script>"
                                       ""
                                       "</body> "
                                       "</html>";

void w5500_packetReceive_forTLS(uint8_t sn)
{
    uint16_t len;
    if(GetSocketStatus(sn)==SOCK_ESTABLISHED)
    {
        len = GetSizeRX(sn);
        //Если пришел пустой пакет, то уходим из функции
        if(!len)
        {
           return;
        }
        else
        {
        }
        //Отобразим размер принятых данных
        printf("socket %d len_data: %d\n", sn, len);

        recv(sn, server_buffer, len);

//        printf("from client %d byte\n", len);
//        for (int i = 0;i < len; ++i)
//        {
//            printf("%X",server_buffer[i]);
//        }
//        printf("\n");

        server_buffer_sz = len;
    }

}

void w5500_packetSend_forTLS(uint8_t sn)
{
    send(sn, client_buffer, client_buffer_sz);

//    printf("from server %d byte\n", client_buffer_sz);
//    for (int i = 0;i < client_buffer_sz; ++i)
//    {
//        printf("%X",client_buffer[i]);
//    }
//    printf("\n");

    client_buffer_sz = 0;
}

/* Server attempts to read data from client. */
static int recv_server(WOLFSSL* ssl, char* buff, int sz, void* ctx)
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
static int send_server(WOLFSSL* ssl, char* buff, int sz, void* ctx)
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
    WOLFSSL_CTX* server_ctx = NULL;
    WOLFSSL*     server_ssl = NULL;

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
//Printf("-- wolfssl_server_accept --\n");
    int ret = 0;

    if (wolfSSL_accept(server_ssl) != WOLFSSL_SUCCESS) {
        if (wolfSSL_want_read(server_ssl)) {
//Printf("Server waiting for server\n");
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
static int wolfssl_send(WOLFSSL* ssl, const char* msg)
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
static int wolfssl_recv(WOLFSSL* ssl)
{
//Printf("-- wolfssl_recv --\n");

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
int tls_server_sizeTest()
{
    Printf("tls_server_sizeTest\n");
    int ret = 0;
    WOLFSSL_CTX* server_ctx = NULL;
    WOLFSSL*     server_ssl = NULL;

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
        Printf("Handshake complete\n");
    }

//    uint8_t i;
//    httpServer_init(client_buffer, server_buffer, 1, socknumlist);
//    wep_define_func();
//    display_reg_webContent_list();


//    wolfSSL_set_fd(server_ssl, 0);


    /* Send and receive HTTP messages. */




    wolfssl_recv(server_ssl);
    wolfssl_send(server_ssl, msgHTTPIndex2);



while(1)
{
//    httpServer_run(0);
    while (wolfSSL_want_read(server_ssl));
//    wolfSSL_read(server_ssl, void* data, int sz)
//    wolfssl_recv(server_ssl);

}


//    /* Dispose of SSL objects. */
//    wolfssl_free(server_ctx, server_ssl);

    /* Cleanup wolfSSL library. */
//    wolfSSL_Cleanup();

    if (ret == 0)
        Printf("Done\n");
    else
    {
        char buffer[80];
        Printf("Error: %d, %s\n", ret, wolfSSL_ERR_error_string(ret, buffer));
    }

    return (ret == 0) ? 0 : 1;
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
