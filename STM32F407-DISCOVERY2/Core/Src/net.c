#include "net.h"
//-----------------------------------------------
struct udp_pcb *upcbSend;
struct udp_pcb *upcbReceive;
extern uint8_t sendOk;
extern SPI_HandleTypeDef hspi1;
extern uint8_t txBuf[MAX_PACKET_LEN +1];
uint8_t testSend[MAX_PACKET_LEN] = {0x55, 0x55, 0xff, 0xff, 0xff,
//                                   0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0xff, 0xff, 0xff, 0xff
                                   };
extern uint8_t rxBuf[MAX_PACKET_LEN + 1];
extern UART_HandleTypeDef huart6;
//-----------------------------------------------
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
//-----------------------------------------------
void udp_client_connect(void)
{
  ip_addr_t DestIPaddr;
  err_t err;
  upcbReceive = udp_new();
  upcbSend = udp_new();
  if (upcbReceive!=NULL)
  {
    IP4_ADDR(&DestIPaddr, 192, 168, 1, 101);
    upcbReceive->local_port = 1555;
    upcbSend->local_port = 1555;
    udp_connect(upcbSend, &DestIPaddr, 1555);
    err= udp_connect(upcbReceive, &DestIPaddr, 1555);
  	if (err == ERR_OK)
  	{
      udp_recv(upcbReceive, udp_receive_callback, NULL);
    }
  }
}
//-----------------------------------------------
void udp_client_send()
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
    struct pbuf *p;
    p = pbuf_alloc(PBUF_TRANSPORT, MAX_PACKET_LEN, PBUF_POOL);
    if (p != NULL)
    {
        pbuf_take(p, (void *) txBuf, MAX_PACKET_LEN);
        udp_send(upcbSend, p);
        pbuf_free(p);
    }
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);
}
//-----------------------------------------------
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
//    HAL_SPI_Transmit(&hspi1, (uint8_t*)p->payload, p->len, 0x1000);
    strncpy(txBuf,p->payload,p->len);
//    reciveBuf[p->len]=0;
    pbuf_free(p);
    HAL_GPIO_TogglePin(GPIOD, Green_Led_Pin);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
}
//-----------------------------------------------
void packetSendUDP()
{
    HAL_GPIO_TogglePin(GPIOD, Blue_Led_Pin);
    udp_client_send();
    sendOk = 1;
}
//--------------------------------------------------
