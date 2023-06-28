#ifndef   NEW_HTTP_SERVER
#ifndef __NET_H
#define __NET_H
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "w5500.h"

#define be16toword(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))

void net_poll(void);
void net_ini(void);
void packet_receive(void);
#endif /* __NET_H */
#endif
