#include "main.h"
#include "ping.h"
#include "spi_eeprom.h"
#include "w5500.h"
#include "socket.h"

#define Sn_PROTO(ch) (0x001408 + (ch << 5))
#define PING_BIND_PORT 3000

PINGMSGR PingRequest = {0};
PINGMSGR PingReply = {0};
static uint16_t ping_RandomID = 0x1234;
static uint16_t ping_RandomSeqNum = 0x4321;
uint8_t ping_reply_received = 0;
uint8_t ping_req = 0;
uint8_t ping_rep = 0;
uint8_t ping_cnt = 0;
uint8_t ping_rep_buf[150] = {0};

// ping state machine
#define PING_STA_FREE 0
#define PING_STA_OPEN 1
#define PING_STA_SEND 2
#define PING_STA_WAIT 3
#define PING_STA_CLOSE 4

uint8_t ping_sta = PING_STA_FREE;

// Currenual number of devices of ping
uint8_t ping_device_serial = 0;

// ping target
extern uint8_t destip[4];

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
void workInPingMode() {
    printf("Work in PING mode\r\n");
    PINGMSGR PingRequest;
    PingRequest.Type  = PING_REQUEST; // Ping-Request
    PingRequest.Code = CODE_ZERO;
//    PingRequest.ID = htons(RandomID++); // set ping-request's ID to random integer value
//    PingRequest.SeqNum = htons(RandomSeqNum++);
    PingRequest.CheckSum = 0;
//    PingRequest.CheckSum = htons(checksum((uint8_t *)&PingRequest,sizeof(PingRequest)));















//    socket(PING_SOCKET, Sn_MR_IPRAW, LOCAL_PORT_UDP, 0);
//    while(getSn_SR(PING_SOCKET)!=SOCK_IPRAW);
//    printf("open PING socket %d\r\n", PING_SOCKET);
//    if(sendto(PING_SOCKET,(uint8_t *)&PingRequest,sizeof(PingRequest), destip, LOCAL_PORT_UDP)==0){
//        printf( "\r\n Fail to send ping-reply packet \r\n");}
//    uint16_t rlen = 0;
//    int32_t len = 0;
//    uint16_t destPort = LOCAL_PORT_UDP;
//    while(rlen == 0){
//        if ( (rlen = getSn_RX_RSR(PING_SOCKET) ) > 0){
//            /* receive data from a destination */
//            len = recvfrom(PING_SOCKET, (uint8_t *)pingDataBuf, rlen, destip, &destPort);
//            //Выводим на печать
//            printf("ping: "
//                   "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
//                   "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
//                   "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
//                   "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
//                   "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
//                   "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
//                   "\r\n",
//                   pingDataBuf[0], pingDataBuf[1], pingDataBuf[2], pingDataBuf[3], pingDataBuf[4], pingDataBuf[5], pingDataBuf[6], pingDataBuf[7],
//                   pingDataBuf[8],pingDataBuf[9], pingDataBuf[10], pingDataBuf[11], pingDataBuf[12], pingDataBuf[13], pingDataBuf[14], pingDataBuf[15],
//                   pingDataBuf[16], pingDataBuf[17], pingDataBuf[18], pingDataBuf[19], pingDataBuf[20], pingDataBuf[21], pingDataBuf[22], pingDataBuf[23],
//                   pingDataBuf[24], pingDataBuf[25], pingDataBuf[26], pingDataBuf[27], pingDataBuf[28], pingDataBuf[29], pingDataBuf[30], pingDataBuf[31],
//                   pingDataBuf[32], pingDataBuf[33], pingDataBuf[34], pingDataBuf[35], pingDataBuf[36], pingDataBuf[37], pingDataBuf[38], pingDataBuf[39],
//                   pingDataBuf[40], pingDataBuf[41], pingDataBuf[42], pingDataBuf[43], pingDataBuf[44], pingDataBuf[45], pingDataBuf[46], pingDataBuf[47]
//                   );
//        }
//    }

}

// ping service processing, about 50ms performed once
void Ethernet_ping_service_deal(uint8_t sn)
{
    static uint16_t ping_cycle = 0;
    uint16_t rlen;
    uint8_t i, res = 0;
        // Current channel status
    uint8_t sn_sr = 0;

    switch (ping_sta)
    {
        case PING_STA_FREE: // idle state
        {
            ping_cycle++;
            if (ping_cycle > 50 * 20)
            {
                ping_cycle = 0;
                ping_sta = PING_STA_OPEN;
            }
            break;
        }
        case PING_STA_OPEN: // Open the channel
        {
            sn_sr = getSn_SR(sn);
            if (sn_sr == SOCK_CLOSED)
            {
                close(sn);
                /* Create Socket */
                WIZCHIP_WRITE(Sn_PROTO(sn), IPPROTO_ICMP); /* Set ICMP Protocol */
                    if (socket (sn, Sn_MR_IPRAW, PING_BIND_PORT, 0) != 0) /* Determine if the IP RAW mode socket is turned on */
                {
                }
            }
            else if (sn_sr == SOCK_IPRAW)
            {
                printf("Ping socket open\r\n");
                ping_device_serial = 0;
                ping_req = 0;
                ping_rep = 0;
                ping_sta = PING_STA_SEND;
            }
            break;
        }
        Case ping_sta_send: // Send data
        {
            // After the query is 1 round, disconnect
            if (ping_device_serial > 15)
            {
                ping_sta = PING_STA_CLOSE;
                break;
            }
            // Judgment if the query is completed
            for (i = ping_device_serial; i < 16; i++)
            {
                if (System_Para.ethernet_ping_ip[i][0] != 0)
                {
                    ping_device_serial = i;
                    if (ping_req == 0)
                    {
                        ping_rep = 0;
                        memcpy(ping_ip_dest, System_Para.ethernet_ping_ip[i], 4);
                        ol_print(DEBUG_CHN, 0, "Ping:%d.%d.%d.%d\r\n", (ping_ip_dest[0]), (ping_ip_dest[1]), (ping_ip_dest[2]), (ping_ip_dest[3]));
                    }
                    ping_req++;
                    Ping_Request (Sn, Ping_IP_DEST); / * Send PING Request * /
                        ping_sta = PING_STA_WAIT;
                    ping_cnt = 0;
                    break;
                }
            }
            // Query
            if(i == 16)
            {
                ping_sta = PING_STA_CLOSE;
                break;
            }
        }
        Case ping_sta_wait: // Waiting results
        {
            if ((rlen = getSn_RX_RSR(sn)) > 0)
            {
                Rlen = ping_reply (SN, Ping_IP_DEST, RLEN); / * Get reply information * /
                    ping_rep++;
                if (ping_reply_received)
                {
                    ol_print(DEBUG_CHN, 0, "rep%d t=%d,len=%d\n",ping_req,ping_cnt, rlen);
                    ping_sta = PING_STA_SEND;
                    if(ping_req == 4)
                    {
                        ping_req = 5;
                    }
                }
            }
            IF ((ping_cnt> 50)) // More 4 times, end query
            {
                ol_print(DEBUG_CHN, 0, "rep time out\r\n\r\n");
                ping_cnt = 0;
                ping_sta = PING_STA_SEND;
                if(ping_req == 4)
                {
                    ping_req = 5;
                }
            }
            else
            {
                ping_cnt++;
            }
            // ping after 4 times, ping Next device
            if (ping_req == 5)
            {
                ol_print(DEBUG_CHN, 0, "ping %d.%d.%d.%d, s=4, r=%d, l=%d\n",
                         ping_ip_dest[0], ping_ip_dest[1], ping_ip_dest[2], ping_ip_dest[3], ping_rep, 4 - ping_rep);
                if (ping_rep == 0)
                {
                    System_Sta.ping_list_sta &= ~(0x0001 << ping_device_serial);
                }
                else
                {
                    System_Sta.ping_list_sta |= (0x0001 << ping_device_serial);
                }
                ping_rep = 0;
                ping_req = 0;
                ping_device_serial++;
            }
            break;
        }
        Case ping_sta_close: // Turn off the channel
        {
            ol_print(DEBUG_CHN, 0, "Ping socket close\n");
            w5500_close(sn);
            ping_sta = PING_STA_FREE;
            break;
        }
    }
}

