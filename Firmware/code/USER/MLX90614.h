#ifndef __MLX90614_H
#define __MLX90614_H

#include "stm32f4xx.h"  
#include "stdio.h"

/* 1. 宏定义 (Defines) */
#define SCL_H()  GPIO_SetBits(GPIOB, GPIO_Pin_6)
#define SCL_L()  GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define SDA_H()  GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define SDA_L()  GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define MLX90614_Addr_Read   0xB5
#define MLX90614_Addr_Write  0xB4
#define SDA_Read()  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)



/* 2. 结构体/共用体声明 (Structures / Typedefs) */
// typedef struct { ... } MyStruct;

/* 3. 全局变量声明 (使用 extern) */
// extern uint8_t XXX_Status;

/* 4. 函数声明 (Function Prototypes) */
 void MLX90614_GPIO_Init(void);
 void MLX90614_IIC_Start(void);
 void MLX90614_IIC_END(void);
 void MLX90614_SendByte(u8 data);
 u8 MLX90614_WaitAck(void);
 void MLX90614_SendAck(u8 ack);
 u8  MLX90614_ReceiveByte(u8 ack);
  void MLX90614_SMBus_Request(void);
 u8 MLX90614_ReadHumen_Temp(float *temp);
#endif  /* __XXX_H */
