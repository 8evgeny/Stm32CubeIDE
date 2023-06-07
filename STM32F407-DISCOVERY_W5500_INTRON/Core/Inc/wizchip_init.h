#ifndef __WIZCHIP_INIT_H__
#define __WIZCHIP_INIT_H__


#include "main.h"
#include "wizchip_conf.h"


/* CS */
extern SPI_HandleTypeDef hspi1;
#define WIZCHIP_SPI  			hspi1
#define WIZCHIP_CS_PIN			GPIO_PIN_11
#define WIZCHIP_CS_PORT			GPIOD

void WIZCHIPInitialize();
void csEnable(void);
void csDisable(void);
//void spiWriteByte(uint8_t tx);
//uint8_t spiReadByte(void);
void wizchip_spi_readburst(uint8_t* pBuf, uint16_t len);
void wizchip_spi_writeburst(uint8_t* pBuf, uint16_t len);
void wizchip_cs_deselect(void);
void wizchip_cs_select(void);
uint8_t wizchip_spi_readbyte(void);
void wizchip_spi_writebyte(uint8_t wb);
void wizchip_cris_enter(void);
void wizchip_cris_exit(void);
#endif
