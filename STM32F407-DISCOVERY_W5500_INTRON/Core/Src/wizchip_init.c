/*
 * wizchip_init.c
 *
 *  Created on: Nov 28, 2019
 *      Author: becky
 */

#include "wizchip_init.h"
extern void Printf(const char* fmt, ...);

uint8_t gDATABUF[DATA_BUF_SIZE];
wiz_NetInfo defaultNetInfo = { .mac = {0x00, 0x00, 0x00,0x00, 0x00, 0x00},
                            .ip = {0, 0, 0, 0},
                            .sn = {0,0,0,0},
                            .gw = {0, 0, 0, 0},
                            .dns = {0,0,0,0},
                            .dhcp = NETINFO_STATIC };

void WIZCHIPInitialize(){
    wizchip_cs_deselect();
    reg_wizchip_cs_cbfunc(wizchip_cs_select, wizchip_cs_deselect);
    reg_wizchip_spi_cbfunc(wizchip_spi_readbyte, wizchip_spi_writebyte);
    reg_wizchip_spiburst_cbfunc(wizchip_spi_readburst, wizchip_spi_writeburst);

	uint8_t tmp;
	//w5500, w5200
#if _WIZCHIP_SOCK_NUM_ == 8
	uint8_t memsize[2][8] = { {2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
#else
	uint8_t memsize[2][4] = { {2,2,2,2},{2,2,2,2}};
#endif
	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
	{
		//myprintf("WIZCHIP Initialized fail.\r\n");
        printf("WIZCHIP Initialized fail.\r\n", 1, 10);
	  return;
	}
	/* PHY link status check */
	do {
		if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
		{
            printf("Unknown PHY Link status.\r\n", 1, 10);
		  return;
		}
	} while (tmp == PHY_LINK_OFF);
    printf("link_ON\n");
}

void wizchip_spi_readburst(uint8_t* pBuf, uint16_t len)
{
    HAL_SPI_Receive(&hspi1, pBuf, len , 1000);
}

void wizchip_spi_writeburst(uint8_t* pBuf, uint16_t len)
{
    HAL_SPI_Transmit(&hspi1, pBuf, len , 1000);
}

void wizchip_cs_deselect(void)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
}

void wizchip_cs_select(void)
{
     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);
}

uint8_t wizchip_spi_readbyte(void)
{
    uint8_t wb=0xFF;
    HAL_SPI_Receive(&hspi1, &wb, 1, 1000);
    return	wb;
}

void wizchip_spi_writebyte(uint8_t wb)
{
    HAL_SPI_Transmit(&hspi1, &wb, 1, 1000);
}

void wizchip_cris_enter(void)
{
}

void wizchip_cris_exit(void)
{
}

void print_network_information(void)
{
    wizchip_getnetinfo(&defaultNetInfo);
    printf("Mac addr:\t%02X:%02X:%02X:%02X:%02X:%02X\n\r",defaultNetInfo.mac[0],defaultNetInfo.mac[1],defaultNetInfo.mac[2],defaultNetInfo.mac[3],defaultNetInfo.mac[4],defaultNetInfo.mac[5]);
    printf("IP address:\t%d.%d.%d.%d\n\r",defaultNetInfo.ip[0],defaultNetInfo.ip[1],defaultNetInfo.ip[2],defaultNetInfo.ip[3]);
    printf("SM Mask:\t%d.%d.%d.%d\n\r",defaultNetInfo.sn[0],defaultNetInfo.sn[1],defaultNetInfo.sn[2],defaultNetInfo.sn[3]);
    printf("Gate way:\t%d.%d.%d.%d\n\r",defaultNetInfo.gw[0],defaultNetInfo.gw[1],defaultNetInfo.gw[2],defaultNetInfo.gw[3]);
    printf("DNS serv:\t%d.%d.%d.%d\n\r",defaultNetInfo.dns[0],defaultNetInfo.dns[1],defaultNetInfo.dns[2],defaultNetInfo.dns[3]);
}
