/*
 * wizchip_init.c
 *
 *  Created on: Nov 28, 2019
 *      Author: becky
 */

#include "wizchip_init.h"
extern void Printf(const char* fmt, ...);

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
        Printf("WIZCHIP Initialized fail.\r\n", 1, 10);
	  return;
	}
	/* PHY link status check */
	do {
		if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
		{
            Printf("Unknown PHY Link status.\r\n", 1, 10);
		  return;
		}
	} while (tmp == PHY_LINK_OFF);
}

//void csEnable(void)
//{
//	HAL_GPIO_WritePin(WIZCHIP_CS_PORT, WIZCHIP_CS_PIN, GPIO_PIN_RESET);
//}

//void csDisable(void)
//{
//	HAL_GPIO_WritePin(WIZCHIP_CS_PORT, WIZCHIP_CS_PIN, GPIO_PIN_SET);
//}

//void spiWriteByte(uint8_t tx)
//{
//	uint8_t rx;
//	HAL_SPI_TransmitReceive(&WIZCHIP_SPI, &tx, &rx, 1, 10);
//}

//uint8_t spiReadByte(void)
//{
//	uint8_t rx = 0, tx = 0xFF;
//	HAL_SPI_TransmitReceive(&WIZCHIP_SPI, &tx, &rx, 1, 10);
//	return rx;
//}

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
