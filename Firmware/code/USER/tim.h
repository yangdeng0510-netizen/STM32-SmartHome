#ifndef __TIM_H
#define __TIM_H

#include "stm32f4xx.h"  

/* 1. 宏定义 (Defines) */
#define  Electric_fan   0
/* 2. 结构体/共用体声明 (Structures / Typedefs) */
// typedef struct { ... } MyStruct;

/* 3. 全局变量声明 (使用 extern) */
// extern uint8_t XXX_Status;
   extern int pwm_speed;
/* 4. 函数声明 (Function Prototypes) */
void servo_angle (float angle);
void TIM3_CH3_PC8_Init(void);
void TIM5_CH2_PA1_init(void);
void SetCompare_TIM3_CH3(u32 Compare);
#endif /* __XXX_H */
