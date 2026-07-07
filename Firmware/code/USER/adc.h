#ifndef __ADC_H
#define __ADC_H

#include "stm32f4xx.h"  

/* 1. 宏定义 (Defines) */


/* 2. 结构体/共用体声明 (Structures / Typedefs) */
// typedef struct { ... } MyStruct;

/* 3. 全局变量声明 (使用 extern) */
// extern uint8_t XXX_Status;

/* 4. 函数声明 (Function Prototypes) */
void ADC1_LDR_PC0_Init(void);          // 标准库版本
void ADC1_LDR_PC0_Reg_Init(void);      // 寄存器版本

 u16 Read_ADC1_LDR_PC0(void);
#endif /* __XXX_H */


