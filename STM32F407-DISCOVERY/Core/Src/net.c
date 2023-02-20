#include "net.h"
//-----------------------------------------------
struct udp_pcb *upcb;
extern SPI_HandleTypeDef hspi1;
extern uint8_t toSend[MAX_PACKET_LEN];
uint8_t testSend[MAX_PACKET_LEN] = {0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0xff, 0xff, 0xff, 0xff};
extern uint8_t toRecive[MAX_PACKET_LEN];
extern UART_HandleTypeDef huart6;
//-----------------------------------------------
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
//-----------------------------------------------
void udp_client_connect(void)
{
  ip_addr_t DestIPaddr;
  err_t err;
  upcb = udp_new();
  if (upcb!=NULL)
  {
    IP4_ADDR(&DestIPaddr, 192, 168, 1, 101);
  	upcb->local_port = 1555;
    err= udp_connect(upcb, &DestIPaddr, 1555);
  	if (err == ERR_OK)
  	{
  	  udp_recv(upcb, udp_receive_callback, NULL);
    }
  }
}
//-----------------------------------------------
void udp_client_send()
{
    struct pbuf *p;
    p = pbuf_alloc(PBUF_TRANSPORT, MAX_PACKET_LEN, PBUF_POOL);
    if (p != NULL)
    {
//        pbuf_take(p, (void *) toSend, MAX_PACKET_LEN);
        pbuf_take(p, (void *) testSend, MAX_PACKET_LEN);
        udp_send(upcb, p);
        pbuf_free(p);
    }
}
//-----------------------------------------------
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
//    HAL_SPI_Transmit(&hspi1, (uint8_t*)p->payload, p->len, 0x1000);
   strncpy(toRecive,p->payload,p->len);
//    toRecive[p->len]=0;
   pbuf_free(p);
   HAL_GPIO_TogglePin(GPIOD, Green_Led_Pin);
   HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
}
//-----------------------------------------------
void packetSendUDP()
{
    HAL_GPIO_TogglePin(GPIOD, Blue_Led_Pin);
    udp_client_send();
}
//--------------------------------------------------
