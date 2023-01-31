#include "net.h"
//-----------------------------------------------
struct udp_pcb *upcb;
char str1[65];
uint8_t allByte[MAX_PACKET_LEN];
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
    IP4_ADDR(&DestIPaddr, 192, 168, 1, 108);
  	upcb->local_port = 1555;
  	err= udp_connect(upcb, &DestIPaddr, 1556);
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
    pbuf_take(p, (void *) allByte, MAX_PACKET_LEN);
    udp_send(upcb, p);
    pbuf_free(p);
  }
}
//-----------------------------------------------
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  strncpy(str1,p->payload,p->len);
  str1[p->len]=0;
  pbuf_free(p);
  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
}
//-----------------------------------------------
void packetSendUDP()
{
    udp_client_send();
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
}
//--------------------------------------------------
