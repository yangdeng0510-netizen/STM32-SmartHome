#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx.h"  

/* 1. 宏定义 (Defines) */
#define KEY1_GPIO_PORT    GPIOA 
#define KEY1_PIN          GPIO_Pin_0
#define KEY2_GPIO_PORT    GPIOB
#define KEY2_PIN          GPIO_Pin_0
#define KEY3_GPIO_PORT    GPIOB
#define KEY3_PIN          GPIO_Pin_1

#define LED1_GPIO_PORT    GPIOD
#define LED1_PIN          GPIO_Pin_12
#define LED2_GPIO_PORT    GPIOD
#define LED2_PIN          GPIO_Pin_13
#define LED3_GPIO_PORT    GPIOD
#define LED3_PIN          GPIO_Pin_14

#define LED1_OFF         GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN);
#define LED2_OFF         GPIO_SetBits(LED2_GPIO_PORT,LED2_PIN);
#define LED3_OFF         GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN);
#define LED1_ON          GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN);
#define LED2_ON          GPIO_ResetBits(LED2_GPIO_PORT,LED2_PIN);
#define LED3_ON          GPIO_ResetBits(LED3_GPIO_PORT,LED3_PIN);

/* 2. 结构体/共用体声明 (Structures / Typedefs) */
// typedef struct { ... } MyStruct;

/* 3. 全局变量声明 (使用 extern) */
// extern uint8_t Key_Status;

/* 4. 函数声明 (Function Prototypes) */
void KEY_Init(void);          // 初始化按键引脚
void LED_Init(void);          // 初始化按键引脚
uint8_t KEY_Scan(void);       // 扫描按键状态


#endif /* __KEY_H */




