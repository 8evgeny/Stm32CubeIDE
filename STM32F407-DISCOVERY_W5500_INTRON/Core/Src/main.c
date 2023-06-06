/* USER CODE BEGIN Header */

#include "socket.h"
#include "w5500.h"
#include "net.h"
#include "loopback.h"
#include "my_function.h"
#include "eeprom.h"
#include "wizchip_init.h"
#include "SSLInterface.h"
#include "httpServer.h"
#include "webpage.h"

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
extern int tls_server_sizeTest();
extern int tls_sock_serverTest();
extern void tlsProcess();

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define I2C_REQUEST_WRITE                       0x00
#define I2C_REQUEST_READ                        0x01
#define SLAVE_OWN_ADDRESS                       0xA0
uint32_t count = 0;
uint8_t sdCartOn = 0;
char *pindex;
extern lfs_t lfs;
extern lfs_file_t file;
uint8_t num_block_index = 1;
uint8_t num_block_main = 8;
char indexLen[8];
#define WRITE_ONCE_TO_EEPROM 1024
uint16_t local_port = LOCAL_PORT;


//uint8_t txBuf[MAX_PACKET_LEN ]= {0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55};
//uint8_t txBufW5500[MAX_PACKET_LEN ]= {0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55};

//uint8_t txCyclon[32]= {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
//                       0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};

//uint8_t txCyclon[32]= {0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,};

//uint8_t txCyclon[32]= {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};

uint8_t txCyclon[MAX_PACKET_LEN];
uint8_t rxCyclon[MAX_PACKET_LEN];
//uint8_t test[32] = {0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff,
//                    0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff};
uint8_t test1[MAX_PACKET_LEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                                 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                                0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
uint8_t test2[MAX_PACKET_LEN] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
uint8_t test3[MAX_PACKET_LEN] = {0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
                                0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
                                0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0xff};
uint8_t test4[MAX_PACKET_LEN] = {0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
                                0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
                                0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f};
uint8_t test5[MAX_PACKET_LEN] = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
                                0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
                                0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99};
uint8_t test6[MAX_PACKET_LEN] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                                0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                                0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
uint32_t num_send = 0;
uint32_t num_rcvd = 0;
uint32_t num;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RNG_HandleTypeDef hrng;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;
DMA_HandleTypeDef hdma_spi3_rx;
DMA_HandleTypeDef hdma_spi3_tx;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart6_tx;

/* USER CODE BEGIN PV */

uint8_t capture = 0;
extern uint8_t ipaddr[4];
extern uint8_t ipgate[4];
extern uint8_t ipmask[4];
char MD5[32];
uint8_t destip[4];
extern uint8_t macaddr[6];
extern wiz_NetInfo defaultNetInfo;
uint8_t RX_BUF[DATA_BUF_SIZE];
uint8_t TX_BUF[DATA_BUF_SIZE];
#define MAX_HTTPSOCK	4
uint8_t socknumlist[] = {0, 1, 2, 3};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_SPI3_Init(void);
static void MX_TIM1_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_RNG_Init(void);
static void MX_RTC_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

void UART_Printf(const char* fmt, ...);
extern void print_network_information(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
// Этот обратный вызов автоматически вызывается HAL при возникновении события UEV
    if(htim->Instance == TIM1) //check if the interrupt comes from TIM1
    {
        ++capture;
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);

        if (capture == 2)
        {
            ++count;
//            HAL_SPI_TransmitReceive(&hspi3, txBuf , rxBuf, MAX_PACKET_LEN, 0x1000);
//            memcpy(txBuf, rxBuf + 1, MAX_PACKET_LEN);
        }
        if (capture == 1)
        {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);

        }
    }

}

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
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    capture = 0;
//    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
//    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

FATFS fs;
FIL fil;
void printFileFromEEPROM(const char* nameFile_onEEPROM);
void testEEPROM()
{
    uint8_t rd_value[36] = {0};
    uint8_t wr_value[36] = {'f','b','c','d','e','f','g','i','j','k','l','m','n','o','p','q',
                            'r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0','\0'};
    uint8_t erase_value[36] = {'H','e','f','l','o'};

//    AT24C_ReadBytes (0x004A, rd_value, 36);
    uint16_t num = 36;
    BSP_EEPROM_ReadBuffer(rd_value, 0x004A, &num);
    UART_Printf("EEPROM read: %s\r\n",rd_value); delayUS_ASM(20000);

    UART_Printf("EEPROM write:"); delayUS_ASM(20000);
    UART_Printf("%s\r\n",erase_value); delayUS_ASM(20000);
    BSP_EEPROM_WriteBuffer(erase_value, 0x004A, 36);
//    AT24C_WriteBytes (0x004A, erase_value, 36);

    delayUS_ASM(100000);

//    AT24C_ReadBytes (0x004A, rd_value, 36);
    BSP_EEPROM_ReadBuffer(rd_value, 0x004A, &num);
    UART_Printf("EEPROM read: %s\r\n",rd_value); delayUS_ASM(10000);

    UART_Printf("EEPROM write:"); delayUS_ASM(10000);
    UART_Printf("%s\r\n",wr_value); delayUS_ASM(10000);
    BSP_EEPROM_WriteBuffer(wr_value, 0x004A, 36);
//    AT24C_WriteBytes (0x004A, wr_value, 36);

    delayUS_ASM(100000);

//    AT24C_ReadBytes (0x004A, rd_value, 36);
    BSP_EEPROM_ReadBuffer(rd_value, 0x004A, &num);
    UART_Printf("EEPROM read: %s\r\n",rd_value); delayUS_ASM(10000);
}

void copyFileToEEPROM(const char* nameFile_onSD)
{
    uint32_t time = HAL_GetTick();
    f_open(&fil, nameFile_onSD, FA_OPEN_ALWAYS | FA_READ );
    uint32_t numByteFile = fil.obj.objsize;
    UINT rc;
    pindex = malloc(numByteFile);
    f_read(&fil, pindex, numByteFile, &rc);
    f_close(&fil);

    UART_Printf("copy %s ... ", nameFile_onSD); delayUS_ASM(5000);
    lfs_file_open(&lfs, &file, nameFile_onSD, LFS_O_WRONLY | LFS_O_CREAT );
    lfs_file_write(&lfs, &file, pindex, numByteFile);
    lfs_file_close(&lfs, &file);
    memset(pindex, 0x00, numByteFile);
    free (pindex);
    char tmp[30];
    sprintf(tmp,"%ds\n", (HAL_GetTick() - time)/1000 );
    UART_Printf(tmp); delayUS_ASM(5000);
//    printFileFromEEPROM(nameFile_onSD);
}

void printFileFromEEPROM(const char* nameFile_onEEPROM)
{
    lfs_file_open(&lfs, &file, nameFile_onEEPROM, LFS_O_RDONLY );
    uint32_t numByteFile = lfs_file_size(&lfs, &file);
    pindex = malloc(numByteFile);
    lfs_file_read(&lfs, &file, pindex, numByteFile);
    lfs_file_close(&lfs, &file);

    UART_Printf("print %s\n", nameFile_onEEPROM); delayUS_ASM(5000);
    for (int i = 0; i < numByteFile; ++i)
    {
        UART_Printf("%c", pindex[i]); delayUS_ASM(100);
    }
    UART_Printf("\n"); delayUS_ASM(100);
    free (pindex);
}

void testReadFile(const char* nameFile_onEEPROM)
{
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
}

void copyParametersToEEPROM()
{
    UART_Printf("copy parameters to eeprom\n"); delayUS_ASM(10000);
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
    UART_Printf("host_IP:\n%s\n", tmp); delayUS_ASM(5000);
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
    UART_Printf("dest_IP:\n%s\n", tmp); delayUS_ASM(5000);
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
    UART_Printf("gate_IP:\n%s\n", tmp); delayUS_ASM(5000);
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
    UART_Printf("mask_IP:\n%s\n", tmp); delayUS_ASM(5000);
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
}

void setParametersFromSD()
{
    char tmp[100];
    char tmp1[5];
    char tmp2[5];
    char tmp3[5];
    char tmp4[5];
    f_open(&fil, "host_IP", FA_OPEN_ALWAYS | FA_READ );
    f_gets(tmp1, 5, &fil);
    ipaddr[0] = atoi(tmp1);
    f_gets(tmp2, 5, &fil);
    ipaddr[1] = atoi(tmp2);
    f_gets(tmp3, 5, &fil);
    ipaddr[2] = atoi(tmp3);
    f_gets(tmp4, 5, &fil);
    ipaddr[3] = atoi(tmp4);
    sprintf(tmp,"host_IP: %d.%d.%d.%d\r\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
    UART_Printf(tmp); delayUS_ASM(10000);
    f_close(&fil);
    sprintf(tmp,"%.3s%.3s%.3s%.3s\r\n",tmp1,tmp2,tmp3,tmp4);

    f_open(&fil, "dest_IP", FA_OPEN_ALWAYS | FA_READ );
    f_lseek(&fil, 0);
    f_gets(tmp1, 100, &fil);
    destip[0] = atoi(tmp1);
    f_gets(tmp2, 100, &fil);
    destip[1] = atoi(tmp2);
    f_gets(tmp3, 100, &fil);
    destip[2] = atoi(tmp3);
    f_gets(tmp4, 100, &fil);
    destip[3] = atoi(tmp4);
    sprintf(tmp,"dest_IP: %d.%d.%d.%d\r\n",destip[0],destip[1],destip[2],destip[3]);
    UART_Printf(tmp); delayUS_ASM(10000);
    f_close(&fil);
    sprintf(tmp,"%.3s%.3s%.3s%.3s\r\n",tmp1,tmp2,tmp3,tmp4);

    f_open(&fil, "gate_IP", FA_OPEN_ALWAYS | FA_READ );
    f_lseek(&fil, 0);
    f_gets(tmp1, 100, &fil);
    ipgate[0] = atoi(tmp1);
    f_gets(tmp2, 100, &fil);
    ipgate[1] = atoi(tmp2);
    f_gets(tmp3, 100, &fil);
    ipgate[2] = atoi(tmp3);
    f_gets(tmp4, 100, &fil);
    ipgate[3] = atoi(tmp4);
    sprintf(tmp,"gate_IP: %d.%d.%d.%d\r\n",ipgate[0],ipgate[1],ipgate[2],ipgate[3]);
    UART_Printf(tmp); delayUS_ASM(10000);
    f_close(&fil);
    sprintf(tmp,"%.3s%.3s%.3s%.3s\r\n",tmp1,tmp2,tmp3,tmp4);

    f_open(&fil, "mask_IP", FA_OPEN_ALWAYS | FA_READ );
    f_lseek(&fil, 0);
    f_gets(tmp1, 100, &fil);
    ipmask[0] = atoi(tmp1);
    f_gets(tmp2, 100, &fil);
    ipmask[1] = atoi(tmp2);
    f_gets(tmp3, 100, &fil);
    ipmask[2] = atoi(tmp3);
    f_gets(tmp4, 100, &fil);
    ipmask[3] = atoi(tmp4);
    sprintf(tmp,"mask_IP: %d.%d.%d.%d\r\n",ipmask[0],ipmask[1],ipmask[2],ipmask[3]);
    UART_Printf(tmp); delayUS_ASM(10000);
    f_close(&fil);
    sprintf(tmp,"%.3s%.3s%.3s%.3s\r\n",tmp1,tmp2,tmp3,tmp4);

    f_open(&fil, "md5", FA_OPEN_ALWAYS | FA_READ );
    f_gets(tmp, 100, &fil);
    strncpy(MD5, tmp, 32);
    f_close(&fil);
    UART_Printf("md5: %s\n",MD5); delayUS_ASM(10000);

    sprintf(tmp,"mac: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
    UART_Printf(tmp); delayUS_ASM(10000);
}

void SetParaametersFromEEPROM()
{
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
    UART_Printf(tmp); delayUS_ASM(10000);
    sprintf(tmp,"dest_IP: %d.%d.%d.%d\r\n",destip[0],destip[1],destip[2],destip[3]);
    UART_Printf(tmp); delayUS_ASM(10000);
    sprintf(tmp,"gate_IP: %d.%d.%d.%d\r\n",ipgate[0],ipgate[1],ipgate[2],ipgate[3]);
    UART_Printf(tmp); delayUS_ASM(10000);
    sprintf(tmp,"mask_IP: %d.%d.%d.%d\r\n",ipmask[0],ipmask[1],ipmask[2],ipmask[3]);
    UART_Printf(tmp); delayUS_ASM(10000);
    sprintf(tmp,"md5: %s", MD5);
    UART_Printf(tmp); delayUS_ASM(10000);

    sprintf(tmp,"mac: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
    UART_Printf(tmp); delayUS_ASM(10000);
}

void wep_define_func(void)
{
    // Index page and netinfo / base64 image demo
    reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t *)index_page);				// index.html 		: Main page example
    reg_httpServer_webContent((uint8_t *)"netinfo.html", (uint8_t *)netinfo_page);			// netinfo.html 	: Network information example page
    reg_httpServer_webContent((uint8_t *)"netinfo.js", (uint8_t *)w5x00web_netinfo_js);     // netinfo.js 		: JavaScript for Read Network configuration 	(+ ajax.js)
    reg_httpServer_webContent((uint8_t *)"img.html", (uint8_t *)img_page);					// img.html 		: Base64 Image data example page

    // Example #1
    reg_httpServer_webContent((uint8_t *)"dio.html", (uint8_t *)dio_page);					// dio.html 		: Digital I/O control example page
    reg_httpServer_webContent((uint8_t *)"dio.js", (uint8_t *)w5x00web_dio_js);             // dio.js 			: JavaScript for digital I/O control 	(+ ajax.js)

    // AJAX JavaScript functions
    reg_httpServer_webContent((uint8_t *)"ajax.js", (uint8_t *)w5x00web_ajax_js);			// ajax.js			: JavaScript for AJAX request transfer

//    reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t *)index_page2);
//    reg_httpServer_webContent((uint8_t *)"main.html", (uint8_t *)main_page);
}

void net_ini_WIZNET()
{
    uint8_t sn_TCP = 0; // Сокет 0
    WIZCHIPInitialize();
    ctlnetwork(CN_SET_NETINFO, (void*) &defaultNetInfo);
    print_network_information();
    socket(sn_TCP, Sn_MR_TCP, local_port, 0/*SF_UNI_BLOCK*/); //У W5500 4 флага
    if (SOCK_OK == listen(sn_TCP))
        printf("socket %d listening\n", sn_TCP);
}

void workEEPROM()
{
    //  UART_Printf("Simple eeprom TEST\n"); delayUS_ASM(10000);
    //  testEEPROM();
      UART_Printf("LittleFsInit\n"); delayUS_ASM(10000);
      littleFsInit();
    //  UART_Printf("FsEeprom TEST ... "); delayUS_ASM(10000);
    //  FsForEeprom_test();

    f_mount(&fs, "", 0);
    FRESULT result = f_open(&fil, "host_IP", FA_OPEN_ALWAYS | FA_READ );
    if (result == 0)
    {
        sdCartOn = 1;
        UART_Printf("SD_READ\n");
        delayUS_ASM(10000);
    }

    if (result != 0)
        UART_Printf("SD_NOT_OPEN\n");
        delayUS_ASM(10000);
    f_close(&fil);

    if (sdCartOn == 1)
    {
        setParametersFromSD();
//        copyParametersToEEPROM();
//        copyFileToEEPROM("index.html");
//        copyFileToEEPROM("main.html");

    } else
    {
        SetParaametersFromEEPROM();
//        testReadFile("index.html");
//        testReadFile("main.html");
//        printFileFromEEPROM("index.html");
//        printFileFromEEPROM("main.html");

    }
}

void prepearUDP_PLIS()
{
    uint8_t sn = 0;
    //socket(sn, Sn_MR_UDP, 9999, SF_UNI_BLOCK);
    //char buf1[] = "abcdefghjkabcdefghjkabcdefghjkabcdefghjkabcdefghjkabcdefghjkabcdefghjkabcdefghjk\r\n";
    //char buf2[] = "1234567890\r\n";
    //char buf3[] = "2345678901\r\n";
    //char buf4[] = "3456789012\r\n";
    //char buf5[] = "4567890123\r\n";
    //char buf6[] = "5678901234\r\n";
    //char buf7[] = "6789012345\r\n";
    //char buf8[] = "7890123456\r\n";
    //char buf[82] ;
    #define SOCK_UDPS        1
    #define DATA_BUF_SIZE   2048
      extern uint8_t gDATABUF[DATA_BUF_SIZE];


    //for (uint8_t i = 4; i < 8 ;++i)
    //{
    //    socket(i, Sn_MR_UDP, localport + i, 0x00);
    //}

    //OpenSocket(0, Sn_MR_UDP); //То -же но локальный порт по умолчанию
    //OpenSocket(1, Sn_MR_UDP);
    //OpenSocket(2, Sn_MR_UDP);
    //OpenSocket(3, Sn_MR_UDP);
    //OpenSocket(4, Sn_MR_UDP);
    //OpenSocket(5, Sn_MR_UDP);
    //OpenSocket(6, Sn_MR_UDP);
    //OpenSocket(7, Sn_MR_UDP);

    #ifdef INTRON
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); //Внешнее тактирование
    #endif
    #ifndef INTRON
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); //Внешнее тактирование
    //HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); //Внутреннее тактирование
    #endif
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET); //CLK_EN (ПЛИС)
}

void sendReceiveUDP()
{
#ifdef INTRON
    for (uint8_t i = 4; i < 8 ;++i)
    {
      while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET); //Очищаю сдвиговый регистр
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

      HAL_SPI_TransmitReceive(&hspi2, txCyclon , rxCyclon, MAX_PACKET_LEN, 0x1000);

      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); //Очищаю сдвиговый регистр приема
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

      while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET); // Жду пока плис уронит флаг

      sendPackets(4, destip, destport + 4 );
      if (firstSend != 1)
          receivePackets(4, destip, destport + 4 );
    }
    firstSend = 0; //После сброса сперва отправляем 4 пакета а потом уже прием
#endif

#ifndef INTRON
//    for (uint8_t i = 4; i < 8 ;++i)
//    {
//      while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET);
//      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET); //Очищаю сдвиговый регистр
//      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

//      HAL_SPI_TransmitReceive(&hspi2, txCyclon , rxCyclon, MAX_PACKET_LEN, 0x1000);

//      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); //Очищаю сдвиговый регистр приема
//      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

//      while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET); // Жду пока плис уронит флаг

//      sendPackets(4, destip, destport + 4 );
//      if (firstSend != 1)
//          receivePackets(4, destip, destport + 4 );
//    }
//    firstSend = 0; //После сброса сперва отправляем 4 пакета а потом уже прием
#endif

//    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
//    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);

//    HAL_Delay(1000);
//UART_Printf("txCyclon1 - %.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X\r\n",
//        txCyclon[0],txCyclon[1],txCyclon[2],txCyclon[3],txCyclon[4],txCyclon[5],txCyclon[6],txCyclon[7],
//        txCyclon[8],txCyclon[9],txCyclon[10],txCyclon[11],txCyclon[12],txCyclon[13],txCyclon[14],txCyclon[15],
//        txCyclon[16],txCyclon[17],txCyclon[18],txCyclon[19],txCyclon[20],txCyclon[21],txCyclon[22],txCyclon[23],
//        txCyclon[24],txCyclon[25],txCyclon[26],txCyclon[27],txCyclon[28],txCyclon[29],txCyclon[30],txCyclon[31]
//        );
//UART_Printf("rxCyclon1 - %.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X\r\n",
//        rxCyclon[0],rxCyclon[1],rxCyclon[2],rxCyclon[3],rxCyclon[4],rxCyclon[5],rxCyclon[6],rxCyclon[7],
//        rxCyclon[8],rxCyclon[9],rxCyclon[10],rxCyclon[11],rxCyclon[12],rxCyclon[13],rxCyclon[14],rxCyclon[15],
//        rxCyclon[16],rxCyclon[17],rxCyclon[18],rxCyclon[19],rxCyclon[20],rxCyclon[21],rxCyclon[22],rxCyclon[23],
//        rxCyclon[24],rxCyclon[25],rxCyclon[26],rxCyclon[27],rxCyclon[28],rxCyclon[29],rxCyclon[30],rxCyclon[31]
//        );
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
  MX_TIM1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_FATFS_Init();
  MX_RNG_Init();
  MX_RTC_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

    workEEPROM();
//    net_ini();
    net_ini_WIZNET();// Делаю то-же но на родной библиотеке

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    prepearUDP_PLIS();

//    tls_client_serverTest(); // работает
    tls_server_sizeTest();

//web serverWIZ - РАБОТАЕТ
//    uint8_t i;
//    httpServer_init(TX_BUF, RX_BUF, MAX_HTTPSOCK, socknumlist);
//    wep_define_func();
//    display_reg_webContent_list();

//uint8_t firstSend = 1;
  while (1)
  {

//web serverWIZ - РАБОТАЕТ
//    for(i = 0; i < MAX_HTTPSOCK; i++) {httpServer_run(i);}
//      httpServer_run(0);

//      net_poll(); //Старый код http сервер


      sendReceiveUDP();

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
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
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 20730;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim1, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

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
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, OTG_FS_PowerSwitchOn_Pin|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9|GPIO_PIN_10|Green_Led_Pin|Orange_Led_Pin
                          |Red_Led_Pin|Blue_Led_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11|GPIO_PIN_2, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

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
                           Orange_Led_Pin Red_Led_Pin Blue_Led_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|Green_Led_Pin
                          |Orange_Led_Pin|Red_Led_Pin|Blue_Led_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
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
    char tmp[20];
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);

    sendto(sn, (uint8_t *)rxCyclon, MAX_PACKET_LEN, destip, destport);
//    sendto_mod(sn, (uint8_t *)test6, MAX_PACKET_LEN, destip, destport);

    ++num_send;
//    ++num;
//    if (num % 20000 == 0)
//    {
//        sprintf(tmp, "%u\r\n",num);
//        UART_Printf(tmp);
//    }
    if (num_send == 500)
    {
        HAL_GPIO_WritePin(GPIOD, Orange_Led_Pin, GPIO_PIN_RESET);
    }
    if (num_send == 1000)
    {
//        close(0);
//        socket(0, Sn_MR_UDP, localport, 0x00);
        num_send = 0;
        HAL_GPIO_WritePin(GPIOD, Orange_Led_Pin, GPIO_PIN_SET);
    }
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);
}

void receivePackets(uint8_t sn, uint8_t* destip, uint16_t destport)
{
    recvfrom_mod(sn, (uint8_t *)txCyclon, MAX_PACKET_LEN, destip, &destport);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);
    ++num_rcvd;
    if (num_rcvd == 500)
    {
        HAL_GPIO_WritePin(GPIOD, Blue_Led_Pin, GPIO_PIN_RESET);
    }
    if (num_rcvd == 1000)
    {
        num_rcvd = 0;
        HAL_GPIO_WritePin(GPIOD, Blue_Led_Pin, GPIO_PIN_SET);
    }
}

/**
 * @brief Default function to enable interrupt.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
void 	  wizchip_cris_enter(void)           {

}

/**
 * @brief Default function to disable interrupt.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
void 	  wizchip_cris_exit(void)          {

}

/**
 * @brief Default function to select chip.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
void 	wizchip_cs_select(void)            {
     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);
}

/**
 * @brief Default function to deselect chip.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
void 	wizchip_cs_deselect(void)          {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
}


/**
 * @brief Default function to read in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
uint8_t wizchip_spi_readbyte(void)        {
    uint8_t wb=0xFF;
    HAL_SPI_Receive(&hspi1, &wb, 1, 1000);
    return	wb;
}
/**
 * @brief Default function to write in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
void 	wizchip_spi_writebyte(uint8_t wb) {
        HAL_SPI_Transmit(&hspi1, &wb, 1, 1000);
}

/**
 * @brief Default function to burst read in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
void 	wizchip_spi_readburst(uint8_t* pBuf, uint16_t len) 	{
    HAL_SPI_Receive(&hspi1, pBuf, len , 1000);
}

/**
 * @brief Default function to burst write in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
void 	wizchip_spi_writeburst(uint8_t* pBuf, uint16_t len) {
    HAL_SPI_Transmit(&hspi1, pBuf, len , 1000);
}



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
#if(0)
    SPI1 - обмен в режиме мастер с W5500
            SYN -  PA5
            MISO - PA6
            MOSI - PA7

    W5500   SCLK - PA5
            SCS  - PD11 выбор
            INT  - PD8 вход INT
            MOSI - PA7
            RST  - PC9
            MISO - PA6

    SPI3 - обмен в режиме слейв с INTRON
            SYN -  PC10
            MISO - PC11
            MOSI - PC12

    SPI2 - обмен с ПЛИС
            SYN  -  PB10 - 99
            MISO -  PC2 - 101
            MOSI -  PC3 - 103
            CLK_EN - PC4 - 100
            RESET  - PC5 - 104
            TE_SEL - PC8 - 94 выбор тактирования (1 - внешнее)
            FPGA_EN- PA8 - 96 (1 - разрешение работы общее)
            CPU_INT- PB15 - 97  (Сигнал ПЛИС о готовности данных)
            F0 -           135
            C4 -           134

F0 подаем на вход таймера TIM1 (PE9) и по переднему входу захват и переход в обработчик

Между F0 - 125 мкс - 32 канала по 8 бит  - 256 бит
Контроллер воспринимает как 64 байта (в два раза чаще)
Считываем 16 байт (в реальности это 8 байт - 8 каналов) используется у нас только 4 или 5 каналов

#endif
//int AT24C_ReadBytes (uint16_t addr, uint8_t *buf, uint16_t bytes_count)
//{
//  uint16_t i;
//  //Disable Pos
//  LL_I2C_DisableBitPOS(I2C1);
//  LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
//  LL_I2C_GenerateStartCondition(I2C1);
//  while(!LL_I2C_IsActiveFlag_SB(I2C1)){};
//  //read state
//  (void) I2C1->SR1;
//  LL_I2C_TransmitData8(I2C1, SLAVE_OWN_ADDRESS | I2C_REQUEST_WRITE);
//  while(!LL_I2C_IsActiveFlag_ADDR(I2C1)){};
//  LL_I2C_ClearFlag_ADDR(I2C1);
//  LL_I2C_TransmitData8(I2C1, (uint8_t) (addr>>8));
//  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
//  LL_I2C_TransmitData8(I2C1, (uint8_t) addr);
//  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
//  LL_I2C_GenerateStartCondition(I2C1);
//  while(!LL_I2C_IsActiveFlag_SB(I2C1)){};
//  (void) I2C1->SR1;
//  LL_I2C_TransmitData8(I2C1, SLAVE_OWN_ADDRESS | I2C_REQUEST_READ);
//  while (!LL_I2C_IsActiveFlag_ADDR(I2C1)){};
//  LL_I2C_ClearFlag_ADDR(I2C1);
//  for(i=0;i<bytes_count;i++)
//  {
//    if(i<(bytes_count-1))
//    {
//      while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){};
//      buf[i] = LL_I2C_ReceiveData8(I2C1);
//    }
//    else
//    {
//      LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
//      LL_I2C_GenerateStopCondition(I2C1);
//      while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){};
//      buf[i] = LL_I2C_ReceiveData8(I2C1);
//    }
//  }
//  return i;
//}
//-------------------------------------------------------

//-------------------------------------------------------
//int AT24C_WriteBytes (uint16_t addr,uint8_t *buf, uint16_t bytes_count)
//{
//  uint16_t i;
//  //Disable Pos
//  LL_I2C_DisableBitPOS(I2C1);
//  LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
//  LL_I2C_GenerateStartCondition(I2C1);
//  while(!LL_I2C_IsActiveFlag_SB(I2C1)){};
//  //read state
//  (void) I2C1->SR1;
//  LL_I2C_TransmitData8(I2C1, SLAVE_OWN_ADDRESS | I2C_REQUEST_WRITE);
//  while(!LL_I2C_IsActiveFlag_ADDR(I2C1)){};
//  LL_I2C_ClearFlag_ADDR(I2C1);
//  LL_I2C_TransmitData8(I2C1, (uint8_t) (addr>>8));
//  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
//  LL_I2C_TransmitData8(I2C1, (uint8_t) addr);
//  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
//  for(i=0;i<bytes_count;i++)
//  {
//    LL_I2C_TransmitData8(I2C1, buf[i]);
//    while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
//  }
//  LL_I2C_GenerateStopCondition(I2C1);
//  return i;
//}
