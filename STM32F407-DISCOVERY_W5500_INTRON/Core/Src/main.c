/* USER CODE BEGIN Header */

#include "socket.h"
#include "w5500.h"
#include "net.h"
#include "loopback.h"
#include "my_function.h"
#include "wizchip_init.h"
//#include "SSLInterface.h"
#include "httpServer.h"
#include "webpage.h"
#include "spi_eeprom.h"
#include "eeprom.h"

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stdio.h"
#include "stdarg.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

extern void FsForEeprom_test();
extern void littleFsInit();
void sendPackets(uint8_t, uint8_t* , uint16_t );
void receivePackets(uint8_t, uint8_t* , uint16_t );
extern int tls_client_serverTest();
extern void tls_server_Handshake();
extern void polarSSLTest();
extern void bearSSLTest();
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define I2C_REQUEST_WRITE                       0x00
#define I2C_REQUEST_READ                        0x01
#define SLAVE_OWN_ADDRESS                       0xA0
uint32_t count = 0;
uint8_t sdCartOn = 1;
char *pindex;
char *pmain;
char *psettingsIP;
uint8_t ABONENT_or_BASE;
uint8_t HANDSHAKE = 0;
#ifdef LFS
extern lfs_t lfs;
extern lfs_file_t file;
#endif
//uint8_t num_block_index = 1;
//uint8_t num_block_main = 8;
//char indexLen[8];
#define WRITE_ONCE_TO_EEPROM 1024
uint16_t local_port_web = LOCAL_PORT_WEB;
uint16_t local_port_udp = LOCAL_PORT_UDP;
uint8_t destipHOST[4] = {192,168,1,11}; //для тестов

uint8_t loginOK = 0;
uint8_t passwordOK = 0;
extern int8_t http_disconnect(uint8_t sn);

uint8_t txCyclon[MAX_PACKET_LEN];
uint8_t rxCyclon[MAX_PACKET_LEN];

uint8_t test1[MAX_PACKET_LEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                                 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                                0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
uint8_t test2[MAX_PACKET_LEN] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
uint8_t test3[MAX_PACKET_LEN] = {0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
                                0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
                                0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0xff};
uint8_t test4[MAX_PACKET_LEN] = {0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,
                                0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,
                                0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17};
uint8_t test5[MAX_PACKET_LEN] = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
                                0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
                                0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99};
uint8_t test6[MAX_PACKET_LEN] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                                0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                                0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
uint8_t test7[MAX_PACKET_LEN] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0xff, 0xff, 0xff, 0xff, 0xaa, 0xaa, 0xaa, 0xaa,
                                0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0xff, 0xff, 0xff, 0xff, 0xaa, 0xaa, 0xaa, 0xaa,
                                0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0xff, 0xff, 0xff, 0xff, 0xaa, 0xaa, 0xaa, 0xaa};
uint8_t zeroStr[MAX_PACKET_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint32_t num_send = 0;
uint32_t num_rcvd = 0;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RNG_HandleTypeDef hrng;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart6_tx;

/* USER CODE BEGIN PV */
extern uint8_t RxBuffer[256];
extern uint8_t EEPROM_StatusByte;

uint8_t TxBuffer[256] = "__12345COOL!!!1234512345qwertasdfgh";
uint8_t capture = 0;
uint8_t ipaddr[4];
uint8_t ipgate[4];
uint8_t ipmask[4];
char MD5[32];
#ifndef   NEW_HTTP_SERVER
uint32_t indexLen;
uint32_t mainLen;
#endif
uint8_t destip[4];
uint8_t macaddr[6]={0x00}/*MAC_ADDR*/;
extern wiz_NetInfo defaultNetInfo;
#define DATA_BUF_SIZE   2048
uint8_t RX_BUF_WEB[DATA_BUF_SIZE];
uint8_t TX_BUF_WEB[DATA_BUF_SIZE];
uint8_t socknumlist[] = {0, 1, 2, 3};
extern char host_IP[16]; //для http Сервера
extern char dest_IP[16];
extern char gate_IP[16];
extern char mask_IP[16];
extern char mac[18];
FATFS fs;
FIL fil;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_SPI3_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_RNG_Init(void);
static void MX_RTC_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

void UART_Printf(const char* fmt, ...);
extern void print_network_information(void);
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//// Этот обратный вызов автоматически вызывается HAL при возникновении события UEV
//    if(htim->Instance == TIM1) //check if the interrupt comes from TIM1
//    {
//        ++capture;
//        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);

//        if (capture == 2)
//        {
//            ++count;
////            HAL_SPI_TransmitReceive(&hspi3, txBuf , rxBuf, MAX_PACKET_LEN, 0x1000);
////            memcpy(txBuf, rxBuf + 1, MAX_PACKET_LEN);
//        }
//        if (capture == 1)
//        {
//            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
//            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);

//        }
//    }

//}
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == SPI3)
    {
//        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
    }
}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
//    if(hspi->Instance == SPI3)
//    {
//        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);
//    }
}

//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
//    capture = 0;
////    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
////    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
//}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef enum {
    eepromCLEAR,
    eepromNotCLEAR
} isEEPROMClear;
isEEPROMClear isEEPROMclear();

void printFileFromEEPROM(const char* nameFile_onEEPROM);

void simpleTestI2C_EEPROM(uint16_t addr)
{
    uint16_t num = 256;
    printf("Simple test I2C_EEPROM ...\n");

    uint8_t rd_value[256] = {0};
    uint8_t wr_value[256] = {'1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
                             '1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
                             '1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
                             '1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
                             '1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
                             '1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
                             '1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
                             'Q','W','R','_','_','*',
                             '1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','\0'};

    uint8_t wr_value2[256] = {'A','B','3','D','E','F','J','K','L','M','N','O','P','Q','R','\0'};
    BSP_EEPROM_ReadBuffer(rd_value, addr, &num);
    printf("EEPROM read: %s\r\n",rd_value);
    printf("EEPROM write:");
    printf("%s\r\n",wr_value);
    BSP_EEPROM_WriteBuffer(wr_value, addr, num);
    delayUS_ASM(100000);
    BSP_EEPROM_ReadBuffer(rd_value, addr, &num);
    printf("EEPROM read: %s\r\n",rd_value);
    delayUS_ASM(100000);

    printf("EEPROM write:");
    printf("%s\r\n",wr_value2);
    BSP_EEPROM_WriteBuffer(wr_value2, addr, num);
    delayUS_ASM(100000);
    BSP_EEPROM_ReadBuffer(rd_value, addr, &num);
    printf("EEPROM read: %s\r\n",rd_value);
    delayUS_ASM(100000);
    printf("Simple test I2C_EEPROM ..OK\r\n\n");

    uint16_t len = 93;
    uint8_t rd_value2[93] = {0};
    BSP_EEPROM_ReadBuffer(rd_value2, ipSettingAdressInEEPROM, &len);
    printf("\r\nEEPROM settings read: %s\r\n", rd_value2);

}

void saveLenFileToEeprom(const char* nameFile_onSD, uint32_t numByteFile)
{
    printf("save len file %s to eeprom\n", nameFile_onSD);
    const char* index = "index.html";
    const char* main = "main.html";
    char tmp[6];
    sprintf(tmp,"%.5d", numByteFile);
    if (nameFile_onSD == index)
    {
        BSP_EEPROM_WriteBuffer((uint8_t*)tmp, indexLenAdressInEEPROM, 6);
    }
    if (nameFile_onSD == main)
    {
        BSP_EEPROM_WriteBuffer((uint8_t*)tmp, mainLenAdressInEEPROM, 6);
    }
}

void copyFileToAdressEEPROM(const char* nameFile_onSD, uint16_t WriteAddr)
{
    printf("\n\nCopy file %s to adress 0x%.4X i2c eeprom \n",nameFile_onSD, WriteAddr);
    FRESULT result = f_open(&fil, nameFile_onSD, FA_OPEN_ALWAYS | FA_READ );
    if (result == FR_OK)
    {
        uint32_t numByteFile = fil.obj.objsize;
        saveLenFileToEeprom(nameFile_onSD, numByteFile);
        printf("num byte: %d\n",numByteFile);
        UINT rc;
        pindex = malloc(numByteFile);
        FRESULT res = f_read(&fil, pindex, numByteFile, &rc);
        printf("file %s read: %d\n", nameFile_onSD, res);
    //    printf("copy %s to eeprpm (adress 0x%.4X)\n", nameFile_onSD, WriteAddr);
        int result = BSP_EEPROM_WriteBuffer((uint8_t*)pindex, WriteAddr, numByteFile);
        printf("file %s write to adress 0x%.4X on eprom: %d\n", nameFile_onSD, WriteAddr, result);
        f_close(&fil);
        free (pindex);
    }
    else
    {
        printf("\nError opening file %s!!! \n",nameFile_onSD);
    }
}

void copyFileToEEPROM(const char* nameFile_onSD)
{
#ifdef LFS
    uint32_t time = HAL_GetTick();
    f_open(&fil, nameFile_onSD, FA_OPEN_ALWAYS | FA_READ );
    uint32_t numByteFile = fil.obj.objsize;
    UINT rc;
    pindex = malloc(numByteFile);
    f_read(&fil, pindex, numByteFile, &rc);
    f_close(&fil);

    printf("copy %s ... ", nameFile_onSD);
    lfs_file_open(&lfs, &file, nameFile_onSD, LFS_O_WRONLY | LFS_O_CREAT );
    lfs_file_write(&lfs, &file, pindex, numByteFile);
    lfs_file_close(&lfs, &file);
    memset(pindex, 0x00, numByteFile);
    free (pindex);
    char tmp[30];
    sprintf(tmp,"%ds\n", (HAL_GetTick() - time)/1000 );
    Printf(tmp);
//    printFileFromEEPROM(nameFile_onSD);
#endif
}

void loadFilesFromEepromToMemory(uint16_t ReadAddrIndex, uint16_t numByteFileIndex,
                                 uint16_t ReadAddrMain, uint16_t numByteFileMain,
                                 uint16_t ReadAddrSettings, uint16_t numByteFileSettings )
{
    printf("load to memory index.html main.html ip_settings\n");
    uint16_t * numByteIndex = &numByteFileIndex;
    pindex = malloc(numByteFileIndex);
    int result = BSP_EEPROM_ReadBuffer((uint8_t*)pindex, ReadAddrIndex, numByteIndex);
//    printf("\nread %d byte from adress 0x%.4X on eprom: %d\n", numByteFileIndex, ReadAddrIndex, result);

    uint16_t * numByteMain = &numByteFileMain;
    pmain = malloc(numByteFileMain);
    result = BSP_EEPROM_ReadBuffer((uint8_t*)pmain, ReadAddrMain, numByteMain);
//    printf("read %d byte from adress 0x%.4X on eprom: %d\n", numByteFileMain, ReadAddrMain, result);

    //Тут гружу в память настройки
    uint16_t * numByteSettings = &numByteFileSettings;
    psettingsIP = malloc(numByteFileSettings);
    result = BSP_EEPROM_ReadBuffer((uint8_t*)psettingsIP, ReadAddrSettings, numByteSettings);
//    printf("read %d byte from adress 0x%.4X on eprom: %d\n", numByteFileSettings, ReadAddrSettings, result);
}

void printFilesFromMemory()
{
#ifndef   NEW_HTTP_SERVER
    Printf("\nprintFilesFromMemory\n\n");
        for (int i = 0; i < indexLen; ++i)
        {
            Printf("%c", pindex[i]);
        }
        Printf("\n\n\n");

        for (int i = 0; i < mainLen; ++i)
        {
            Printf("%c", pmain[i]);
        }
        Printf("\n");
#endif
}

void printFileFromAdressEEPROM(uint16_t ReadAddr, uint16_t numByteFile)
{
    printf("Print %d byte from 0x%.4X adress i2c eeprom\n",numByteFile, ReadAddr);
    uint16_t * numByte = &numByteFile;
    pindex = malloc(numByteFile);
    memset(pindex, 0x00, numByteFile);
    int result = BSP_EEPROM_ReadBuffer((uint8_t*)pindex, ReadAddr, numByte);
    printf("read %d byte from adress 0x%.4X on eprom: %d\n", numByteFile, ReadAddr, result);
    Printf("print reading file\n");
    for (int i = 0; i < numByteFile; ++i)
    {
        Printf("%c", pindex[i]);
    }
    Printf("\n");
    free (pindex);
}

void printFileFromEEPROM(const char* nameFile_onEEPROM)
{
#ifdef LFS
    lfs_file_open(&lfs, &file, nameFile_onEEPROM, LFS_O_RDONLY );
    uint32_t numByteFile = lfs_file_size(&lfs, &file);
    pindex = malloc(numByteFile);
    lfs_file_read(&lfs, &file, pindex, numByteFile);
    lfs_file_close(&lfs, &file);

    Printf("print %s\n", nameFile_onEEPROM);
    for (int i = 0; i < numByteFile; ++i)
    {
        Printf("%c", pindex[i]);
    }
    Printf("\n");
    free (pindex);
#endif
}

void testReadFile(const char* nameFile_onEEPROM)
{
#ifdef LFS
    UART_Printf("test read %s ... ", nameFile_onEEPROM); delayUS_ASM(1000);
    uint32_t time = HAL_GetTick();
    lfs_file_open(&lfs, &file, nameFile_onEEPROM, LFS_O_RDONLY );
    uint32_t numByteFile = lfs_file_size(&lfs, &file);
    pindex = malloc(numByteFile);
    lfs_file_read(&lfs, &file, pindex, numByteFile);
    lfs_file_close(&lfs, &file);
    free (pindex);
    char tmp[30];
    sprintf(tmp,"%ds\n", (HAL_GetTick() - time)/1000 );
    UART_Printf(tmp); delayUS_ASM(5000);
#endif
}

void markEEPROMasOld()
{
    char old[16] = {0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00};
    BSP_EEPROM_WriteBuffer((uint8_t*)old, markEEPROMclear, 16);
    printf("eeprom mark as OLD\n");
}

void markEEPROMasNew()
{
    char clear[16] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00};
    //Ручная Установка признака новая
    BSP_EEPROM_WriteBuffer((uint8_t*)clear, markEEPROMclear, 16);
    printf("eeprom mark as NEW\n");
}

isEEPROMClear isEEPROMclear()
{
    uint16_t numByte = 16;
    uint16_t * pnumByte = &numByte;
    char tmp[16];
    char clear[16] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00};
    BSP_EEPROM_ReadBuffer((uint8_t *)tmp, markEEPROMclear, pnumByte);
    if(strncmp(tmp, clear, 15) == 0)
        return eepromCLEAR;
    else
        return eepromNotCLEAR;
}

void copyMacToAdressEEPROM(uint16_t Addr)
{
#ifndef  MAC_IN_DECIMAL
    char tmp[18];
    FRESULT result = f_open(&fil, "mac16", FA_OPEN_ALWAYS | FA_READ );
    if (result == FR_OK)
    {
        printf("\nCopy MAC adress from SD in Hex to adress 0x%.4X eeprom\n",Addr);
        UINT rc;
        f_read(&fil, tmp, 18, &rc);
        f_close(&fil);
        tmp[17]=0x00;
        printf("MAC:\n%s\n",tmp);
        int open = BSP_EEPROM_WriteBuffer((uint8_t *)tmp, Addr, 18);
        printf("MAC write from SD in Hex to adress 0x%.4X on eprom: %d\n", Addr, open);
    }
#endif
#ifdef  MAC_IN_DECIMAL
    char tmp[24];
    FRESULT result = f_open(&fil, "mac10", FA_OPEN_ALWAYS | FA_READ );
    if (result == FR_OK)
    {
        printf("\nCopy MAC adress from SD in Decimal to adress 0x%.4X eeprom\n",Addr);
        UINT rc;
        f_read(&fil, tmp, 24, &rc);
        f_close(&fil);
        tmp[23]=0x00;
        printf("MAC:\n%s\n",tmp);
        int open = BSP_EEPROM_WriteBuffer((uint8_t *)tmp, Addr, 24);
        printf("MAC write from SD in Decimal to adress 0x%.4X on eprom: %d\n", Addr, open);
    }
#endif
}

void copyParametersFromSDToAdressEEPROM(uint16_t Addr)
{
    printf("\nCopy IP settings from SD to adress 0x%.4X eeprom\n",Addr);
    char tmp[settingsLen];
    f_open(&fil, "host_IP", FA_OPEN_ALWAYS | FA_READ );
    UINT rc;
    f_read(&fil, tmp, 15, &rc);
    f_close(&fil);
    f_open(&fil, "dest_IP", FA_OPEN_ALWAYS | FA_READ );
    f_read(&fil, tmp+15, 15, &rc);
    f_close(&fil);
    f_open(&fil, "gate_IP", FA_OPEN_ALWAYS | FA_READ );
    f_read(&fil, tmp+30, 15, &rc);
    f_close(&fil);
    f_open(&fil, "mask_IP", FA_OPEN_ALWAYS | FA_READ );
    f_read(&fil, tmp+45, 15, &rc);
    f_close(&fil);
    f_open(&fil, "md5", FA_OPEN_ALWAYS | FA_READ );
    f_gets(tmp+60, 33, &fil);
    f_close(&fil);
    printf("settings:\n%s\n",tmp);
    BSP_EEPROM_WriteBuffer((uint8_t *)tmp, Addr, settingsLen);
    delayUS_ASM(100000);
//    Printf("Settings IP write to adress 0x%.4X on eprom: %d", Addr, result);

    uint16_t len = 93;
    uint8_t rd_value[93] = {0};
    BSP_EEPROM_ReadBuffer(rd_value, ipSettingAdressInEEPROM, &len);
    printf("\r\nEEPROM read settings: %s\r\n",rd_value);

}

void copyParametersFromSDToAdressSPIEEPROM()
{

}

void copyMacToAdressSPIEEPROM()
{

}

void copyDefaultParametersToAdressEEPROM(uint16_t Addr)
{
    printf("\nSet default IP settings to adress 0x%.4X eeprom\n",Addr);
    char defaultIP[settingsLen] =
    {'1','9','2','.','1','6','8','.','0','0','1','.','2','2','2',
     '1','9','2','.','1','6','8','.','0','0','1','.','2','0','0',
     '1','9','2','.','1','6','8','.','0','0','1','.','0','0','1',
     '2','5','5','.','2','5','5','.','2','5','5','.','0','0','0',
     'd','4','1','d','8','c','d','9','8','f','0','0','b','2','0','4','e','9','8','0','0','9','9','8','e','c','f','8','4','2','7','e'};
    printf("default settings:\n%s\n",defaultIP);
    int result = BSP_EEPROM_WriteBuffer((uint8_t *)defaultIP, Addr, settingsLen);
    Printf("Settings IP write to adress 0x%.4X on eprom: %d", Addr, result);
}

void copyDefaultMACToAdressEEPROM(uint16_t Addr)
{
    printf("Set default MAC adress to adress eeprom 0x%.4X \n", Addr);
    char defaultMAC[24] =
    {'0','0','0',':','0','2','1',':','0','6','6',':','1','9','1',':','2','4','0',':','0','8','2','\0'};
    int result = BSP_EEPROM_WriteBuffer((uint8_t *)defaultMAC, Addr, 24);
    printf("Set default MAC adress to adress eeprom 0x%.4X: %d\n", Addr, result);
}

void SetMacFromAdressEEPROM(uint16_t Addr)
{
#ifndef MAC_IN_DECIMAL
    printf("Set MAC adress in HEX from adress eeprom 0x%.4X \n", Addr);
    uint16_t numByte = 18;
    uint16_t * pnumByte = &numByte;
    char tmp[18];
    char tmp2[2];
    int result = BSP_EEPROM_ReadBuffer((uint8_t *)tmp, Addr, pnumByte);
    printf("MAC read from adress 0x%.4X on eprom: %d\n", Addr, result);
    printf("MAC:\n%s\n",tmp);
    strncpy(mac,tmp,18);
    strncpy(tmp2, tmp, 2);
    macaddr[0] = convertHexToDecimal(tmp2);
    strncpy(tmp2,tmp+3, 2);
    macaddr[1] = convertHexToDecimal(tmp2);
    strncpy(tmp2,tmp+6, 2);
    macaddr[2] = convertHexToDecimal(tmp2);
    strncpy(tmp2,tmp+9, 2);
    macaddr[3] = convertHexToDecimal(tmp2);
    strncpy(tmp2,tmp+12, 2);
    macaddr[4] = convertHexToDecimal(tmp2);
    strncpy(tmp2,tmp+15, 2);
    macaddr[5] = convertHexToDecimal(tmp2);
    printf("mac: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
#endif
#ifdef MAC_IN_DECIMAL
    printf("Set MAC adress in decimal from adress eeprom 0x%.4X \n", Addr);
    uint16_t numByte = 24;
    uint16_t * pnumByte = &numByte;
    char tmp[24];
    char tmp2[3];
    int result = BSP_EEPROM_ReadBuffer((uint8_t *)tmp, Addr, pnumByte);
    printf("MAC read from adress 0x%.4X on eprom: %d\n", Addr, result);
//    printf("MAC:\n%s\n",tmp);
    strncpy(tmp2, tmp, 4);
    macaddr[0] = atoi(tmp2);
    strncpy(tmp2,tmp+4, 4);
    macaddr[1] = atoi(tmp2);
    strncpy(tmp2,tmp+8, 4);
    macaddr[2] = atoi(tmp2);
    strncpy(tmp2,tmp+12, 4);
    macaddr[3] = atoi(tmp2);
    strncpy(tmp2,tmp+16, 4);
    macaddr[4] = atoi(tmp2);
    strncpy(tmp2,tmp+20, 4);
    macaddr[5] = atoi(tmp2);
    printf("mac: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
#endif
}

void SetParaametersFromAdressEEPROM(uint16_t Addr)
{
    printf("Set IP paraameters from adress eeprom 0x%.4X \n", Addr);
    uint16_t numByte = settingsLen;
    uint16_t * pnumByte = &numByte;
    char tmp[settingsLen];
    char tmp2[3];

    uint16_t len = 93;
    uint8_t rd_value[93] = {0};
    BSP_EEPROM_ReadBuffer(rd_value, Addr, &len);
    printf("\r\nEEPROM read: %s\r\n",rd_value);


    int result = BSP_EEPROM_ReadBuffer((uint8_t *)tmp, Addr, pnumByte);
    printf("Settings IP read from adress 0x%.4X on eprom: %d\n", Addr, result);
    printf("settings:\n%s\n",tmp);

    strncpy(host_IP,tmp,15);
    strncpy(dest_IP,tmp+15,15);
    strncpy(gate_IP,tmp+30,15);
    strncpy(mask_IP,tmp+45,15);

    strncpy(tmp2,tmp,3);
    ipaddr[0] = atoi(tmp2);
    strncpy(tmp2,tmp+4,3);
    ipaddr[1] = atoi(tmp2);
    strncpy(tmp2,tmp+8,3);
    ipaddr[2] = atoi(tmp2);
    strncpy(tmp2,tmp+12,3);
    ipaddr[3] = atoi(tmp2);

    strncpy(tmp2,tmp+15,3);
    destip[0] = atoi(tmp2);
    strncpy(tmp2,tmp+19,3);
    destip[1] = atoi(tmp2);
    strncpy(tmp2,tmp+23,3);
    destip[2] = atoi(tmp2);
    strncpy(tmp2,tmp+27,3);
    destip[3] = atoi(tmp2);

    strncpy(tmp2,tmp+30,3);
    ipgate[0] = atoi(tmp2);
    strncpy(tmp2,tmp+34,3);
    ipgate[1] = atoi(tmp2);
    strncpy(tmp2,tmp+38,3);
    ipgate[2] = atoi(tmp2);
    strncpy(tmp2,tmp+42,3);
    ipgate[3] = atoi(tmp2);

    strncpy(tmp2,tmp+45,3);
    ipmask[0] = atoi(tmp2);
    strncpy(tmp2,tmp+49,3);
    ipmask[1] = atoi(tmp2);
    strncpy(tmp2,tmp+53,3);
    ipmask[2] = atoi(tmp2);
    strncpy(tmp2,tmp+57,3);
    ipmask[3] = atoi(tmp2);

    strncpy(MD5, tmp+60, 32);

    printf("host_IP: %d.%d.%d.%d\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
    printf("dest_IP: %d.%d.%d.%d\n",destip[0],destip[1],destip[2],destip[3]);
    printf("gate_IP: %d.%d.%d.%d\n",ipgate[0],ipgate[1],ipgate[2],ipgate[3]);
    printf("mask_IP: %d.%d.%d.%d\n",ipmask[0],ipmask[1],ipmask[2],ipmask[3]);
    printf("md5: %s\n", MD5);
}

void copyParametersToEEPROM()
{
#ifdef LFS
    Printf("\nCopy IP settings from SD to eeprom\n");
    char tmp[100];
    char tmp1[5];
    char tmp2[5];
    char tmp3[5];
    char tmp4[5];
    f_open(&fil, "host_IP", FA_OPEN_ALWAYS | FA_READ );
    f_gets(tmp1, 5, &fil);
    f_gets(tmp2, 5, &fil);
    f_gets(tmp3, 5, &fil);
    f_gets(tmp4, 5, &fil);
    f_close(&fil);
    sprintf(tmp,"%.3s\r\n%.3s\r\n%.3s\r\n%.3s\r\n",tmp1,tmp2,tmp3,tmp4);
    Printf("host_IP:\n%s\n", tmp);
    lfs_file_open(&lfs, &file, "host_IP", LFS_O_WRONLY | LFS_O_CREAT);
    lfs_file_write(&lfs, &file, &tmp, sizeof(tmp));
    lfs_file_close(&lfs, &file);

    f_open(&fil, "dest_IP", FA_OPEN_ALWAYS | FA_READ );
    f_gets(tmp1, 100, &fil);
    f_gets(tmp2, 100, &fil);
    f_gets(tmp3, 100, &fil);
    f_gets(tmp4, 100, &fil);
    f_close(&fil);
    sprintf(tmp,"%.3s\r\n%.3s\r\n%.3s\r\n%.3s\r\n",tmp1,tmp2,tmp3,tmp4);
    Printf("dest_IP:\n%s\n", tmp);
    lfs_file_open(&lfs, &file, "dest_IP", LFS_O_WRONLY | LFS_O_CREAT);
    lfs_file_write(&lfs, &file, &tmp, sizeof(tmp));
    lfs_file_close(&lfs, &file);

    f_open(&fil, "gate_IP", FA_OPEN_ALWAYS | FA_READ );
    f_gets(tmp1, 100, &fil);
    f_gets(tmp2, 100, &fil);
    f_gets(tmp3, 100, &fil);
    f_gets(tmp4, 100, &fil);
    f_close(&fil);
    sprintf(tmp,"%.3s\r\n%.3s\r\n%.3s\r\n%.3s\r\n",tmp1,tmp2,tmp3,tmp4);
    Printf("gate_IP:\n%s\n", tmp);
    lfs_file_open(&lfs, &file, "gate_IP", LFS_O_WRONLY | LFS_O_CREAT);
    lfs_file_write(&lfs, &file, &tmp, sizeof(tmp));
    lfs_file_close(&lfs, &file);

    f_open(&fil, "mask_IP", FA_OPEN_ALWAYS | FA_READ );
    f_gets(tmp1, 100, &fil);
    f_gets(tmp2, 100, &fil);
    f_gets(tmp3, 100, &fil);
    f_gets(tmp4, 100, &fil);
    f_close(&fil);
    sprintf(tmp,"%.3s\r\n%.3s\r\n%.3s\r\n%.3s\r\n",tmp1,tmp2,tmp3,tmp4);
    Printf("mask_IP:\n%s\n", tmp);
    lfs_file_open(&lfs, &file, "mask_IP", LFS_O_WRONLY | LFS_O_CREAT);
    lfs_file_write(&lfs, &file, &tmp, sizeof(tmp));
    lfs_file_close(&lfs, &file);

    f_open(&fil, "md5", FA_OPEN_ALWAYS | FA_READ );
    f_gets(tmp, 100, &fil);
    strncpy(MD5, tmp, 32);
    f_close(&fil);
    lfs_file_open(&lfs, &file, "md5", LFS_O_WRONLY | LFS_O_CREAT);
    lfs_file_write(&lfs, &file, &tmp, sizeof(tmp));
    lfs_file_close(&lfs, &file);
#endif
}

void setMacFromSD()
{
#ifndef  MAC_IN_DECIMAL
    char tmp[18];
    char tmp2[2];
    f_open(&fil, "mac16", FA_OPEN_ALWAYS | FA_READ );
    f_lseek(&fil, 0);
    f_gets(tmp, 18, &fil);
    strncpy(mac,tmp,18);
    strncpy(tmp2,tmp,2);
    macaddr[0] = convertHexToDecimal(tmp2);
    strncpy(tmp2,tmp+3,2);
    macaddr[1] = convertHexToDecimal(tmp2);
    strncpy(tmp2,tmp+6,2);
    macaddr[2] = convertHexToDecimal(tmp2);
    strncpy(tmp2,tmp+9,2);
    macaddr[3] = convertHexToDecimal(tmp2);
    strncpy(tmp2,tmp+12,2);
    macaddr[4] = convertHexToDecimal(tmp2);
    strncpy(tmp2,tmp+15,2);
    macaddr[5] = convertHexToDecimal(tmp2);
    printf("mac: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
    f_close(&fil);
#endif
#ifdef  MAC_IN_DECIMAL
    char tmp[33];
    char tmp2[3];
    f_open(&fil, "mac10", FA_OPEN_ALWAYS | FA_READ );
    f_lseek(&fil, 0);
    f_gets(tmp, 24, &fil);
    strncpy(tmp2,tmp,3);
    macaddr[0] = atoi(tmp2);
    strncpy(tmp2,tmp+4,3);
    macaddr[1] = atoi(tmp2);
    strncpy(tmp2,tmp+8,3);
    macaddr[2] = atoi(tmp2);
    strncpy(tmp2,tmp+12,3);
    macaddr[3] = atoi(tmp2);
    strncpy(tmp2,tmp+16,3);
    macaddr[4] = atoi(tmp2);
    strncpy(tmp2,tmp+20,3);
    macaddr[5] = atoi(tmp2);
    printf("mac: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
    f_close(&fil);
#endif
}

void setParametersFromSD()
{
    Printf("\nSet IP Parameters from SD\n");
    char tmp[33];
    char tmp2[3];
    f_open(&fil, "host_IP", FA_OPEN_ALWAYS | FA_READ );
    f_gets(tmp, 16, &fil);
    strcpy(host_IP, tmp);
    strncpy(tmp2,tmp,3);
    ipaddr[0] = atoi(tmp2);
    strncpy(tmp2,tmp+4,3);
    ipaddr[1] = atoi(tmp2);
    strncpy(tmp2,tmp+8,3);
    ipaddr[2] = atoi(tmp2);
    strncpy(tmp2,tmp+12,3);
    ipaddr[3] = atoi(tmp2);
    printf("host_IP: %d.%d.%d.%d\r\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
    f_close(&fil);

    f_open(&fil, "dest_IP", FA_OPEN_ALWAYS | FA_READ );
    f_gets(tmp, 16, &fil);
    strcpy(dest_IP, tmp);
    strncpy(tmp2,tmp,3);
    destip[0] = atoi(tmp2);
    strncpy(tmp2,tmp+4,3);
    destip[1] = atoi(tmp2);
    strncpy(tmp2,tmp+8,3);
    destip[2] = atoi(tmp2);
    strncpy(tmp2,tmp+12,3);
    destip[3] = atoi(tmp2);
    printf("dest_IP: %d.%d.%d.%d\r\n",destip[0],destip[1],destip[2],destip[3]);
    f_close(&fil);

    f_open(&fil, "gate_IP", FA_OPEN_ALWAYS | FA_READ );
    f_gets(tmp, 16, &fil);
    strcpy(gate_IP, tmp);
    strncpy(tmp2,tmp,3);
    ipgate[0] = atoi(tmp2);
    strncpy(tmp2,tmp+4,3);
    ipgate[1] = atoi(tmp2);
    strncpy(tmp2,tmp+8,3);
    ipgate[2] = atoi(tmp2);
    strncpy(tmp2,tmp+12,3);
    ipgate[3] = atoi(tmp2);
    printf("gate_IP: %d.%d.%d.%d\r\n",ipgate[0],ipgate[1],ipgate[2],ipgate[3]);
    f_close(&fil);

    f_open(&fil, "mask_IP", FA_OPEN_ALWAYS | FA_READ );
    f_gets(tmp, 16, &fil);
    strcpy(mask_IP, tmp);
    strncpy(tmp2,tmp,3);
    ipmask[0] = atoi(tmp2);
    strncpy(tmp2,tmp+4,3);
    ipmask[1] = atoi(tmp2);
    strncpy(tmp2,tmp+8,3);
    ipmask[2] = atoi(tmp2);
    strncpy(tmp2,tmp+12,3);
    ipmask[3] = atoi(tmp2);
    printf("mask_IP: %d.%d.%d.%d\r\n",ipmask[0],ipmask[1],ipmask[2],ipmask[3]);
    f_close(&fil);

    f_open(&fil, "md5", FA_OPEN_ALWAYS | FA_READ );
    f_gets(tmp, 33, &fil);
    strncpy(MD5, tmp, 32);
    f_close(&fil);
    printf("md5: %s\n",MD5);
}

void SetParaametersFromEEPROM()
{
#ifdef LFS
    char tmp[100];
    char tmp5[23];
    char tmp6[3];
    UART_Printf("Load data from EEPROM\r\n"); delayUS_ASM(10000);
    lfs_file_open(&lfs, &file, "host_IP", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &tmp5, sizeof (tmp5));
    lfs_file_close(&lfs, &file);
    strncpy(tmp6,tmp5,3);
    ipaddr[0] = atoi(tmp6);
    strncpy(tmp6,tmp5+5,3);
    ipaddr[1] = atoi(tmp6);
    strncpy(tmp6,tmp5+10,3);
    ipaddr[2] = atoi(tmp6);
    strncpy(tmp6,tmp5+15,3);
    ipaddr[3] = atoi(tmp6);

    lfs_file_open(&lfs, &file, "dest_IP", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &tmp5, sizeof (tmp5));
    lfs_file_close(&lfs, &file);
    strncpy(tmp6,tmp5,3);
    destip[0] = atoi(tmp6);
    strncpy(tmp6,tmp5+5,3);
    destip[1] = atoi(tmp6);
    strncpy(tmp6,tmp5+10,3);
    destip[2] = atoi(tmp6);
    strncpy(tmp6,tmp5+15,3);
    destip[3] = atoi(tmp6);

    lfs_file_open(&lfs, &file, "gate_IP", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &tmp5, sizeof (tmp5));
    lfs_file_close(&lfs, &file);
    strncpy(tmp6,tmp5,3);
    ipgate[0] = atoi(tmp6);
    strncpy(tmp6,tmp5+5,3);
    ipgate[1] = atoi(tmp6);
    strncpy(tmp6,tmp5+10,3);
    ipgate[2] = atoi(tmp6);
    strncpy(tmp6,tmp5+15,3);
    ipgate[3] = atoi(tmp6);

    lfs_file_open(&lfs, &file, "mask_IP", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &tmp5, sizeof (tmp5));
    lfs_file_close(&lfs, &file);
    strncpy(tmp6,tmp5,3);
    ipmask[0] = atoi(tmp6);
    strncpy(tmp6,tmp5+5,3);
    ipmask[1] = atoi(tmp6);
    strncpy(tmp6,tmp5+10,3);
    ipmask[2] = atoi(tmp6);
    strncpy(tmp6,tmp5+15,3);
    ipmask[3] = atoi(tmp6);

    lfs_file_open(&lfs, &file, "md5", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &tmp, sizeof (tmp));
    lfs_file_close(&lfs, &file);
    strcpy(MD5, tmp);

    sprintf(tmp,"host_IP: %d.%d.%d.%d\r\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
    Printf(tmp);
    sprintf(tmp,"dest_IP: %d.%d.%d.%d\r\n",destip[0],destip[1],destip[2],destip[3]);
    Printf(tmp);
    sprintf(tmp,"gate_IP: %d.%d.%d.%d\r\n",ipgate[0],ipgate[1],ipgate[2],ipgate[3]);
    Printf(tmp);
    sprintf(tmp,"mask_IP: %d.%d.%d.%d\r\n",ipmask[0],ipmask[1],ipmask[2],ipmask[3]);
    Printf(tmp);
    sprintf(tmp,"md5: %s", MD5);
    Printf(tmp);

    sprintf(tmp,"mac: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
    Printf(tmp);
#endif
}

void wep_define_func(void)
{
#ifdef   NEW_HTTP_SERVER
    // Index page and netinfo / base64 image demo
#ifdef   PROD
    reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t *)index_page);				// index.html 		: Main page example
    reg_httpServer_webContent((uint8_t *)"main.html", (uint8_t *)main_page);                // main.html
    reg_httpServer_webContent((uint8_t *)"host_IP", (uint8_t *)host_IP);
    reg_httpServer_webContent((uint8_t *)"dest_IP", (uint8_t *)dest_IP);
    reg_httpServer_webContent((uint8_t *)"gate_IP", (uint8_t *)gate_IP);
    reg_httpServer_webContent((uint8_t *)"mask_IP", (uint8_t *)mask_IP);
    reg_httpServer_webContent((uint8_t *)"mac_adr", (uint8_t *)mac);
#endif
#ifndef   PROD
    reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t *)index_page_WIZNET);       //web сервер WIZNET
    reg_httpServer_webContent((uint8_t *)"netinfo.html", (uint8_t *)netinfo_page);			// netinfo.html 	: Network information example page
    reg_httpServer_webContent((uint8_t *)"img.html", (uint8_t *)img_page);					// img.html 		: Base64 Image data example page
    reg_httpServer_webContent((uint8_t *)"dio.html", (uint8_t *)dio_page);					// dio.html 		: Digital I/O control example page
// js файлы внесены в код html
//    reg_httpServer_webContent((uint8_t *)"netinfo.js", (uint8_t *)w5x00web_netinfo_js);     // netinfo.js 		: JavaScript for Read Network configuration 	(+ ajax.js)
//    reg_httpServer_webContent((uint8_t *)"dio.js", (uint8_t *)w5x00web_dio_js);             // dio.js 			: JavaScript for digital I/O control 	(+ ajax.js)
//    reg_httpServer_webContent((uint8_t *)"ajax.js", (uint8_t *)w5x00web_ajax_js);			// ajax.js			: JavaScript for AJAX request transfer
#endif
#endif //NEW_HTTP_SERVER
}

void net_ini_WIZNET(uint8_t socketTCP)
{
    printf("net_ini_WIZNET_WEB\n");

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_Delay(70);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    HAL_Delay(70);
    uint8_t sn_TCP = socketTCP;
    WIZCHIPInitialize();

    printf("WIZCHIPInitialize  OK\n");

    for (int i =0; i < 6; ++i)
    {
        defaultNetInfo.mac[i] = macaddr[i];
    }
    for (int i =0; i < 4; ++i)
    {
        defaultNetInfo.ip[i] = ipaddr[i];
        defaultNetInfo.gw[i] = ipgate[i];
        defaultNetInfo.sn[i] = ipmask[i];
    }

    ctlnetwork(CN_SET_NETINFO, (void*) &defaultNetInfo);
    print_network_information();
    socket(sn_TCP, Sn_MR_TCP, local_port_web, 0/*SF_UNI_BLOCK*/); //У W5500 4 флага
    if (SOCK_OK == listen(sn_TCP))
        printf("socket %d listening\n", sn_TCP);
}

void workI2C_EEPROM()
{
//      simpleTestI2C_EEPROM(simpleTestEEPROMadress);

//Дальше работаю без  littleFsInit  глюки при записи больших файлов !!!
//      littleFsInit();
//      Printf("FsEeprom TEST ... ");
//      FsForEeprom_test();

    f_mount(&fs, "", 0);
    FRESULT result = f_open(&fil, "host_IP", FA_OPEN_ALWAYS | FA_READ );
    if (result == 0)
    {
        Printf("SD active\r\n");
    }
    else
    {
        sdCartOn = 0;
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
//        markEEPROMasNew(); //Ручная установка EEPROM как NEW
        if (eepromCLEAR == isEEPROMclear()) //Проверяем EEPROM новая ли
        {
            printf("EEPROM: NEW\n");
            // Пишем на eeprom все параметры по умолчанию
            copyDefaultParametersToAdressEEPROM(ipSettingAdressInEEPROM);
            copyDefaultMACToAdressEEPROM(macAdressInEEPROM);
            SetMacFromAdressEEPROM(macAdressInEEPROM);
            markEEPROMasOld(); // Снимаем признак новая EEPROM
        }
        else
        {
            printf("EEPROM: OLD\r\n");
        }

    }

    if (result != 0)
        printf("not found SD\r\n");
    f_close(&fil);
    if (sdCartOn == 1)
    {
        setParametersFromSD();
        copyParametersFromSDToAdressEEPROM(ipSettingAdressInEEPROM);
        copyMacToAdressEEPROM(macAdressInEEPROM);
        setMacFromSD();
#ifndef   NEW_HTTP_SERVER
        copyFileToAdressEEPROM("index.html", indexAdressInEEPROM);
        copyFileToAdressEEPROM("main.html", mainAdressInEEPROM);
#endif
#ifdef LFS
//lfs не использую
//        copyParametersToEEPROM();
//        copyFileToEEPROM("index.html");
//        copyFileToEEPROM("main.html");
#endif
    } else //Работа с i2c eeprom
    {
        SetParaametersFromAdressEEPROM(ipSettingAdressInEEPROM);
        SetMacFromAdressEEPROM(macAdressInEEPROM);
#ifndef   NEW_HTTP_SERVER
        //Загружаем длины файлов
        char tmp[5];
        uint16_t numByteLen = 5;
        uint16_t * len = &numByteLen;
        BSP_EEPROM_ReadBuffer((uint8_t*)tmp, indexLenAdressInEEPROM, len);
        indexLen = atoi(tmp);
        printf("index.html len - %d\n", indexLen);
        BSP_EEPROM_ReadBuffer((uint8_t*)tmp, mainLenAdressInEEPROM, len);
        mainLen = atoi(tmp);
        printf("main.html len - %d\n", mainLen);

        loadFilesFromEepromToMemory(indexAdressInEEPROM, indexLen,
                                    mainAdressInEEPROM, mainLen,
                                    ipSettingAdressInEEPROM, settingsLen);
//        printFilesFromMemory();
//        printFileFromAdressEEPROM(indexAdressInEEPROM, indexLen); //index.html
//        printFileFromAdressEEPROM(mainAdressInEEPROM, mainLen); //main.html
#endif
#ifdef LFS
//lfs не использую
//        SetParaametersFromEEPROM();
//        testReadFile("index.html");
//        testReadFile("main.html");
//        printFileFromEEPROM("index.html");
//        printFileFromEEPROM("main.html");
#endif
    }
}

void workSPI_EEPROM()
{
        testSPI_EEPROM();//Test с SPI EEPROM
    //    copyDataFromI2cEepromToSpiEeprom();//Копируем данные из I2C eeprom в SPI eeprom (Settings и Mac)
    if (sdCartOn == 1)
    {
        copyParametersFromSDToAdressSPIEEPROM();//Копируем Settings из SD в SPI eeprom
        copyMacToAdressSPIEEPROM();             //Копируем MAC из SD в SPI eeprom
    }
}
#if 0
GPIO для дебага
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET); //81 pin
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET); //82 pin
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET); //84 pin  1 pin Debug
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET); //85 pin  2 pin Debug
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET); //86 pin
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET); //87 pin
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET); //88 pin
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET); //56 pin
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET); //57 pin
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET); //60 pin
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET); //Синий
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET); //Зеленый
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); //Красный
#endif

void prepearUDP_PLIS(uint8_t udpSocket)
{
    printf("prepearUDP_PLIS\r\n");
    socket(udpSocket, Sn_MR_UDP, local_port_udp , 0x00);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET); //CLK_EN (ПЛИС)
}

void sendReceiveUDP(uint8_t udpSocket)
{
    if (ABONENT_or_BASE == 0) {   //Cтанционный мост
        while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET) {}; // CPU_INT Жду пока плис поднимет флаг
//Очищаю сдвиговый регистр передачи MOSI
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET); HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
//Обмен с ПЛИС
        HAL_SPI_TransmitReceive(&hspi2, txCyclon , rxCyclon, MAX_PACKET_LEN, 0x1000);
//Очищаю сдвиговый регистр приема MISO
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

        sendPackets(udpSocket, destip, local_port_udp);
        receivePackets(udpSocket, destip, local_port_udp);
    }

    if (ABONENT_or_BASE == 1) {  //Абонентский мост
        while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET) {}; // CPU_INT Жду пока плис поднимет флаг
//Очищаю сдвиговый регистр передачи MOSI
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET); HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
//Обмен с ПЛИС
        HAL_SPI_TransmitReceive(&hspi2, txCyclon , rxCyclon, MAX_PACKET_LEN, 0x1000);
//Очищаю сдвиговый регистр приема MISO
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

        sendPackets(udpSocket, destip, local_port_udp);
        receivePackets(udpSocket, destip, local_port_udp);

    }
}

void sendHANDSHAKE(uint8_t udpSocket) {
    uint32_t currTime = HAL_GetTick();
    uint32_t delay = 500;
    sendto(udpSocket, (uint8_t *)test1, MAX_PACKET_LEN, destip, local_port_udp);
    if (HAL_GetTick() < currTime + delay){ //Пакет отправился быстро - destip в сети
        printf("HANDSHAKE = 1\r\n");
        HAL_GPIO_WritePin(GPIOD, Red_Led_Pin, GPIO_PIN_RESET);
        HANDSHAKE = 1;
    }
    else {
        printf("HANDSHAKE = 0\r\n");
        HAL_GPIO_WritePin(GPIOD, Green_Led_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOD, Blue_Led_Pin, GPIO_PIN_RESET);
        HAL_Delay(200);
        HAL_GPIO_WritePin(GPIOD, Red_Led_Pin, GPIO_PIN_RESET);
        HAL_Delay(200);
        HAL_GPIO_WritePin(GPIOD, Red_Led_Pin, GPIO_PIN_SET);
    }
}


void testSpiEepromWriteRead()
{
    //25AA1024  page - 256
    printf("\ntestSpiEepromClearWriteRead\r\n");
    uint8_t RxBuffer[256] = {0x00};
    uint8_t TxBuffer[256] = {0x00};
    uint8_t err = 0;
    for (uint32_t adr = 0; adr< 0xFFFFF; adr += 4096)
    {
        sprintf(TxBuffer,"%d", HAL_GetTick());
        EEPROM_SPI_WritePage(TxBuffer, adr, (uint16_t)256);
        EEPROM_SPI_ReadBuffer(RxBuffer, adr, (uint16_t)256);
        if (strcmp(TxBuffer, RxBuffer) == 0)
            printf("test SPI for address %5X OK data: %s\n", adr, TxBuffer);
        else
        {
            printf("test SPI for address %5X ERROR data: %s\n", adr, TxBuffer);
            err = 1;
        }
        if (err == 1)
            break;

    }
    if (err == 0)
        printf("Test SPI OK");
}

void testSpiEepromWritePage(uint32_t adr)
{
    uint8_t RxBuffer[256] = {0x00};
    HAL_Delay(1000);
    Printf("\nTest EEPROM_SPI_WritePage\n");
    EEPROM_SPI_ReadBuffer(RxBuffer, adr, (uint16_t)256);
    Printf("eeprom before write: %s\n", RxBuffer);
    Printf("Data to write: %s\n", TxBuffer);
    EEPROM_SPI_WritePage(TxBuffer, adr, (uint16_t)256);
    HAL_Delay(2000);
//    EEPROM_PowerDown();
//    HAL_Delay(1000);
//    EEPROM_WakeUP();
//    HAL_Delay(1000);
    EEPROM_SPI_ReadBuffer(RxBuffer, adr, (uint16_t)256);
    Printf("eeprom after write: %s\n", RxBuffer);

}

void testSpiEepromReadPage(uint32_t adr)
{
    uint8_t RxBuffer[256] = {0x00};
    HAL_Delay(1000);
    Printf("\nTest EEPROM_SPI_ReadPage\n");
    EEPROM_SPI_ReadBuffer(RxBuffer, adr, (uint16_t)256);
//    HAL_Delay(2000);
    Printf("RX Buffer after read: %s\n", RxBuffer);
}

void testSpiEepromWriteByte(uint32_t adr)
{
    uint8_t RxBuffer[256] = {0x00};
    HAL_Delay(1000);
    Printf("RX Buffer before write byte: %s\n", RxBuffer);
    EEPROM_SPI_ReadBuffer(RxBuffer, adr, (uint16_t)128);
    uint8_t byte[1] = "I";
    EEPROM_SPI_WriteByte(byte, adr);
    HAL_Delay(100);
    EEPROM_SPI_ReadBuffer(RxBuffer, adr, (uint16_t)128);
    Printf("RX Buffer awter write byte: %s\n", RxBuffer);
}

void testSPI_EEPROM()
{
    printf("\n-- Tests_SPI_EEPROM --\n");
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    EEPROM_SPI_INIT(&hspi3);
    printEepromSpiStatus();
//    EEPROM_CHIP_ERASE();
//    EEPROM_PAGE_ERASE(0x00000100); //PAGE_ERASE не работает
    testSpiEepromWriteRead();
}

void copyDataFromI2cEepromToSpiEeprom()
{
#if 0
    0000 - 00FF   256b   IP settings
    0100 - 010F   16b    длина index.html
    0110 - 011F   16b    длина main.html
    0120 - 019F   128b   mac
#endif
    printf("copyDataFromI2cEepromToSpiEeprom\n");
    uint16_t numByteSettings = settingsLen;
    uint16_t * pnumByteSettings = &numByteSettings;
    uint16_t numByteMac = 18;
    uint16_t * pnumByteMac = &numByteMac;
    uint8_t tmpSettings[settingsLen];
    uint8_t tmpMac[settingsLen];
    BSP_EEPROM_ReadBuffer((uint8_t *)tmpSettings, 0x0000, pnumByteSettings);
    BSP_EEPROM_ReadBuffer((uint8_t *)tmpMac, 0x0120, pnumByteMac);

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    EEPROM_SPI_INIT(&hspi3);

    EEPROM_SPI_WriteBuffer(tmpSettings, 0x0000, settingsLen);
    EEPROM_SPI_WriteBuffer(tmpMac, 0x0120, 18);
    uint8_t SettBuff[settingsLen];
    uint8_t MacBuff[18];
    EEPROM_SPI_ReadBuffer(SettBuff, 0x0000, settingsLen);
    EEPROM_SPI_ReadBuffer(MacBuff, 0x0120, 18);
    printf("Settings: %s\n", SettBuff);
    printf("Mac: %s\n", MacBuff);
}

void reboot()
{
    printf("*****  REBOOT  *****\r\n");
#ifdef   NEW_HTTP_SERVER
    http_disconnect(0);
#endif
    HAL_NVIC_SystemReset();
}

void checkLogin(char* buf)
{
    char login1[10] = {'a','d','m','i','n','\0'};
    char login2[10] = {'u','s','e','r','\0'};
    char login3[10] = {'1','2','3','\0'};
    if ((strcmp(buf + 5 ,login1) == 0) ||
        (strcmp(buf + 5,login2) == 0) ||
        (strcmp(buf + 5,login3) == 0))
    {
        printf("Login OK\n");
        loginOK = 1;
    }
    else
    {
        printf("Login not found!!!\n");
    }
}

void checkPassword(char* buf)
{
    if (strncmp(buf + 8, MD5 , 32) == 0)
    {
        Printf("password OK\r\n");
        passwordOK = 1;
    }
    else
    {
        Printf("password not OK\r\n");
    }
}

void setNewHostIP(char * tmpbuf)
{
    printf("setNewHostIP: %s\n",tmpbuf);
    uint8_t temp[4];
    char host_IP_1[3];char host_IP_2[3];char host_IP_3[3];char host_IP_4[3];
    int i=0;
    uint8_t j = 0;
    char oktet[3];
    while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
    //i указывает на '.'  j - колл скопированных символов
    if (j == 3) {host_IP_1[0] = oktet[0]; host_IP_1[1] = oktet[1]; host_IP_1[2] = oktet[2];}
    if (j == 2) {host_IP_1[0] = '0'; host_IP_1[1] = oktet[0]; host_IP_1[2] = oktet[1];}
    if (j == 1) {host_IP_1[0] = '0'; host_IP_1[1] = '0'; host_IP_1[2] = oktet[0];}
    temp[0] = atoi(host_IP_1);
    i++; j=0;
    while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
    if (j == 3) {host_IP_2[0] = oktet[0]; host_IP_2[1] = oktet[1]; host_IP_2[2] = oktet[2];}
    if (j == 2) {host_IP_2[0] = '0'; host_IP_2[1] = oktet[0]; host_IP_2[2] = oktet[1];}
    if (j == 1) {host_IP_2[0] = '0'; host_IP_2[1] = '0'; host_IP_2[2] = oktet[0];}
    temp[1] = atoi(host_IP_2);
    i++; j=0;
    while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
    if (j == 3) {host_IP_3[0] = oktet[0]; host_IP_3[1] = oktet[1]; host_IP_3[2] = oktet[2];}
    if (j == 2) {host_IP_3[0] = '0'; host_IP_3[1] = oktet[0]; host_IP_3[2] = oktet[1];}
    if (j == 1) {host_IP_3[0] = '0'; host_IP_3[1] = '0'; host_IP_3[2] = oktet[0];}
    temp[2] = atoi(host_IP_3);
    i++; j=0;
    while (1) {if(tmpbuf[i] == (uint8_t)'\0') break; oktet[j] = tmpbuf[i]; i++; j++; }
    if (j == 3) {host_IP_4[0] = oktet[0]; host_IP_4[1] = oktet[1]; host_IP_4[2] = oktet[2];}
    if (j == 2) {host_IP_4[0] = '0'; host_IP_4[1] = oktet[0]; host_IP_4[2] = oktet[1];}
    if (j == 1) {host_IP_4[0] = '0'; host_IP_4[1] = '0'; host_IP_4[2] = oktet[0];}
    temp[3] = atoi(host_IP_4);

    if ((temp[0] == ipaddr[0])&&(temp[1] == ipaddr[1])&&(temp[2] == ipaddr[2])&&(temp[3] == ipaddr[3]))
    {
         printf("*****  hostIP not changed  *****\r\n");
    }
    else
    {
        ipaddr[0] = temp[0];
        ipaddr[1] = temp[1];
        ipaddr[2] = temp[2];
        ipaddr[3] = temp[3];
        printf("new host IP: %d.%d.%d.%d\r\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
        if (sdCartOn == 1)
        {
            FRESULT result = f_open(&fil, "host_IP", FA_OPEN_ALWAYS | FA_WRITE );
            if (result == 0)
            {
                printf("*****  write new host IP to SD  *****\n");
                UINT bw;
                char lf[1] = {'\n'};
                char tmp[16];
                strncpy(tmp,host_IP_1,3);
                strncpy(tmp+3,".",1);
                strncpy(tmp+4,host_IP_2,3);
                strncpy(tmp+7,".",1);
                strncpy(tmp+8,host_IP_3,3);
                strncpy(tmp+11,".",1);
                strncpy(tmp+12,host_IP_4,3);
                strncpy(tmp+15,lf, 1);
                f_write(&fil, tmp, 16, &bw);
                f_close(&fil);
            }
        }
        else //EEPROM
        {
            printf("*****  write new host IP to eeprom  *****\n");
            BSP_EEPROM_WriteBuffer((uint8_t *)host_IP_1, ipSettingAdressInEEPROM, 3);
            BSP_EEPROM_WriteBuffer((uint8_t *)host_IP_2, ipSettingAdressInEEPROM + 4, 3);
            BSP_EEPROM_WriteBuffer((uint8_t *)host_IP_3, ipSettingAdressInEEPROM + 8, 3);
            BSP_EEPROM_WriteBuffer((uint8_t *)host_IP_4, ipSettingAdressInEEPROM + 12, 3);
        }
    }
}

void setNewMaskIP(char * tmpbuf)
{
    uint8_t temp[4];
    char mask_IP_1[3];char mask_IP_2[3];char mask_IP_3[3];char mask_IP_4[3];
    int i=0;
    uint8_t j = 0;
    char oktet[3];
    while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
    //i указывает на '.'  j - колл скопированных символов
    if (j == 3) {mask_IP_1[0] = oktet[0]; mask_IP_1[1] = oktet[1]; mask_IP_1[2] = oktet[2]; }
    if (j == 2) {mask_IP_1[0] = '0'; mask_IP_1[1] = oktet[0]; mask_IP_1[2] = oktet[1];}
    if (j == 1) {mask_IP_1[0] = '0'; mask_IP_1[1] = '0'; mask_IP_1[2] = oktet[0];}
    temp[0] = atoi(mask_IP_1);
    i++; j=0;
    while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
    if (j == 3) {mask_IP_2[0] = oktet[0]; mask_IP_2[1] = oktet[1]; mask_IP_2[2] = oktet[2];}
    if (j == 2) {mask_IP_2[0] = '0'; mask_IP_2[1] = oktet[0]; mask_IP_2[2] = oktet[1];}
    if (j == 1) {mask_IP_2[0] = '0'; mask_IP_2[1] = '0'; mask_IP_2[2] = oktet[0]; }
    temp[1] = atoi(mask_IP_2);
    i++; j=0;
    while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
    if (j == 3) {mask_IP_3[0] = oktet[0]; mask_IP_3[1] = oktet[1]; mask_IP_3[2] = oktet[2];}
    if (j == 2) {mask_IP_3[0] = '0'; mask_IP_3[1] = oktet[0]; mask_IP_3[2] = oktet[1];}
    if (j == 1) {mask_IP_3[0] = '0'; mask_IP_3[1] = '0'; mask_IP_3[2] = oktet[0];}
    temp[2] = atoi(mask_IP_3);
    i++; j=0;
    while (1) {if(tmpbuf[i] == (uint8_t)'\0') break; oktet[j] = tmpbuf[i]; i++; j++; }
    if (j == 3) {mask_IP_4[0] = oktet[0]; mask_IP_4[1] = oktet[1]; mask_IP_4[2] = oktet[2];}
    if (j == 2) {mask_IP_4[0] = '0'; mask_IP_4[1] = oktet[0]; mask_IP_4[2] = oktet[1];}
    if (j == 1) {mask_IP_4[0] = '0'; mask_IP_4[1] = '0'; mask_IP_4[2] = oktet[0];}
    temp[3] = atoi(mask_IP_4);

    if ((temp[0] == ipmask[0])&&(temp[1] == ipmask[1])&&(temp[2] == ipmask[2])&&(temp[3] == ipmask[3]))
    {
         printf("*****  maskIP not changed  *****\n");
    }
    else
    {
        ipmask[0] = temp[0];
        ipmask[1] = temp[1];
        ipmask[2] = temp[2];
        ipmask[3] = temp[3];

        printf("new mask IP: %d.%d.%d.%d\r\n",ipmask[0],ipmask[1],ipmask[2],ipmask[3]);
        if (sdCartOn == 1)
        {
            FRESULT result = f_open(&fil, "mask_IP", FA_OPEN_ALWAYS | FA_WRITE );
            if (result == 0)
            {
                printf("*****  write new mask IP to SD  *****\n");
                UINT bw;
                char lf[1] = {'\n'};
                char tmp[16];
                strncpy(tmp,mask_IP_1,3);
                strncpy(tmp+3,".",1);
                strncpy(tmp+4,mask_IP_2,3);
                strncpy(tmp+7,".",1);
                strncpy(tmp+8,mask_IP_3,3);
                strncpy(tmp+11,".",1);
                strncpy(tmp+12,mask_IP_4,3);
                strncpy(tmp+15,lf, 1);
                f_write(&fil, tmp, 16, &bw);
                f_close(&fil);
            }
        }
        else //EEPROM
        {
            printf("*****  write new mask IP to eeprom  *****\n");
            BSP_EEPROM_WriteBuffer((uint8_t *)mask_IP_1, ipSettingAdressInEEPROM + 45, 3);
            BSP_EEPROM_WriteBuffer((uint8_t *)mask_IP_2, ipSettingAdressInEEPROM + 49, 3);
            BSP_EEPROM_WriteBuffer((uint8_t *)mask_IP_3, ipSettingAdressInEEPROM + 54, 3);
            BSP_EEPROM_WriteBuffer((uint8_t *)mask_IP_4, ipSettingAdressInEEPROM + 58, 3);
        }
    }
}

void setNewGateIP(char * tmpbuf)
{
    uint8_t temp[4];
    char gate_IP_1[3];char gate_IP_2[3];char gate_IP_3[3];char gate_IP_4[3];
    int i=0;
    uint8_t j = 0;
    char oktet[3];
    while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
    //i указывает на '.'  j - колл скопированных символов
    if (j == 3) {gate_IP_1[0] = oktet[0]; gate_IP_1[1] = oktet[1]; gate_IP_1[2] = oktet[2];}
    if (j == 2) {gate_IP_1[0] = '0'; gate_IP_1[1] = oktet[0]; gate_IP_1[2] = oktet[1];}
    if (j == 1) {gate_IP_1[0] = '0'; gate_IP_1[1] = '0'; gate_IP_1[2] = oktet[0];}
    temp[0] = atoi(gate_IP_1);
    i++; j=0;
    while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
    if (j == 3) {gate_IP_2[0] = oktet[0]; gate_IP_2[1] = oktet[1]; gate_IP_2[2] = oktet[2];}
    if (j == 2) {gate_IP_2[0] = '0'; gate_IP_2[1] = oktet[0]; gate_IP_2[2] = oktet[1];}
    if (j == 1) {gate_IP_2[0] = '0'; gate_IP_2[1] = '0'; gate_IP_2[2] = oktet[0];}
    temp[1] = atoi(gate_IP_2);
    i++; j=0;
    while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
    if (j == 3) {gate_IP_3[0] = oktet[0]; gate_IP_3[1] = oktet[1]; gate_IP_3[2] = oktet[2];}
    if (j == 2) {gate_IP_3[0] = '0'; gate_IP_3[1] = oktet[0]; gate_IP_3[2] = oktet[1];}
    if (j == 1) {gate_IP_3[0] = '0'; gate_IP_3[1] = '0'; gate_IP_3[2] = oktet[0];}
    temp[2] = atoi(gate_IP_3);
    i++; j=0;
    while (1) {if(tmpbuf[i] == (uint8_t)'\0') break; oktet[j] = tmpbuf[i]; i++; j++; }
    if (j == 3) {gate_IP_4[0] = oktet[0]; gate_IP_4[1] = oktet[1]; gate_IP_4[2] = oktet[2];}
    if (j == 2) {gate_IP_4[0] = '0'; gate_IP_4[1] = oktet[0]; gate_IP_4[2] = oktet[1];}
    if (j == 1) {gate_IP_4[0] = '0'; gate_IP_4[1] = '0'; gate_IP_4[2] = oktet[0];}
    temp[3] = atoi(gate_IP_4);

    if ((temp[0] == ipgate[0])&&(temp[1] == ipgate[1])&&(temp[2] == ipgate[2])&&(temp[3] == ipgate[3]))
    {
         Printf("*****  gateIP not changed  *****\r\n");
    }
    else
    {
        ipgate[0] = temp[0];
        ipgate[1] = temp[1];
        ipgate[2] = temp[2];
        ipgate[3] = temp[3];

        printf("new gate IP: %d.%d.%d.%d\r\n",ipgate[0],ipgate[1],ipgate[2],ipgate[3]);
        if (sdCartOn == 1)
        {
            FRESULT result = f_open(&fil, "gate_IP", FA_OPEN_ALWAYS | FA_WRITE );
            if (result == 0)
            {
                printf("*****  write new gate IP to SD  *****\n");
                UINT bw;
                char lf[1] = {'\n'};
                char tmp[16];
                strncpy(tmp,gate_IP_1,3);
                strncpy(tmp+3,".",1);
                strncpy(tmp+4,gate_IP_2,3);
                strncpy(tmp+7,".",1);
                strncpy(tmp+8,gate_IP_3,3);
                strncpy(tmp+11,".",1);
                strncpy(tmp+12,gate_IP_4,3);
                strncpy(tmp+15,lf, 1);
                f_write(&fil, tmp, 16, &bw);
                f_close(&fil);
            }
        }
        else //EEPROM
        {
            printf("*****  write new gate IP to eeprom  *****\n");
            BSP_EEPROM_WriteBuffer((uint8_t *)gate_IP_1, ipSettingAdressInEEPROM + 30, 3);
            BSP_EEPROM_WriteBuffer((uint8_t *)gate_IP_2, ipSettingAdressInEEPROM + 34, 3);
            BSP_EEPROM_WriteBuffer((uint8_t *)gate_IP_3, ipSettingAdressInEEPROM + 38, 3);
            BSP_EEPROM_WriteBuffer((uint8_t *)gate_IP_4, ipSettingAdressInEEPROM + 42, 3);
        }
    }
}

void setNewDestIP(char * tmpbuf)
{
    uint8_t temp[4];
    char dest_IP_1[3];char dest_IP_2[3];char dest_IP_3[3];char dest_IP_4[3];
    int i=0;
    uint8_t j = 0;
    char oktet[3];
    while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
    //i указывает на '.'  j - колл скопированных символов
    if (j == 3) {dest_IP_1[0] = oktet[0]; dest_IP_1[1] = oktet[1]; dest_IP_1[2] = oktet[2];}
    if (j == 2) {dest_IP_1[0] = '0'; dest_IP_1[1] = oktet[0]; dest_IP_1[2] = oktet[1];}
    if (j == 1) {dest_IP_1[0] = '0'; dest_IP_1[1] = '0'; dest_IP_1[2] = oktet[0];}
    temp[0] = atoi(dest_IP_1);
    i++; j=0;
    while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
    if (j == 3) {dest_IP_2[0] = oktet[0]; dest_IP_2[1] = oktet[1]; dest_IP_2[2] = oktet[2];}
    if (j == 2) {dest_IP_2[0] = '0'; dest_IP_2[1] = oktet[0]; dest_IP_2[2] = oktet[1];}
    if (j == 1) {dest_IP_2[0] = '0'; dest_IP_2[1] = '0'; dest_IP_2[2] = oktet[0];}
    temp[1] = atoi(dest_IP_2);
    i++; j=0;
    while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
    if (j == 3) {dest_IP_3[0] = oktet[0]; dest_IP_3[1] = oktet[1]; dest_IP_3[2] = oktet[2];}
    if (j == 2) {dest_IP_3[0] = '0'; dest_IP_3[1] = oktet[0]; dest_IP_3[2] = oktet[1];}
    if (j == 1) {dest_IP_3[0] = '0'; dest_IP_3[1] = '0'; dest_IP_3[2] = oktet[0];}
    temp[2] = atoi(dest_IP_3);
    i++; j=0;
    while (1) {if(tmpbuf[i] == (uint8_t)'\0') break; oktet[j] = tmpbuf[i]; i++; j++; }
    if (j == 3) {dest_IP_4[0] = oktet[0]; dest_IP_4[1] = oktet[1]; dest_IP_4[2] = oktet[2];}
    if (j == 2) {dest_IP_4[0] = '0'; dest_IP_4[1] = oktet[0]; dest_IP_4[2] = oktet[1];}
    if (j == 1) {dest_IP_4[0] = '0'; dest_IP_4[1] = '0'; dest_IP_4[2] = oktet[0];}
    temp[3] = atoi(dest_IP_4);

    if ((temp[0] == destip[0])&&(temp[1] == destip[1])&&(temp[2] == destip[2])&&(temp[3] == destip[3]))
    {
         Printf("*****  destIP not changed  *****\r\n");
    }
    else
    {
        destip[0] = temp[0];
        destip[1] = temp[1];
        destip[2] = temp[2];
        destip[3] = temp[3];

        printf("new dest IP: %d.%d.%d.%d\r\n",destip[0],destip[1],destip[2],destip[3]);
        if (sdCartOn == 1)
        {
            FRESULT result = f_open(&fil, "dest_IP", FA_OPEN_ALWAYS | FA_WRITE );
            if (result == 0)
            {
                printf("*****  write new dest IP to SD  *****\n");
                UINT bw;
                char lf[1] = {'\n'};
                char tmp[16];
                strncpy(tmp,dest_IP_1,3);
                strncpy(tmp+3,".",1);
                strncpy(tmp+4,dest_IP_2,3);
                strncpy(tmp+7,".",1);
                strncpy(tmp+8,dest_IP_3,3);
                strncpy(tmp+11,".",1);
                strncpy(tmp+12,dest_IP_4,3);
                strncpy(tmp+15,lf, 1);
                f_write(&fil, tmp, 16, &bw);
                f_close(&fil);
            }
        }
        else //EEPROM
        {
            printf("*****  write new dest IP to eeprom  *****\n");
            BSP_EEPROM_WriteBuffer((uint8_t *)dest_IP_1, ipSettingAdressInEEPROM + 15, 3);
            BSP_EEPROM_WriteBuffer((uint8_t *)dest_IP_2, ipSettingAdressInEEPROM + 19, 3);
            BSP_EEPROM_WriteBuffer((uint8_t *)dest_IP_3, ipSettingAdressInEEPROM + 23, 3);
            BSP_EEPROM_WriteBuffer((uint8_t *)dest_IP_4, ipSettingAdressInEEPROM + 27, 3);
        }
    }
}

void setNewPassword(char * tmpbuf)
{
    printf("new pasword hash: %.32s",tmpbuf);
    printf("\r\n");
    if (sdCartOn == 1)
    {
        FRESULT result = f_open(&fil, "md5", FA_OPEN_ALWAYS | FA_WRITE );
        if (result == 0)
        {
            printf("*****  write new md5 to SD  *****\n");
            f_lseek(&fil, 0);
            f_puts(tmpbuf, &fil);
            f_sync(&fil);
            f_close(&fil);
        }
    }
    else //EEPROM
    {
        printf("*****  write new md5 to eeprom  *****\n");
        BSP_EEPROM_WriteBuffer((uint8_t *)tmpbuf, ipSettingAdressInEEPROM + 60, 33);
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART6_UART_Init();
  MX_SPI3_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_FATFS_Init();
  MX_RNG_Init();
  MX_RTC_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
    printf("Test UART...OK\r\n");
    workI2C_EEPROM(); //  выбор eeprom i2c_eeprom и загрузка параметров
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_RESET) //Я в централи - сигналл выдает ПЛИС
    {
        ABONENT_or_BASE = 0;
        printf("WORK in BASE INTRON\r\n");
    }
    else //Я в абоненте - сигналл выдает ПЛИС
    {
        ABONENT_or_BASE = 1;
        printf("WORK in ABONENT\r\n");
    }
#ifndef   NEW_HTTP_SERVER
//    net_ini();
#endif
    net_ini_WIZNET(0); //TCP socket 0

//    workSPI_EEPROM();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    uint8_t udpSocket = 4;
    prepearUDP_PLIS(udpSocket);

#ifdef   NEW_HTTP_SERVER //web serverWIZ
    uint8_t i;
//    httpServer_init(TX_BUF_WEB, RX_BUF_WEB, MAX_HTTPSOCK, socknumlist);
//    wep_define_func();
//    display_reg_webContent_list();
#endif

//    tls_client_serverTest(); // работает
//    polarSSLTest();
//    bearSSLTest();

//uint8_t firstSend = 1;
  while (1)
  {

#ifdef   NEW_HTTP_SERVER
    for(i = 0; i < MAX_HTTPSOCK; i++)
    {
//        httpServer_run(i);
    }
#endif
#ifndef   NEW_HTTP_SERVER
      net_poll();
#endif

      if (HANDSHAKE == 1)
          sendReceiveUDP(udpSocket);
      if (HANDSHAKE == 0)
          sendHANDSHAKE(udpSocket);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }


  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_MAY;
  sDate.Date = 0x9;
  sDate.Year = 0x23;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 230400;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, CS_EEPROM_Pin|HOLD_EEPROM_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(WP_EEPROM_GPIO_Port, WP_EEPROM_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, OTG_FS_PowerSwitchOn_Pin|GPIO_PIN_9, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9|GPIO_PIN_10|Green_Led_Pin|Orange_Led_Pin
                          |Red_Led_Pin|Blue_Led_Pin|GPIO_PIN_0|GPIO_PIN_1
                          |GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11|GPIO_PIN_2, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

  /*Configure GPIO pin : CS_EEPROM_Pin */
  GPIO_InitStruct.Pin = CS_EEPROM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(CS_EEPROM_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : WP_EEPROM_Pin HOLD_EEPROM_Pin */
  GPIO_InitStruct.Pin = WP_EEPROM_Pin|HOLD_EEPROM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : OTG_FS_PowerSwitchOn_Pin PC4 PC5 */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : BOOT1_Pin PB15 */
  GPIO_InitStruct.Pin = BOOT1_Pin|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : INT_Pin */
  GPIO_InitStruct.Pin = INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PD9 PD10 PD11 Green_Led_Pin
                           Orange_Led_Pin Red_Led_Pin Blue_Led_Pin PD0
                           PD1 PD3 PD4 PD5
                           PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|Green_Led_Pin
                          |Orange_Led_Pin|Red_Led_Pin|Blue_Led_Pin|GPIO_PIN_0
                          |GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void sendPackets(uint8_t sn, uint8_t* destip, uint16_t destport)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
    if (ABONENT_or_BASE == 0) {  //База
//        sendto(sn, (uint8_t *)rxCyclon, MAX_PACKET_LEN, destip, destport);
        sendto(sn, (uint8_t *)test4, MAX_PACKET_LEN, destip, destport);
    }
    if (ABONENT_or_BASE == 1) {  //Абонентский мост
//        sendto(sn, (uint8_t *)rxCyclon, MAX_PACKET_LEN, destip, destport);
        sendto(sn, (uint8_t *)test4, MAX_PACKET_LEN, destip, destport);
    }

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
    ++num_send;
    if (num_send == 500){
        HAL_GPIO_WritePin(GPIOD, Green_Led_Pin, GPIO_PIN_RESET);
    }
    if (num_send == 1000){
        num_send = 0;
        HAL_GPIO_WritePin(GPIOD, Green_Led_Pin, GPIO_PIN_SET);
    }
}

void receivePackets(uint8_t sn, uint8_t* destip, uint16_t destport)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
    recvfrom(sn, (uint8_t *)txCyclon, MAX_PACKET_LEN, destip, &destport);
     if (0 != strncmp((const char*)txCyclon, (const char*)test4, MAX_PACKET_LEN)) //Для теста
     {
         printf("txCyclon - "
              "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
              "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
              "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
              "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
              "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
              "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
              "\r\n",
               txCyclon[0],txCyclon[1],txCyclon[2],txCyclon[3],txCyclon[4],txCyclon[5],txCyclon[6],txCyclon[7],
               txCyclon[8],txCyclon[9],txCyclon[10],txCyclon[11],txCyclon[12],txCyclon[13],txCyclon[14],txCyclon[15],
               txCyclon[16],txCyclon[17],txCyclon[18],txCyclon[19],txCyclon[20],txCyclon[21],txCyclon[22],txCyclon[23],
               txCyclon[24],txCyclon[25],txCyclon[26],txCyclon[27],txCyclon[28],txCyclon[29],txCyclon[30],txCyclon[31],
               txCyclon[32],txCyclon[33],txCyclon[34],txCyclon[35],txCyclon[36],txCyclon[37],txCyclon[38],txCyclon[39],
               txCyclon[40],txCyclon[41],txCyclon[42],txCyclon[43],txCyclon[44],txCyclon[45],txCyclon[46],txCyclon[47]
               );
             printf("error - "
                  "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
                  "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
                  "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
                  "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
                  "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
                  "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
                  "\r\n",
                   test4[0],test4[1],test4[2],test4[3],test4[4],test4[5],test4[6],test4[7],
                   test4[8],test4[9],test4[10],test4[11],test4[12],test4[13],test4[14],test4[15],
                   test4[16],test4[17],test4[18],test4[19],test4[20],test4[21],test4[22],test4[23],
                   test4[24],test4[25],test4[26],test4[27],test4[28],test4[29],test4[30],test4[31],
                   test4[32],test4[33],test4[34],test4[35],test4[36],test4[37],test4[38],test4[39],
                   test4[40],test4[41],test4[42],test4[43],test4[44],test4[45],test4[46],test4[47]
                   );
         HAL_GPIO_WritePin(GPIOD, Red_Led_Pin, GPIO_PIN_SET);
     }


    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);

    ++num_rcvd;
    if (num_rcvd == 500){
        HAL_GPIO_WritePin(GPIOD, Red_Led_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOD, Blue_Led_Pin, GPIO_PIN_RESET);
    }
    if (num_rcvd == 1000){
        num_rcvd = 0;
        HAL_GPIO_WritePin(GPIOD, Blue_Led_Pin, GPIO_PIN_SET);
    }
}

int convertHexToDecimal(char hexadecimalnumber[2])
{
    int decimalNumber = 0;
    char hexDigits[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    for ( int j = 0; j < 16; j++)
    {
       if (hexadecimalnumber[0] == hexDigits[j])
           decimalNumber += j * 16;
    }
    for (int j = 0; j < 16; j++)
    {
       if (hexadecimalnumber[1] == hexDigits[j])
           decimalNumber += j;
    }
//    printf("Decimal Number : %d", decimalNumber);
    return decimalNumber;
}
//    HAL_SPI_TransmitReceive(&hspi2, test1 , rxCyclon, MAX_PACKET_LEN, 0x1000);
//    printf("%u\trxCyclon - "
//         "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
//         "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
//         "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
//         "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
//         "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
//         "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
//         "\r\n",
//         HAL_GetTick(),
//          rxCyclon[0],rxCyclon[1],rxCyclon[2],rxCyclon[3],rxCyclon[4],rxCyclon[5],rxCyclon[6],rxCyclon[7],
//          rxCyclon[8],rxCyclon[9],rxCyclon[10],rxCyclon[11],rxCyclon[12],rxCyclon[13],rxCyclon[14],rxCyclon[15],
//          rxCyclon[16],rxCyclon[17],rxCyclon[18],rxCyclon[19],rxCyclon[20],rxCyclon[21],rxCyclon[22],rxCyclon[23],
//          rxCyclon[24],rxCyclon[25],rxCyclon[26],rxCyclon[27],rxCyclon[28],rxCyclon[29],rxCyclon[30],rxCyclon[31],
//          rxCyclon[32],rxCyclon[33],rxCyclon[34],rxCyclon[35],rxCyclon[36],rxCyclon[37],rxCyclon[38],rxCyclon[39],
//          rxCyclon[40],rxCyclon[41],rxCyclon[42],rxCyclon[43],rxCyclon[44],rxCyclon[45],rxCyclon[46],rxCyclon[47]
//          );
//    printf("\ttxCyclon - "
//         "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
//         "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
//         "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
//         "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
//         "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
//         "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X"
//         "\r\n",
//          test1[0],test1[1],test1[2],test1[3],test1[4],test1[5],test1[6],test1[7],
//          test1[8],test1[9],test1[10],test1[11],test1[12],test1[13],test1[14],test1[15],
//          test1[16],test1[17],test1[18],test1[19],test1[20],test1[21],test1[22],test1[23],
//          test1[24],test1[25],test1[26],test1[27],test1[28],test1[29],test1[30],test1[31],
//          test1[32],test1[33],test1[34],test1[35],test1[36],test1[37],test1[38],test1[39],
//          test1[40],test1[41],test1[42],test1[43],test1[44],test1[45],test1[46],test1[47]
//          );

//     if(strcmp((char*)rxCyclon, (char*)zeroStr) != 0)
//         printf("string no zero\r\n");
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

