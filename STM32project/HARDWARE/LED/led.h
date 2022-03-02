#ifndef _LED_H
#define _LED_H
#include "sys.h"


//LED端口定义
#define LED0 PFout(9)
#define LED1 PFout(10)

void LED_Init(void);  //初始化
#endif
