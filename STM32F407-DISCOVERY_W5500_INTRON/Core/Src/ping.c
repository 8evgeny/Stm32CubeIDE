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

    Ethernet_ping_service_deal(PING_SOCKET);
}

// ping service processing, about 50ms performed once
void Ethernet_ping_service_deal(uint8_t sn)
{
    static uint16_t ping_cycle = 0;
    uint16_t rlen;
    uint8_t i = 0;
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
        case PING_STA_SEND: // Send data
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
                ping_device_serial = i;
                if (ping_req == 0)
                {
                    ping_rep = 0;
                    printf( "Ping:%d.%d.%d.%d\r\n", destip[0], destip[1], destip[2], destip[3]);
                }
                ping_req++;
                ping_request (sn, destip); /* Send PING Request */
                ping_sta = PING_STA_WAIT;
                ping_cnt = 0;
                break;
            }
            // Query
            if(i == 16)
            {
                ping_sta = PING_STA_CLOSE;
                break;
            }
        }
        case PING_STA_WAIT: // Waiting results
        {
            if ((rlen = getSn_RX_RSR(sn)) > 0)
            {
                rlen = ping_reply (sn, destip, rlen); /* Get reply information */
                    ping_rep++;
                if (ping_reply_received)
                {
                    printf("rep%d t=%d,len=%d\r\n",ping_req, ping_cnt, rlen);
                    ping_sta = PING_STA_SEND;
                    if(ping_req == 4)
                    {
                        ping_req = 5;
                    }
                }
            }
            if ((ping_cnt> 50)) // More 4 times, end query
            {
                printf("rep time out\r\n\r\n");
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
                printf("ping %d.%d.%d.%d, s=4, r=%d, l=%d\r\n",
                         destip[0], destip[1], destip[2], destip[3], ping_rep, 4 - ping_rep);
                ping_rep = 0;
                ping_req = 0;
                ping_device_serial++;
            }
            break;
        }
        case PING_STA_CLOSE: // Turn off the channel
        {
            printf("Ping socket close\r\n");
            close(sn);
            ping_sta = PING_STA_FREE;
            break;
        }
    }
}

uint8_t ping_count(uint8_t sn, uint16_t pCount, uint8_t *addr)
{
    uint16_t rlen, cnt, i;

    ping_reply_received = 0;
    ping_req = 0;
    ping_rep = 0;
    printf( "Ping:%d.%d.%d.%d\r\n", (addr[0]), (addr[1]), (addr[2]), (addr[3]));

    for (i = 0; i <pCount + 1; i ++) /* loop PCOUNT */
    {
        switch (getSn_SR (sn)) /* Get the Socket status */
        {
            case SOCK_CLOSED: /* Socket Off Status */
            {
                close(sn);
                /* Create Socket */
                WIZCHIP_WRITE (Sn_PROTO (sn), IPPROTO_ICMP); /* Set ICMP Protocol */
                    if (socket(sn, Sn_MR_IPRAW, PING_BIND_PORT, 0) != 0) /* Determine if the IP RAW mode socket is turned on */
                {
                }
                /* Check socket register */
                while (getSn_SR(sn) != SOCK_IPRAW)
                {
                    HAL_Delay(50);
                };
                break;
            }
            case SOCK_IPRAW: /* IP RAW Mode */
            {
                cnt = 0;
                ping_request (sn, addr); /* Send PING Request */
                ping_req++;
                while (1)
                {
                    if ((rlen = getSn_RX_RSR(sn)) > 0)
                    {
                        rlen = ping_reply (sn, addr, rlen); /* Get reply information */
                        ping_rep++;
                        if (ping_reply_received)
                        {
                            printf("rep t=%d,len=%d\r\n", cnt, rlen);
                            break;
                        }
                    }
                    if ((cnt > 50))
                    {
                        printf( "rep time out\r\n\r\n");
                        cnt = 0;
                        break;
                    }
                    else
                    {
                        cnt++;
                        HAL_Delay(10);
                    }
                }
                break;
            }
        default:
            break;
        }
        if (ping_req >= pCount)
        {
            close(sn);
        }
    }
    return ping_rep;
}

uint8_t ping_request(uint8_t sn, uint8_t *addr)
{
//    uint8_t *buffer;
    uint16_t i/*, temp_len*/ = 0;
    ping_reply_received = 0; /* ping Reply Initialization Sign */
        PingRequest.Type = PING_REQUEST;				 /*Ping-Request*/
    PingRequest.Code = CODE_ZERO; /* Always '0' */
        PingRequest.ID = htons (ping_RandomID ++); /* Setting the ping response ID is a random integer variable */
        PingRequest.SeqNum = htons (ping_RandomSeqNum ++); /* Set the serial number of the ping response is a random shaping variable */
        for (i = 0; i < PING_BUF_LEN; i++)
    {
        PingRequest.Data [i] = (i)% 8; /* ping the corresponding number in '0' ~ '8' */
    }
    PingRequest.CheckSum = 0;
    /* Calculated response */
        PingRequest.CheckSum = htons(checksum((uint8_t *)&PingRequest, sizeof(PingRequest)));

    /* Send ping response to destination */
        if (sendto(sn, (uint8_t *)&PingRequest, sizeof(PingRequest), addr, PING_BIND_PORT) == 0)
    {
            printf( "Fail to send ping-reply packet\r\n");
    }
    else
    {
        printf("ping send\r\n");
    }
    return 0;
}

uint8_t ping_reply(uint8_t sn, uint8_t *addr, uint16_t rlen)
{
    uint16_t tmp_checksum;
    uint16_t len;
    uint16_t i;

    uint16_t port = PING_BIND_PORT;
    PINGMSGR PingReply;

    memset(ping_rep_buf, 0, sizeof(ping_rep_buf));
    len = recvfrom( sn, ping_rep_buf, rlen, addr, &port); /* Receive data from destination */

        if (ping_rep_buf[0] == PING_REPLY)
    {
        PingReply.Type = ping_rep_buf[0];
        PingReply.Code = ping_rep_buf[1];
        PingReply.CheckSum = (ping_rep_buf[3] << 8) + ping_rep_buf[2];
        PingReply.ID = (ping_rep_buf[5] << 8) + ping_rep_buf[4];
        PingReply.SeqNum = (ping_rep_buf[7] << 8) + ping_rep_buf[6];

        for (i = 0; i < len - 8; i++)
        {
            PingReply.Data[i] = ping_rep_buf[8 + i];
        }
        tmp_checksum = ~ checksum (ping_rep_buf, len); /* Check the number of ping replies */
            if (tmp_checksum != 0xffff)
        {
            printf( "tmp_checksum = %x\r\n", tmp_checksum);
        }
        else
        {
            printf( "Reply from %3d.%3d.%3d.%3d ID=%x Byte=%d\r\n\r\n", (addr[0]), (addr[1]), (addr[2]), (addr[3]), htons(PingReply.ID), (rlen + 6));
            ping_reply_received = 1; /* Setting the ping reply sign as 1 */
        }
    }
    else if (ping_rep_buf[0] == PING_REQUEST)
    {
        PingReply.Code = ping_rep_buf[1];
        PingReply.Type = ping_rep_buf[2];
        PingReply.CheckSum = (ping_rep_buf[3] << 8) + ping_rep_buf[2];
        PingReply.ID = (ping_rep_buf[5] << 8) + ping_rep_buf[4];
        PingReply.SeqNum = (ping_rep_buf[7] << 8) + ping_rep_buf[6];
        for (i = 0; i < len - 8; i++)
        {
            PingReply.Data[i] = ping_rep_buf[8 + i];
        }
        tmp_checksum = PingReply.CheckSum; /* Check ping reply number */
            PingReply.CheckSum = 0;
        if (tmp_checksum != PingReply.CheckSum)
        {
                printf(" \n CheckSum is in correct %x shold be %x \n", (tmp_checksum), htons(PingReply.CheckSum));
        }
        else
        {
        }
        printf(" Request from %d.%d.%d.%d  ID:%x SeqNum:%x  :data size %d bytes\r\n",
                 (addr[0]), (addr[1]), (addr[2]), (addr[3]), (PingReply.ID), (PingReply.SeqNum), (rlen + 6));
        ping_reply_received = 1; /* Setting the ping reply sign as 1 */
    }
    else
    {
        printf("Unkonwn msg. \n");
    }
    return 0;
}

uint32_t htonl(uint32_t net)
{
    return __builtin_bswap32(net);
}

uint16_t htons(uint16_t net)
{
    return __builtin_bswap16(net);
}

uint64_t htonll(uint64_t net)
{
    return __builtin_bswap64(net);
}
