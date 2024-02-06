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
#include "spi_eeprom.h"

extern uint8_t SEGGER;
extern uint8_t destip[4];
extern uint8_t ABONENT_or_BASE;
extern UART_HandleTypeDef huart6;
extern IWDG_HandleTypeDef hiwdg;
extern uint32_t num_send;
extern uint32_t num_rcvd;
uint32_t numSendDiagnosticPacket = 0;
uint8_t CCMRAMDATA testDataFromBase[MAX_PACKET_LEN];
uint8_t CCMRAMDATA testDataFromAbon[MAX_PACKET_LEN];
uint8_t CCMRAMDATA testDataToAbon[MAX_PACKET_LEN];
char timeSend[20];

uint8_t commandfromBaseToAbonentReboot[MAX_PACKET_LEN]= {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
                                                   0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
                                                   0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88};
uint8_t commandfromBaseToAbonentNetDiagnostic[MAX_PACKET_LEN]= {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
                                                          0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
                                                          0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77};
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
uint8_t test5[MAX_PACKET_LEN] = {0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
                                0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
                                0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82};
uint8_t test6[MAX_PACKET_LEN] = {0xFF, 0x14, 0x00, 0x00, 0xFF, 0x14, 0x00, 0x00, 0xFF, 0x14, 0x00, 0x00, 0xFF, 0x14, 0x00, 0x00,
                                0xFF, 0x14, 0x00, 0x00, 0xFF, 0x14, 0x00, 0x00, 0xFF, 0x14, 0x00, 0x00, 0xFF, 0x14, 0x00, 0x00,
                                0xFF, 0x14, 0x00, 0x00, 0xFF, 0x14, 0x00, 0x00, 0xFF, 0x14, 0x00, 0x00, 0xFF, 0x14, 0x00, 0x00};
uint8_t test7[MAX_PACKET_LEN] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0xff, 0xff, 0xff, 0xff, 0xaa, 0xaa, 0xaa, 0xaa,
                                0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0xff, 0xff, 0xff, 0xff, 0xaa, 0xaa, 0xaa, 0xaa,
                                0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0xff, 0xff, 0xff, 0xff, 0xaa, 0xaa, 0xaa, 0xaa};
uint8_t zeroStr[MAX_PACKET_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void UART_Printf(const char* fmt, ...) {
    char buff[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    HAL_UART_Transmit_DMA(&huart6, (uint8_t*)buff, strlen(buff));
    va_end(args);
}
void Printf(const char* fmt, ...) {
    char buff[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    HAL_UART_Transmit(&huart6, (uint8_t*)buff, strlen(buff) ,HAL_MAX_DELAY );
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
uint8_t checkCommands(uint8_t dataToDx[MAX_PACKET_LEN]){
    if ( strncmp ((const char*)dataToDx, (const char*)commandfromBaseToAbonentReboot, MAX_PACKET_LEN) == 0){
        return REBOOT;
    }
    if ( strncmp ((const char*)dataToDx, (const char*)commandfromBaseToAbonentNetDiagnostic, MAX_PACKET_LEN) == 0){
        return NET_DIAGNOSTIC;
    }
    return NO_COMMAND;
}
void printWiznetReg(){
    printf("wiznet registers:\r\n"
           "Mode Register: %X\r\n"
           "Interrupt Register: %X\r\n"
           "PHY Status Register: %X\r\n"
           ,WIZCHIP_READ(MR)
           ,WIZCHIP_READ(IR)
           ,WIZCHIP_READ(PHYCFGR)
           );
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
void print_2_Channel_control(uint8_t data[MAX_PACKET_LEN / 4]) {
    SEGGER_RTT_SetTerminal(2);
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
void create_2_channelDataForControl(uint8_t dataFromBase[MAX_PACKET_LEN], uint8_t dataForControl[MAX_PACKET_LEN / 4]){
    for (int i = 0; i < 12; ++i) {
        strncpy((char *)dataForControl + i , (const char*)dataFromBase + 1 + i * 4 , 1);
    }
}
uint8_t check_2_Channel(uint8_t data[MAX_PACKET_LEN / 4], uint8_t trueData[MAX_PACKET_LEN / 4]) {
    if ( strncmp ((const char*)data, (const char*)trueData, 12) == 0){
        return 0;
    }
    return 1;
}
void commandFromWebNetDiagnostic() {
    if (ABONENT_or_BASE == BASE){
        printf(" **********   Start Net Diagnostis BASE  **********\r\n");

        uint8_t netDiagnostic[1];
        netDiagnostic[0] = 0x88;
        EEPROM_SPI_WriteBuffer(netDiagnostic, netDiagnosticFlag, 1);
        printf("netDiagnosticFlag set ON\r\n");
        reboot();
    }
    else
    {
        printf(" **********   SELECT BASE !!!   **********\r\n");
    }
}
uint8_t checkNetDiagnosticMode()
{
    // считываем значение по адресу netDiagnosticFlag
    uint8_t netDiagnostic[1];
    EEPROM_SPI_ReadBuffer(netDiagnostic, netDiagnosticFlag, 1);
    if (netDiagnostic[0] == 0x88) {
       return netDiagnosticON;
    }
    else {
//        printf("netDiagnosticFlag OFF\r\n");
        return netDiagnosticOFF;
    }
}
uint8_t checkPingMode()
{
    // считываем значение по адресу pingFlag
    uint8_t ping[1];
    EEPROM_SPI_ReadBuffer(ping, pingFlag, 1);
    if (ping[0] == 0x88) {
        return pingON;
    }
    else {
        return pingOFF;
    }
}
void netDiagnosticBase(){
    printf("Start net diagnostic Base\r\n");
    uint16_t destport = LOCAL_PORT_UDP;
    uint32_t startDiagnosticTime = HAL_GetTick();
    uint32_t currentDiagnosticTime;
    while(1){
        currentDiagnosticTime = HAL_GetTick();
        if ((startDiagnosticTime + 120000 < currentDiagnosticTime)){ //Длительность сессии 2 мин
            printf("***** Long Diagnostic session - Reboot *****\r\n");
            reboot();
        }
        else {//Минута еще не прошла - работает тест сети
            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET) { // CPU_INT Жду пока плис поднимет флаг
                ++numSendDiagnosticPacket;
                if (numSendDiagnosticPacket == NUM_DIAGNOSTIC_UDP_PACKETS){
                    printf("send %d packets, end test\r\n", NUM_DIAGNOSTIC_UDP_PACKETS);
                    //Результаты теста

                    sendto(UDP_SOCKET, (uint8_t *)commandfromBaseToAbonentReboot, MAX_PACKET_LEN, destip, LOCAL_PORT_UDP);
                    reboot();
                }
                prepeareDataToAbonent(testDataToAbon, numSendDiagnosticPacket, currentDiagnosticTime);

                sendto(UDP_SOCKET, (uint8_t *)testDataToAbon, MAX_PACKET_LEN, destip, destport);
                recvfrom(UDP_SOCKET, (uint8_t *)testDataFromAbon, MAX_PACKET_LEN, destip, &destport);
                currentDiagnosticTime = HAL_GetTick();
                analiseDataFromAbonent(testDataFromAbon, currentDiagnosticTime);

//                if ((numSendDiagnosticPacket == 10) || (numSendDiagnosticPacket % 200 == 0)){
//                    printTestNetData(testDataFromAbon);
//                }


                indicateSend(20,40);
                indicateReceive(20,40);
            }
        }
    } //while
}
void netDiagnosticAbon(){
    printf("Start net diagnostic Abonet\r\n");
    uint16_t destport = LOCAL_PORT_UDP;
    uint32_t startDiagnosticTime = HAL_GetTick();
    uint32_t currentDiagnosticTime;
    uint32_t numReceivedDiagnosticPacket = 0;
    while(1){
        currentDiagnosticTime = HAL_GetTick();
        if ((startDiagnosticTime + 120000 < currentDiagnosticTime)){//Длительность сессии 2 мин
            printf("***** Long Diagnostic session - Reboot *****\r\n");
            reboot();
        }
        else {//Минута еще не прошла - работает тест сети
            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET) {// CPU_INT Жду пока плис поднимет флаг
                recvfrom(UDP_SOCKET, (uint8_t *)testDataFromBase, MAX_PACKET_LEN, destip, &destport);
                if (REBOOT == checkCommands(testDataFromBase)){
                    red_blink  red_blink  red_blink
                        printf("Received command Reboot from Base\r\n");
                    reboot();
                }
                prepeareAnswerToBase(testDataFromBase, currentDiagnosticTime);
                sendto(UDP_SOCKET, (uint8_t *)testDataFromBase, MAX_PACKET_LEN, destip, destport);
                ++numReceivedDiagnosticPacket;
                if ((numReceivedDiagnosticPacket == 1) || (numReceivedDiagnosticPacket % 500 == 0)){
                    printTestNetData(testDataFromBase);
                }


                indicateSend(20,40);
                indicateReceive(20,40);
            }
        }
    }//while
}
void indicateSend(uint16_t numON, uint16_t numOFF){
    ++num_send;
    if (num_send == numON){
        HAL_GPIO_WritePin(GPIOD, Green_Led_Pin, GPIO_PIN_RESET);
    }
    if (num_send == numOFF){
        num_send = 0;
        HAL_GPIO_WritePin(GPIOD, Green_Led_Pin, GPIO_PIN_SET);
    }
}
void indicateReceive(uint16_t numON, uint16_t numOFF){
    ++num_rcvd;
    if (num_rcvd == numON){
        HAL_GPIO_WritePin(GPIOD, Red_Led_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOD, Blue_Led_Pin, GPIO_PIN_RESET);
    }
    if (num_rcvd == numOFF){
        num_rcvd = 0;
        HAL_GPIO_WritePin(GPIOD, Blue_Led_Pin, GPIO_PIN_SET);
        HAL_IWDG_Refresh(&hiwdg);
    }
}
void prepeareDataToAbonent(uint8_t * dataToAbon, uint32_t numPacket, uint32_t currTime){
//В пакет добавляем номер пакета и метку времени базы
    char tmp[48];
    sprintf((char*)tmp,"...%d***%d___", numPacket, currTime);
    strcpy((char*)dataToAbon, tmp);
}
void prepeareAnswerToBase(uint8_t * dataFromBase, uint32_t currTime){
    //В полученный пакет добавляем метку времени абонента
    char tmp[48];
    uint32_t delta = 3300;// чтобы выровнять время базы и абонента
    sprintf((char*)tmp,"%d:::", currTime + delta);

    char substring[4] = "___";
    char *substring_ptr = strstr((char*)dataFromBase, substring);
    strcpy((char*)(substring_ptr + 3), tmp);
}
void printTestNetData(uint8_t data[MAX_PACKET_LEN]) {
        UART_Printf("%.48s\r\n", data );
}
void analiseDataFromAbonent(uint8_t * dataFromAbon, uint32_t currTime){
//В полученный пакет добавляем метку времени базы
    char tmp[48];
    sprintf((char*)tmp,"%d", currTime);

    char substring[4] = ":::";
    char *substring_ptr = strstr((char*)dataFromAbon, substring);
    strcpy((char*)(substring_ptr + 3), tmp);

// Указатель на 1 символ времени отправки
    char substring2[4] = "***";
    char *substring_ptr2 = strstr((char*)dataFromAbon, substring2) + 3;
// Указатель на 1 символ _
    char substring3[4] = "___";
    char *substring_ptr3 = strstr((char*)dataFromAbon, substring3);
// Длина строки с временем отправки
    uint8_t len = substring_ptr3 - substring_ptr2;

    strncpy(timeSend, substring_ptr2, len);
    timeSend[len] = 0x00;
    uint32_t time_send = atol(timeSend);
    if ((numSendDiagnosticPacket % 500 == 0)){
        UART_Printf("timeSend: %d timeReceive: %d\r\n",time_send, HAL_GetTick());
    }

}
void commandFromWebPing(){
    printf("Received command PING from WEB\r\n");
    uint8_t ping[1];
    ping[0] = 0x88;
    EEPROM_SPI_WriteBuffer(ping, pingFlag, 1);
    printf("pingFlag set ON\r\n");
    reboot();
}
void pingCheck(){
    if (pingON == checkPingMode()){
        printf("pingON\r\n");

        //Сбрасываю флаг диагностики
        uint8_t ping[1];
        ping[0] = 0xFF;
        EEPROM_SPI_WriteBuffer(ping, pingFlag, 1);
        printf("pingFlag set OFF\r\n");

        workInPingMode();
        reboot();
    }
}
void workInPingMode(){
    printf("Work in PING mode\r\n");

    socket(PING_SOCKET, Sn_MR_IPRAW, LOCAL_PORT_UDP, 0);
    while(getSn_SR(PING_SOCKET)!=SOCK_IPRAW);
    printf("open PING socket %d\r\n", PING_SOCKET);






}
