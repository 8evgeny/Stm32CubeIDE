#include "main.h"
#include "spi_eeprom.h"
#include "w5500.h"
#include "socket.h"

#define BUF_LEN 32
#define PING_REQUEST 8
#define PING_REPLY 0
#define CODE_ZERO 0
typedef struct pingmsg
{
    uint8_t Type; // 0 - Ping Reply, 8 - Ping Request
    uint8_t Code; // Always 0
    int16_t CheckSum; // Check sum
    int16_t ID; // Identification
    int16_t SeqNum; // Sequence Number
    // Ping Data : 1452 = IP RAW MTU - sizeof(Type + Code + CheckSum + ID + SeqNum)
    int8_t Data[BUF_LEN];
} PINGMSGR;

extern uint8_t destip[4];
uint8_t PingRequest[MAX_PACKET_LEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                                       0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                                       0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
uint8_t pingDataBuf[MAX_PACKET_LEN];



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
    if(sendto(PING_SOCKET,(uint8_t *)&PingRequest,sizeof(PingRequest), destip, LOCAL_PORT_UDP)==0){
        printf( "\r\n Fail to send ping-reply packet \r\n");}
    uint16_t rlen = 0;
    int32_t len = 0;
    uint16_t destPort = LOCAL_PORT_UDP;
    while(rlen == 0){
        if ( (rlen = getSn_RX_RSR(PING_SOCKET) ) > 0){
            /* receive data from a destination */
            len = recvfrom(PING_SOCKET, (uint8_t *)pingDataBuf, rlen, destip, &destPort);
            //Выводим на печать
            printf("ping: "
                   "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
                   "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
                   "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
                   "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
                   "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
                   "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
                   "\r\n",
                   pingDataBuf[0], pingDataBuf[1], pingDataBuf[2], pingDataBuf[3], pingDataBuf[4], pingDataBuf[5], pingDataBuf[6], pingDataBuf[7],
                   pingDataBuf[8],pingDataBuf[9], pingDataBuf[10], pingDataBuf[11], pingDataBuf[12], pingDataBuf[13], pingDataBuf[14], pingDataBuf[15],
                   pingDataBuf[16], pingDataBuf[17], pingDataBuf[18], pingDataBuf[19], pingDataBuf[20], pingDataBuf[21], pingDataBuf[22], pingDataBuf[23],
                   pingDataBuf[24], pingDataBuf[25], pingDataBuf[26], pingDataBuf[27], pingDataBuf[28], pingDataBuf[29], pingDataBuf[30], pingDataBuf[31],
                   pingDataBuf[32], pingDataBuf[33], pingDataBuf[34], pingDataBuf[35], pingDataBuf[36], pingDataBuf[37], pingDataBuf[38], pingDataBuf[39],
                   pingDataBuf[40], pingDataBuf[41], pingDataBuf[42], pingDataBuf[43], pingDataBuf[44], pingDataBuf[45], pingDataBuf[46], pingDataBuf[47]
                   );
        }
    }

}
