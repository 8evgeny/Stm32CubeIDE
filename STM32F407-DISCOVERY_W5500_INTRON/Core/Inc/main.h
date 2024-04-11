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
#define UDP_SOCKET      4
#define PING_SOCKET     6
#define NUM_DIAGNOSTIC_UDP_PACKETS  50000
#define ETH_MAX_BUF_SIZE	2048
#define SERVER_PORT			8883
/* I/O buffer size - wolfSSL buffers messages internally as well. */
#define BUFFER_SIZE         2048
#define MAX_HTTPSOCK	1 //Для нового WEB-сервера

#define EEPROM_CS_Pin GPIO_PIN_2
#define EEPROM_CS_GPIO_Port GPIOE
#define EEPROM_WP_Pin GPIO_PIN_3
#define EEPROM_WP_GPIO_Port GPIOE
#define EEPROM_HOLD_Pin GPIO_PIN_1
#define EEPROM_HOLD_GPIO_Port GPIOE

#ifndef TLS_ON
#define LOCAL_PORT_WEB 80
#else
#define LOCAL_PORT_WEB 443
#endif
#define LOCAL_PORT_UDP 3000

#ifdef DATA_IN_CCM
#define CCMRAMDATA __attribute__((section (".ccmram")))
#endif
#ifndef DATA_IN_CCM
#define CCMRAMDATA
#endif
#define FIRMWARESECTION __attribute__((section (".firmware_data_for_bootloader")))
/*
EEPROM I2C : ATMEL 24C1024 (24C256)
32768 byte 0000 - 7FFF
1MHz (2.5V, 2.7V, 5.0V) compatibility
512 pages of 64-bytes each
0000 - 00FF   256b   IP settings
0100 - 010F   16b    длина index.html (при чтении с карты - старый web сервер)
0110 - 011F   16b    длина main.html  (при чтении с карты - старый web сервер)
0120 - 019F   128b   mac
01A0 - 01AF   16b    Признак чистой EEPROM (если FF - пишем нули и пишем параметры и mac по умолчанию)
01B0 - 02FF
0300 - 03FF   256b   simple test eeprom

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
7800 - 7BFF   31k   резерв
7C00 - 7FFF   32k
*/

#define  ipSettingAdressInEEPROM 0x0000
#define  settingsLen             93
#define  indexLenAdressInEEPROM  0x0100
#define  mainLenAdressInEEPROM   0x0110
#define  indexAdressInEEPROM     0x0400
#define  mainAdressInEEPROM      0x2800
#define  macAdressInEEPROM       0x0120
#define  markEEPROMclear         0x01A0
//#define  simpleTestEEPROMadress  0x7C00
//#define  simpleTestEEPROMadress  0x01A0 //Так длинные значения не пишет
#define  simpleTestEEPROMadress  0x300

#define  ipSettingAdressInSPIEEPROM 0x0000
#define  settingsLenInSPI           93
#define  macAdressInSPIEEPROM       0x0120
#define  markEEPROMSPIclear         0x01A0
#define  resetTwiceFlag             0x0200
#define  netDiagnosticFlag          0x0300
#define  pingFlag                   0x0400
/*25LC1024：1Mbit= 1024Kbit =128KB  = 512*256B = 131072 X 8bit
 Page 1     Address: 00000-000FF length 256b    IP settings
 Page 2     Address: 00100-001FF length 256b
 Page 3     Address: 00200-002FF length 256b    resetTwiceFlag
 Page 4     Address: 00300-003FF length 256b    netDiagnosticFlag
 Page 5     Address: 00400-004FF length 256b    pingFlag
`
`
`
 Page 510   Address: 1FD00-1FDFF length 256b
 Page 511   Address: 1FE00-1FEFF length 256b
 Page 512   Address: 1F000-1FFFF length 256b
*/


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
#include "stdio.h"

#ifndef enable_BIG_PACKET
#define MAX_PACKET_LEN 48  //12*4
#endif
#ifdef enable_BIG_PACKET
//#define MAX_PACKET_LEN 1152 //288*4
#define MAX_PACKET_LEN 80 //20*4
#endif
#ifdef enable_BUFFER
#define BUF_PACKET_SIZE 100
#endif

#define TEST_DATA test6
enum netDiagnostic{
    netDiagnosticON,
    netDiagnosticOFF
};
enum pingMode{
    pingON,
    pingOFF
};
enum ABONENTorBASE{
    BASE,
    ABONENT
};
enum handshakeState{
    hanshakeON,
    hanshakeOFF
};
enum webState{
    webON,
    webOFF
};
enum LOGIN_STATE{
    loginON,
    loginOFF
};
enum PASSWORD_STATE{
    PasswordON,
    PasswordOFF,
    PasswordIgnore
};
enum netMode{
    UDP,
    TCP
};
enum Commands{
    REBOOT,
    NET_DIAGNOSTIC,
    NO_COMMAND
};
enum stateBridge{
    CONNECTION_YES,
    CONNECTION_NO,
};
enum stateCompareDataInChannel{
    NO_ERRORS,
    ERROR_1,
    ERROR_2,
    ERROR_3,
    ERROR_4,
    ERROR_5,
    ERROR_6,
    ERROR_7,
    ERROR_8,
    ERROR_9,
    ERROR_10,
    };

void sendPackets(uint8_t, uint8_t* , uint16_t );
void receivePackets(uint8_t, uint8_t* , uint16_t );
void 	wizchip_cs_select(void);
void  wizchip_cs_deselect(void);
uint8_t wizchip_spi_readbyte(void);
void 	wizchip_spi_writebyte(uint8_t wb);
void 	wizchip_spi_readburst(uint8_t* pBuf, uint16_t len);
void 	wizchip_spi_writeburst(uint8_t* pBuf, uint16_t len);
void 	wizchip_cris_enter(void);
void 	wizchip_cris_exit(void);
void UART_Printf(const char* fmt, ...);
void Printf(const char* fmt, ...);
void network_init(void);
void  wizchip_select(void);
void  wizchip_deselect(void);
void  wizchip_write(uint8_t wb);
uint8_t wizchip_read(void);
void Chip_selection_call_back(void);
void wizchip_initialize(void);
void reboot();
void checkLogin(char* buf);
void checkPassword(char* buf);
void setNewHostIP(char * buf);
void setNewMaskIP(char * buf);
void setNewGateIP(char * buf);
void setNewDestIP(char * buf);
void setNewPassword(char * buf);
int convertHexToDecimal();
void testSPI_EEPROM();
void printAllChannel(uint8_t data[MAX_PACKET_LEN]);
void print_1_Channel(uint8_t data[MAX_PACKET_LEN]);
void print_2_Channel(uint8_t data[MAX_PACKET_LEN]);
void print_3_Channel(uint8_t data[MAX_PACKET_LEN]);
void print_3_Channel_control(uint8_t data[MAX_PACKET_LEN]);
void print_4_Channel(uint8_t data[MAX_PACKET_LEN]);
void create_2_channelDataForControl(uint8_t dataFromBase[MAX_PACKET_LEN], uint8_t dataForControl[MAX_PACKET_LEN / 4]);
uint8_t compare_data_in_Channel(uint8_t data[MAX_PACKET_LEN / 4], uint8_t trueData[MAX_PACKET_LEN / 4]);
void print_2_Channel_control(uint8_t data[MAX_PACKET_LEN / 4]);
uint8_t checkCommands(uint8_t dataToDx[MAX_PACKET_LEN]);
void printWiznetReg(void);
void commandFromWebNetDiagnostic();
uint8_t checkNetDiagnosticMode();
uint8_t checkPingMode();
void netDiagnosticBase();
void netDiagnosticAbon();
void indicateSend(uint16_t numON, uint16_t numOFF);
void indicateReceive(uint16_t numON, uint16_t numOFF);
void prepeareDataToAbonent(uint8_t * dataToAbon, uint32_t numPacket, uint32_t currTime);
void printTestNetData(uint8_t data[MAX_PACKET_LEN]);
void prepeareAnswerToBase(uint8_t * dataFromBase, uint32_t currTime);
void analiseDataFromAbonent(uint8_t * dataFromAbon, uint32_t currTime);
void commandFromWebPing();
void pingCheck();
void workInPingMode();
uint32_t htonl(uint32_t net);
uint16_t htons(uint16_t net);
uint64_t htonll(uint64_t net);
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
#define PC14_OSC32_IN_Pin GPIO_PIN_14
#define PC14_OSC32_IN_GPIO_Port GPIOC
#define PH0_OSC_IN_Pin GPIO_PIN_0
#define PH0_OSC_IN_GPIO_Port GPIOH
#define PH1_OSC_OUT_Pin GPIO_PIN_1
#define PH1_OSC_OUT_GPIO_Port GPIOH
#define OTG_FS_PowerSwitchOn_Pin GPIO_PIN_0
#define OTG_FS_PowerSwitchOn_GPIO_Port GPIOC
#define write_MAC_Pin GPIO_PIN_4
#define write_MAC_GPIO_Port GPIOC
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
#define DEBUG1_Pin GPIO_PIN_3
#define DEBUG1_GPIO_Port GPIOD
#define DEBUG2_Pin GPIO_PIN_4
#define DEBUG2_GPIO_Port GPIOD
#define DEBUG3_Pin GPIO_PIN_5
#define DEBUG3_GPIO_Port GPIOD
#define HOLD_EEPROM_Pin GPIO_PIN_1
#define HOLD_EEPROM_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
#define  blue_blink HAL_GPIO_WritePin(GPIOD, Blue_Led_Pin, GPIO_PIN_SET); HAL_Delay(100); HAL_GPIO_WritePin(GPIOD, Blue_Led_Pin, GPIO_PIN_RESET); HAL_Delay(100);
#define  red_blink HAL_GPIO_WritePin(GPIOD, Red_Led_Pin, GPIO_PIN_SET); HAL_Delay(100); HAL_GPIO_WritePin(GPIOD, Red_Led_Pin, GPIO_PIN_RESET); HAL_Delay(100);
#define  green_blink HAL_GPIO_WritePin(GPIOD, Green_Led_Pin, GPIO_PIN_SET); HAL_Delay(100); HAL_GPIO_WritePin(GPIOD, Green_Led_Pin, GPIO_PIN_RESET); HAL_Delay(100);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
