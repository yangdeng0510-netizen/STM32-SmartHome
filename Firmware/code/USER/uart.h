#ifndef __UART_H
#define __UART_H

#include "stm32f4xx.h"  

/* 1. 宏定义 (Defines) */


/* 2. 结构体/共用体声明 (Structures / Typedefs) */
// typedef struct { ... } MyStruct;

/* 3. 全局变量声明 (使用 extern) */
// extern uint8_t XXX_Status;

/* 4. 函数声明 (Function Prototypes) */
void uart1_init(void);
void usart1_set_baudrate(uint32_t pclk2, uint32_t baud);
void uart1_send_byte(u8 ch);
void uart1_send_string(char *str);
char uart1_receive_byte( void);
#endif /* __XXX_H */
