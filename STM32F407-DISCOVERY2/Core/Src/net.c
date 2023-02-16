#include "net.h"
//-----------------------------------------------
struct udp_pcb *upcb;
extern SPI_HandleTypeDef hspi1;
char str1[20];
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
    IP4_ADDR(&DestIPaddr, 192, 168, 1, 100);
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
    pbuf_take(p, (void *) allByte, MAX_PACKET_LEN);
    udp_send(upcb, p);
    pbuf_free(p);
  }
}
//-----------------------------------------------
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, (uint8_t*)p->payload, p->len, 0x1000);
//    strncpy(str1,p->payload,p->len);
//    str1[p->len]=0;
    pbuf_free(p);
//    HAL_SPI_Transmit(&hspi1, (uint8_t*)str1, 16, 0x1000);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
    HAL_GPIO_TogglePin(GPIOD, Green_Led_Pin);
}
//-----------------------------------------------
void packetSendUDP()
{
    HAL_GPIO_WritePin(GPIOD, Blue_Led_Pin, GPIO_PIN_SET);
    udp_client_send();
    HAL_GPIO_WritePin(GPIOD, Blue_Led_Pin, GPIO_PIN_RESET);
}
//--------------------------------------------------
