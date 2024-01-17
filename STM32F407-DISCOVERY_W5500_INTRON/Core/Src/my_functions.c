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
#include "SEGGER_RTT.h"
#include "socket.h"
#include "net.h"
#include "httpServer.h"

extern uint8_t SEGGER;
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
        if (SEGGER)
            SEGGER_RTT_PutChar(0, str[i]);
    }
    return len;
}

void printAllChannel(uint8_t data[MAX_PACKET_LEN]) {
    SEGGER_RTT_SetTerminal(5);
    SEGGER_RTT_printf(0, "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
                         "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
                         "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
                         "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
                         "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
                         "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
                         "\r\n",
                      data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
                      data[8],data[9], data[10], data[11], data[12], data[13], data[14], data[15],
                      data[16], data[17], data[18], data[19], data[20], data[21], data[22], data[23],
                      data[24], data[25], data[26], data[27], data[28], data[29], data[30], data[31],
                      data[32], data[33], data[34], data[35], data[36], data[37], data[38], data[39],
                      data[40], data[41], data[42], data[43], data[44], data[45], data[46], data[47]
                      );
    SEGGER_RTT_SetTerminal(0);

}

void print_1_Channel(uint8_t data[MAX_PACKET_LEN]) {
    SEGGER_RTT_SetTerminal(1);
    SEGGER_RTT_printf(0, "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X \r\n",
                      data[0], data[4], data[8], data[12], data[16], data[20],
                      data[24], data[28], data[32], data[36], data[40], data[44]
                      );
    SEGGER_RTT_SetTerminal(0);
}

void print_2_Channel(uint8_t data[MAX_PACKET_LEN]) {
    SEGGER_RTT_SetTerminal(2);
    SEGGER_RTT_printf(0, "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X \r\n",
                      data[1], data[5], data[9], data[13], data[17], data[21],
                      data[25], data[29], data[33], data[37], data[41], data[45]
                      );
    SEGGER_RTT_SetTerminal(0);
}

void print_3_Channel(uint8_t data[MAX_PACKET_LEN]) {
    SEGGER_RTT_SetTerminal(3);
    SEGGER_RTT_printf(0, "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X \r\n",
                      data[2], data[6], data[10], data[14], data[18], data[22],
                      data[26], data[30], data[34], data[38], data[42], data[46]
                      );
    SEGGER_RTT_SetTerminal(0);
}

void print_3_Channel_control(uint8_t data[MAX_PACKET_LEN / 4]) {
    SEGGER_RTT_SetTerminal(7);
    SEGGER_RTT_printf(0, "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X \r\n",
                      data[0], data[1], data[2], data[3], data[4], data[5],
                      data[6], data[7], data[8], data[9], data[10], data[11]
                      );
    SEGGER_RTT_SetTerminal(0);
}

void print_4_Channel(uint8_t data[MAX_PACKET_LEN]) {
    SEGGER_RTT_SetTerminal(4);
    SEGGER_RTT_printf(0, "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X \r\n",
                      data[3], data[7], data[11], data[15], data[19], data[23],
                      data[27], data[31], data[35], data[39], data[43], data[47]
                      );
    SEGGER_RTT_SetTerminal(0);
}

void create_3_channelDataForControl(uint8_t dataFromBase[MAX_PACKET_LEN], uint8_t dataForControl[MAX_PACKET_LEN / 4]){
    for (int i = 0; i < 12; ++i) {
        strncpy((char *)dataForControl + i , (const char*)dataFromBase + 2 + i * 4 , 1);
    }
}

uint8_t check_3_Channel(uint8_t data[MAX_PACKET_LEN / 4], uint8_t trueData[MAX_PACKET_LEN / 4]) {
    if ( strcmp ((const char*)data, (const char*)trueData) == 0){
        return 0;
    }
    return 1;
}
