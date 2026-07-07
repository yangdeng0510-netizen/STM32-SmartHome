#ifndef _W25Q64_H
#define _W25Q64_H

#include "stm32f4xx.h"

#define W25Q64_SPI       SPI1
#define W25Q64_SPI_RCC   RCC_APB2Periph_SPI1

#define W25Q64_CS_RCC    RCC_AHB1Periph_GPIOC
#define W25Q64_CS_GPIO   GPIOC
#define W25Q64_CS_Pin    GPIO_Pin_7

#define W25Q64_SCK_RCC   RCC_AHB1Periph_GPIOA
#define W25Q64_SCK_GPIO  GPIOA
#define W25Q64_SCK_Pin   GPIO_Pin_5

#define W25Q64_MISO_RCC  RCC_AHB1Periph_GPIOA
#define W25Q64_MISO_GPIO GPIOA
#define W25Q64_MISO_Pin  GPIO_Pin_6

#define W25Q64_MOSI_RCC  RCC_AHB1Periph_GPIOA
#define W25Q64_MOSI_GPIO GPIOA
#define W25Q64_MOSI_Pin  GPIO_Pin_7

#define W25Q64_CS_H      GPIO_SetBits(W25Q64_CS_GPIO, W25Q64_CS_Pin)
#define W25Q64_CS_L      GPIO_ResetBits(W25Q64_CS_GPIO, W25Q64_CS_Pin)

void W25Q64_SPI_Init(void);
u8 W25Q64_Send_Recv_Byte(u8 data);
void W25Q64_Sector_Erase(u32 addr);
void W25Q64_Write_Enable(void);
u8 W25Q64_Read_Status(void);
u8 W25Q64_GET_FLAG_BUSY(void);
void W25Q64_Page_Program(u32 addr, u8 *buf, int length);
void W25Q64_Read_Data(u32 addr, u8 *buf, int length);
void W25Q64_Chip_Erase(void);
void W25Q64_Block_Erase(u32 addr);

#endif
