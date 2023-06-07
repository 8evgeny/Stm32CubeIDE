
#include "main.h"
#define HEAP_HINT_SERVER NULL
/* I/O buffer size - wolfSSL buffers messages internally as well. */
#include "w5500.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "socket.h"
#include "loopback.h"
#include "my_function.h"

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart6;

uint8_t gDATABUF[DATA_BUF_SIZE];
wiz_NetInfo defaultNetInfo = { .mac = MAC_ADDR,
                            .ip = {192, 168, 1, 222},
                            .sn = {255,255,255,0},			
                            .gw = {192, 168, 1, 1},
                            .dns = {8,8,8,8},
                            .dhcp = NETINFO_STATIC };

unsigned char ethBuf0[ETH_MAX_BUF_SIZE];
uint8_t tmp;//int32_t ret = 0;

// задание размеров буферов W5500 для сокетов по 2 Кбайта 														
uint8_t memsize[2][8] = { {2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};

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

void print_network_information(void)
{
    wizchip_getnetinfo(&defaultNetInfo);
    printf("Mac addr:\t%02x:%02x:%02x:%02x:%02x:%02x\n\r",defaultNetInfo.mac[0],defaultNetInfo.mac[1],defaultNetInfo.mac[2],defaultNetInfo.mac[3],defaultNetInfo.mac[4],defaultNetInfo.mac[5]);
    printf("IP address:\t%d.%d.%d.%d\n\r",defaultNetInfo.ip[0],defaultNetInfo.ip[1],defaultNetInfo.ip[2],defaultNetInfo.ip[3]);
    printf("SM Mask:\t%d.%d.%d.%d\n\r",defaultNetInfo.sn[0],defaultNetInfo.sn[1],defaultNetInfo.sn[2],defaultNetInfo.sn[3]);
    printf("Gate way:\t%d.%d.%d.%d\n\r",defaultNetInfo.gw[0],defaultNetInfo.gw[1],defaultNetInfo.gw[2],defaultNetInfo.gw[3]);
    printf("DNS serv:\t%d.%d.%d.%d\n\r",defaultNetInfo.dns[0],defaultNetInfo.dns[1],defaultNetInfo.dns[2],defaultNetInfo.dns[3]);
}

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
    ctlnetwork(CN_SET_NETINFO, (void*)&defaultNetInfo);
    ctlnetwork(CN_GET_NETINFO, (void*)&defaultNetInfo);

	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
	UART_Printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
    UART_Printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",defaultNetInfo.mac[0],defaultNetInfo.mac[1],defaultNetInfo.mac[2],
          defaultNetInfo.mac[3],defaultNetInfo.mac[4],defaultNetInfo.mac[5]);
    UART_Printf("SIP: %d.%d.%d.%d\r\n", defaultNetInfo.ip[0],defaultNetInfo.ip[1],defaultNetInfo.ip[2],defaultNetInfo.ip[3]);
    UART_Printf("GAR: %d.%d.%d.%d\r\n", defaultNetInfo.gw[0],defaultNetInfo.gw[1],defaultNetInfo.gw[2],defaultNetInfo.gw[3]);
    UART_Printf("SUB: %d.%d.%d.%d\r\n", defaultNetInfo.sn[0],defaultNetInfo.sn[1],defaultNetInfo.sn[2],defaultNetInfo.sn[3]);
    UART_Printf("DNS: %d.%d.%d.%d\r\n", defaultNetInfo.dns[0],defaultNetInfo.dns[1],defaultNetInfo.dns[2],defaultNetInfo.dns[3]);
	UART_Printf("======================\r\n");
	
}







