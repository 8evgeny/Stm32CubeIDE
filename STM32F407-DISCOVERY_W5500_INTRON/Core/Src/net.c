#include "net.h"
//-----------------------------------------------
uint8_t ipaddr[4];
uint8_t ipgate[4];
uint8_t ipmask[4];

char str1[60] __attribute__ ((aligned (4))) ={0} ;
//-----------------------------------------------
void packet_receive(void)
{
	uint8_t i;
    for(i = 0; i < 1; i++)
  {
    w5500_packetReceive(i);
  }
}
//-----------------------------------------------
void net_poll(void)
{
  packet_receive();
}
//-----------------------------------------------
void net_ini(void)
{
	w5500_ini();
}
//-----------------------------------------------
