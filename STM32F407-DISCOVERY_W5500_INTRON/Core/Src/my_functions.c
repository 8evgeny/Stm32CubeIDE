#include "main.h"
#include "ssl.h"
#include "certs.h"
#define HEAP_HINT_SERVER NULL
/* I/O buffer size - wolfSSL buffers messages internally as well. */
#include "w5500.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "my_function.h"
#include "wizchip_init.h"
#include "SSLInterface.h"
extern UART_HandleTypeDef huart6;

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

int _write(int fd, char *str, int len)
{
    for(int i=0; i<len; i++)
    {
        HAL_UART_Transmit(&huart6, (uint8_t *)&str[i], 1, 0xFFFF);
    }
    return len;
}

void polarSSLTest()
{
    //Отображает входящее подключение
//    while(getSn_SR(HTTP_SOCKET) == SOCK_LISTEN)
//    {
//            HAL_Delay(2);
//    }
//    printf("Input connection\r\n");
//    if(getSn_SR(HTTP_SOCKET) != SOCK_ESTABLISHED) printf("Error socket status\r\n");
//    uint8_t rIP[4];
//    getSn_DIPR(HTTP_SOCKET,  rIP);
//    printf("IP:  %d.%d.%d.%d\r\n", rIP[0], rIP[1], rIP[2], rIP[3]);


    int len, server_fd = 0;
    unsigned int ret = 0;
    wiz_tls_context tlsContext;
    /*  initialize ssl context  */
    ret = wiz_tls_init(&tlsContext,&server_fd);
    printf("init [%d] \r\n", ret);


#if defined(MBEDTLS_DEBUG_C)
    printf( "  . Performing the SSL/TLS handshake..." );
#endif

    while(getSn_SR(HTTP_SOCKET) == SOCK_LISTEN)
    {
            HAL_Delay(2);
    }
    printf("Input connection\r\n");
    if(getSn_SR(HTTP_SOCKET) != SOCK_ESTABLISHED) printf("Error socket status\r\n");
    uint8_t rIP[4];
    getSn_DIPR(HTTP_SOCKET,  rIP);
    printf("IP:  %d.%d.%d.%d\r\n", rIP[0], rIP[1], rIP[2], rIP[3]);

    ret = mbedtls_ssl_handshake( tlsContext.ssl );

//    while( ( ret = mbedtls_ssl_handshake( tlsContext.ssl ) ) != 0 )
//    {
//        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
//        {

//            printf( " failed\n\r  ! mbedtls_ssl_handshake returned %d: %s\n\r", ret );

//        }
//    }
printf( " OK ..." );

}








