/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#define delayUS_ASM(us) do {                           \
asm volatile ("MOV R0,%[loops]\n                       \
              1: \n                                    \
              SUB R0, #1\n                             \
              CMP R0, #0\n                             \
              BNE 1b \t"                               \
              : : [loops] "r" (34*us) : "memory"       \
              );                                       \
} while(0)
#define HTTP_SOCKET     0
#define ETH_MAX_BUF_SIZE	2048
#define SERVER_PORT			8883
/* I/O buffer size - wolfSSL buffers messages internally as well. */
#define BUFFER_SIZE         2048
#define MAX_HTTPSOCK	4
#ifdef INTRON

#define MAC_ADDR {0x00,0x15,0x42,0xBF,0xF0,0x51}
#endif
#ifndef INTRON
#define MAC_ADDR {0x00,0x15,0x42,0xBF,0xF0,0x52}
#endif

#define EEPROM_CS_Pin GPIO_PIN_2
#define EEPROM_CS_GPIO_Port GPIOE
#define EEPROM_WP_Pin GPIO_PIN_3
#define EEPROM_WP_GPIO_Port GPIOE
#define EEPROM_HOLD_Pin GPIO_PIN_1
#define EEPROM_HOLD_GPIO_Port GPIOE

/*
EEPROM I2C : ATMEL 24C256
32768 byte 0000 - 7FFF
1MHz (2.5V, 2.7V, 5.0V) compatibility
512 pages of 64-bytes each
0000 - 00FF   256b   IP settings
0100 - 010F   16b    длина index.html
0110 - 011F   16b    длина main.html
0120 - 019F   128b   mac
01A0 - 03FF   резерв
0400 - 07FF   2k   index.html
0800 - 0BFF   3k
0C00 - 0FFF   4k
1000 - 13FF   5K
1400 - 17FF   6k
1800 - 1BFF   7k
1C00 - 1FFF   8k
2000 - 23FF   9K
2400 - 27FF   10k   index.html
2800 - 2BFF   11k   main.html
2C00 - 2FFF   12k
3000 - 33FF   13K
3400 - 37FF   14k
3800 - 3BFF   15k
3C00 - 3FFF   16k
4000 - 43FF   17K
4400 - 47FF   18k
4800 - 4BFF   19k
4C00 - 4FFF   20k
5000 - 53FF   21K
5400 - 57FF   22k
5800 - 5BFF   23k
5C00 - 5FFF   24k
6000 - 63FF   25K
6400 - 67FF   26k   main.html
6800 - 6BFF   27k   резерв
6C00 - 6FFF   28k
7000 - 73FF   29K
7400 - 77FF   30k
7800 - 7BFF   31k
7C00 - 7FFF   32k
*/

#define  ipSettingAdressInEEPROM 0x0000
#define  settingsLen 93
#define  indexLenAdressInEEPROM 0x0100
#define  mainLenAdressInEEPROM 0x0110
#define  indexAdressInEEPROM 0x0400
#define  mainAdressInEEPROM 0x2800
#define  macAdressInEEPROM 0x0120

#include "types.h"

/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define MAX_PACKET_LEN 48
void 	wizchip_cs_select(void);
void  wizchip_cs_deselect(void);
uint8_t wizchip_spi_readbyte(void);
void 	wizchip_spi_writebyte(uint8_t wb);
void 	wizchip_spi_readburst(uint8_t* pBuf, uint16_t len);
void 	wizchip_spi_writeburst(uint8_t* pBuf, uint16_t len);
void 	wizchip_cris_enter(void);
void 	wizchip_cris_exit(void);
void reboot();
#include "stdio.h"
#include "my_function.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CS_EEPROM_Pin GPIO_PIN_2
#define CS_EEPROM_GPIO_Port GPIOE
#define WP_EEPROM_Pin GPIO_PIN_3
#define WP_EEPROM_GPIO_Port GPIOE
#define PC14_OSC32_IN_Pin GPIO_PIN_14
#define PC14_OSC32_IN_GPIO_Port GPIOC
#define PC15_OSC32_OUT_Pin GPIO_PIN_15
#define PC15_OSC32_OUT_GPIO_Port GPIOC
#define PH0_OSC_IN_Pin GPIO_PIN_0
#define PH0_OSC_IN_GPIO_Port GPIOH
#define PH1_OSC_OUT_Pin GPIO_PIN_1
#define PH1_OSC_OUT_GPIO_Port GPIOH
#define OTG_FS_PowerSwitchOn_Pin GPIO_PIN_0
#define OTG_FS_PowerSwitchOn_GPIO_Port GPIOC
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define INT_Pin GPIO_PIN_8
#define INT_GPIO_Port GPIOD
#define Green_Led_Pin GPIO_PIN_12
#define Green_Led_GPIO_Port GPIOD
#define Orange_Led_Pin GPIO_PIN_13
#define Orange_Led_GPIO_Port GPIOD
#define Red_Led_Pin GPIO_PIN_14
#define Red_Led_GPIO_Port GPIOD
#define Blue_Led_Pin GPIO_PIN_15
#define Blue_Led_GPIO_Port GPIOD
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define HOLD_EEPROM_Pin GPIO_PIN_1
#define HOLD_EEPROM_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
