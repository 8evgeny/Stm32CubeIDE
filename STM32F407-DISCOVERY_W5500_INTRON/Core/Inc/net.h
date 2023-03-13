#ifndef __NET_H
#define __NET_H
//--------------------------------------------------
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "w5500.h"
//--------------------------------------------------

#ifdef INTRON
#define IP_ADDR {192,168,1,197}
#define LOCAL_PORT 3000
#endif
#ifndef INTRON
#define IP_ADDR {192,168,1,198}
#define LOCAL_PORT 3000
#endif
#define IP_GATE {192,168,1,1}
#define IP_MASK {255,255,255,0}

//--------------------------------------------------
#define be16toword(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))
//--------------------------------------------------
void net_poll(void);
void net_ini(void);
void packet_receive(void);
//--------------------------------------------------
#endif /* __NET_H */
