#ifndef __RELAY_H
#define __RELAY_H	 

#include "sys.h"


#define Relay0	PBout(8)			//继电器
#define Relay1	PBout(7)			//继电器
#define Relay2	PBout(6)			//继电器

extern unsigned char Relay0_state;			//继电器1的状态
extern unsigned char Relay1_state;			//继电器2的状态
extern unsigned char Relay2_state;			//继电器3的状态

void RELAY_Init(void);			//初始化

		 				    
#endif
