#ifndef __Humidity_Temperature_H
#define __Humidity_Temperature_H

#include "stm32f4xx.h"  
#include "stdio.h"

/* 1. 宏定义 (Defines) */
#define SCL_H()  GPIO_SetBits(GPIOB, GPIO_Pin_6)
#define SCL_L()  GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define SDA_H()  GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define SDA_L()  GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define SHT31_Addr_Read   0x88|1
#define SHT31_Addr_Write  0x88|0
#define SDA_Read()  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)



/* 2. 结构体/共用体声明 (Structures / Typedefs) */
// typedef struct { ... } MyStruct;

/* 3. 全局变量声明 (使用 extern) */
// extern uint8_t XXX_Status;

/* 4. 函数声明 (Function Prototypes) */
 void SHT31_GPIO_Init(void);
 void SHT31_IIC_Start(void);
 void SHT31_IIC_END(void);
 void SHT31_SendByte(u8 data);
 u8 SHT31_WaitAck(void);
 void SHT31_SendAck(u8 ack);
 u8  SHT31_ReceiveByte(u8 ack);
 u8 SHT31_ReadTempHumi(float *temp, float *humi);

#endif  /* __XXX_H */


