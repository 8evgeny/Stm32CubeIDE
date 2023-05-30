/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ssl.h"
#include "certs.h"
#define HEAP_HINT_SERVER NULL
/* I/O buffer size - wolfSSL buffers messages internally as well. */
#define BUFFER_SIZE           512
#include "w5500.h"
char tempBufRead[2048];
int indexTempBufRead = 0;
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "socket.h"
#include "loopback.h"
#include "my_function.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */
uint8_t gDATABUF[DATA_BUF_SIZE];
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
                            .ip = {192, 168, 1, 197},
                            .sn = {255,255,255,0},			
                            .gw = {192, 168, 1, 1},
                            .dns = {0,0,0,0},
                            .dhcp = NETINFO_STATIC };

uint8_t tmp;//int32_t ret = 0;
// задание размеров буферов W5500 для сокетов по 2 Кбайта 														
uint8_t memsize[2][8] = { {2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};													
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Buffer for client connection to allocate dynamic memory from. */
static unsigned char client_buffer[BUFFER_SIZE];
static int client_buffer_sz = 0;
/* Buffer for server connection to allocate dynamic memory from. */
static unsigned char server_buffer[BUFFER_SIZE];
static int server_buffer_sz = 0;



/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void UART_Printf(const char* fmt, ...) {
    char buff[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    HAL_UART_Transmit_DMA(&huart6, (uint8_t*)buff, strlen(buff)
//                          ,HAL_MAX_DELAY
                          );
    va_end(args);
}

void Printf(const char* fmt, ...) {
    char buff[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    HAL_UART_Transmit(&huart6, (uint8_t*)buff, strlen(buff)
                          ,HAL_MAX_DELAY                          );
    va_end(args);
}

/* Chip selection call back */
void Chip_selection_call_back(void)
{
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_select);  // CS must be tried with LOW.
#else
   #if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
      #error "Unknown _WIZCHIP_IO_MODE_"
   #else
      reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
   #endif
#endif
    /* SPI Read & Write callback function */
    reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);

    /* wizchip initialize*/
}

void wizchip_initialize(void)
{
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
    {
       UART_Printf("WIZCHIP Initialized fail.\r\n");
       while(1);
    }
		else{UART_Printf("WIZCHIP Initialized OK.\r\n");}

    /* PHY link status check */
    do
    {
       if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
          UART_Printf("Unknown PHY Link stauts.\r\n");
    }while(tmp == PHY_LINK_OFF);
}

void network_init(void)
{
   uint8_t tmpstr[6];
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);

	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
	UART_Printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	UART_Printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
		  gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	UART_Printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	UART_Printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	UART_Printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	UART_Printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	UART_Printf("======================\r\n");
	
}




/* Server attempts to read data from client. */
static int recv_server(WOLFSSL* ssl, char* buff, int sz, void* ctx)
{
//    Printf("\n-- recv_server --\n");

//    if (server_buffer_sz > 0) {
//        if (sz > server_buffer_sz)
//            sz = server_buffer_sz;
//        XMEMCPY(buff, server_buffer, sz);
//        if (sz < server_buffer_sz) {
//            XMEMMOVE(server_buffer, server_buffer + sz, server_buffer_sz - sz);
//        }
//        server_buffer_sz -= sz;
//    }
//    else
//        sz = WOLFSSL_CBIO_ERR_WANT_READ;

//    return sz;

    uint16_t point;
    uint16_t len;

      if(GetSocketStatus(0)==SOCK_ESTABLISHED)
      {
          len = GetSizeRX(0);
          //Если пришел пустой пакет, то уходим из функции
          if(!len) return 0;
          //Отобразим размер принятых данных
          Printf("socket %d len_packet:0x%04X\r\n",0,len);

          //указатель на начало чтения приёмного буфера
          point = GetReadPointer(0);
          w5500_readSockBuf(0, point, (uint8_t*)tempBufRead, len);
          if (sz < len)
          {
              if (indexTempBufRead == 0) //Не было ранее частей
              {
                  XMEMCPY(buff, tempBufRead, sz);
                  return sz;
              }
              else //в буфере что-то есть - завершаем
              {
                  XMEMCPY(buff, tempBufRead + indexTempBufRead, len);
                  indexTempBufRead = 0;
                  return sz;
              }

           }
          else // запрошено данных больше чем получено
          {
              XMEMCPY(buff, tempBufRead + indexTempBufRead, len);
              indexTempBufRead += len;
              return WOLFSSL_CBIO_ERR_WANT_READ;
          }
      }
      return 0;
}

/* Server attempts to write data to client. */
static int send_server(WOLFSSL* ssl, char* buff, int sz, void* ctx)
{
    Printf("\n-- send_server --\n");
//    if (client_buffer_sz < BUFFER_SIZE)
//    {
//        if (sz > BUFFER_SIZE - client_buffer_sz)
//            sz = BUFFER_SIZE - client_buffer_sz;
//        XMEMCPY(client_buffer + client_buffer_sz, buff, sz);
//        client_buffer_sz += sz;
//    }
//    else
//        sz = WOLFSSL_CBIO_ERR_WANT_WRITE;
//    return sz;
}

/* Create a new wolfSSL server with a certificate for authentication. */
static int wolfssl_server_new(WOLFSSL_CTX** ctx, WOLFSSL** ssl)
{
    Printf("\n-- wolfssl_server_new --\n");
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
    Printf("\n-- wolfssl_server_accept --\n");
    int ret = 0;

    if (wolfSSL_accept(server_ssl) != WOLFSSL_SUCCESS)
    {
        if (wolfSSL_want_read(server_ssl)) {
            Printf("Server waiting for server\n");
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
    Printf("\n-- wolfssl_send --\n");
    int ret;

    Printf("%s", msg);
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
    Printf("\n-- wolfssl_recv --\n");
    int ret;
    byte reply[256];

    ret = wolfSSL_read(ssl, reply, sizeof(reply)-1);
    if (ret > 0) {
        reply[ret] = '\0';
        Printf("%s", reply);
        ret = 0;
    }

    return ret;
}

/* Free the WOLFSSL object and context. */
static void wolfssl_free(WOLFSSL_CTX* ctx, WOLFSSL* ssl)
{
    Printf("\n-- wolfssl_free --\n");
    if (ssl != NULL)
        wolfSSL_free(ssl);
    if (ctx != NULL)
        wolfSSL_CTX_free(ctx);
}


void tlsProcess()
{
    Printf("-- tlsProcess --\n");
    int ret = 0;
    WOLFSSL_CTX* server_ctx = NULL;
    WOLFSSL*     server_ssl = NULL;
    wolfSSL_Init();
    ret = wolfssl_server_new(&server_ctx, &server_ssl);
    while (ret == 0)
    {
        ret = wolfssl_server_accept(server_ssl);
        if (ret == 0 && wolfSSL_is_init_finished(server_ssl))
        {
            break;
        }
    }
    if (ret == 0)
    {
        Printf("Handshake complete\n");
    }




}


/* USER CODE END 0 */

