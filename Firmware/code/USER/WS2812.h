#ifndef __WS2812_H
#define __WS2812_H

#include "stm32f4xx.h"
#include <stdint.h>

/*
 * 开发板 WS2812 使用 PB15
 * PB15 = SPI2_MOSI
 */
#define WS2812_LED_NUM              4

#define WS2812_SPI                  SPI2
#define WS2812_SPI_CLK              RCC_APB1Periph_SPI2

#define WS2812_GPIO_PORT            GPIOB
#define WS2812_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define WS2812_GPIO_PIN             GPIO_Pin_15
#define WS2812_GPIO_PINSOURCE       GPIO_PinSource15
#define WS2812_GPIO_AF              GPIO_AF_SPI2

/*
 * STM32F405:
 * SPI2_TX -> DMA1 Stream4 Channel0
 */
#define WS2812_DMA_CLK              RCC_AHB1Periph_DMA1
#define WS2812_DMA_STREAM           DMA1_Stream4
#define WS2812_DMA_CHANNEL          DMA_Channel_0

/*
 * SPI 3bit 编码：
 * WS2812 0码 -> 100
 * WS2812 1码 -> 110
 *
 * APB1 = 42MHz
 * SPI2 = 42MHz / 16 = 2.625MHz
 */
#define WS2812_RESET_BYTES          120
#define WS2812_DATA_BYTES           (WS2812_LED_NUM * 9)
#define WS2812_SPI_BUF_LEN          (WS2812_RESET_BYTES + WS2812_DATA_BYTES + WS2812_RESET_BYTES)

/*
 * 保留这两个宏，避免 main.c 里旧代码编译报错。
 * SPI+DMA 实际不再依赖手动拉高拉低。
 */
#define WS2812_HIGH                 GPIO_SetBits(WS2812_GPIO_PORT, WS2812_GPIO_PIN)
#define WS2812_LOW                  GPIO_ResetBits(WS2812_GPIO_PORT, WS2812_GPIO_PIN)

extern uint8_t WS2812_Color[WS2812_LED_NUM][3];

/*
 * 为了兼容你原来的 main.c，函数名保持不变。
 */
void WS2812_GPIO_Init_Manual(void);
void WS2812_DWT_Init(void);
void WS2812_Show_Manual(void);
void WS2812_Off_Manual(void);

void WS2812_SetPixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void WS2812_Refresh(void);

#endif
