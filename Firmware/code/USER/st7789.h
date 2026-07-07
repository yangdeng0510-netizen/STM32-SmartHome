#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>

/* 屏幕方向
0 竖屏
1 竖屏翻转
2 横屏
3 横屏翻转
*/
#define USE_HORIZONTAL 0

/* LCD分辨率 */
#define LCD_W 240
#define LCD_H 240

/* ST7789 偏移 */
#define LCD_X_OFFSET 0
#define LCD_Y_OFFSET 25

/************ RGB565 颜色定义 ************/
#define WHITE       0xFFFF   // 白色
#define BLACK       0x0000   // 黑色

#define RED         0xF800   // 红色
#define GREEN       0x07E0   // 绿色
#define BLUE        0x001F   // 蓝色

#define YELLOW      0xFFE0   // 黄色 = 红 + 绿
#define CYAN        0x07FF   // 青色 = 绿 + 蓝
#define MAGENTA     0xF81F   // 品红色 = 红 + 蓝

#define BROWN       0xBC40   // 棕色
#define BRRED       0xFC07   // 亮红/偏粉红

#define GRAY        0x8430   // 灰色
#define LGRAY       0xC618   // 浅灰色

#define DARKBLUE    0x01CF   // 深蓝色
#define LIGHTBLUE   0x7D7C   // 浅蓝色

#define GRAYBLUE    0x5458   // 灰蓝色，适合做底部状态栏
#define LGRAYBLUE   0xA651   // 浅灰蓝色
#define LBBLUE      0x2B12   // 亮蓝色/浅蓝背景

#define LIGHTGREEN  0x841F   // 浅绿色/偏亮绿色

/* SPI1 */
#define LCD_SPI                   SPI1
#define LCD_SPI_RCC               RCC_APB2Periph_SPI1
#define LCD_SPI_RCC_CMD           RCC_APB2PeriphClockCmd

/* GPIO */
#define LCD_GPIO_RCC              (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD)

/* SCK */
#define LCD_SCL_PORT              GPIOB
#define LCD_SCL_PIN               GPIO_Pin_3
#define LCD_SCL_SOURCE            GPIO_PinSource3

/* MOSI */
#define LCD_SDA_PORT              GPIOB
#define LCD_SDA_PIN               GPIO_Pin_5
#define LCD_SDA_SOURCE            GPIO_PinSource5

/* CS */
#define LCD_CS_PORT               GPIOB
#define LCD_CS_PIN                GPIO_Pin_4

/* DC */
#define LCD_DC_PORT               GPIOD
#define LCD_DC_PIN                GPIO_Pin_3

/* BLK */
#define LCD_BLK_PORT              GPIOD
#define LCD_BLK_PIN               GPIO_Pin_4

/* 函数声明 */
void LCD_GPIO_Init(void);
void LCD_SPI_Init(void);

void LCD_WR_REG(u8 data);
void LCD_WR_DATA8(u8 data);
void LCD_WR_DATA(u16 data);

void LCD_Init(void);

void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2);

void LCD_Clear(u16 color);

void LCD_Fill(u16 x1,
              u16 y1,
              u16 x2,
              u16 y2,
              u16 color);

void LCD_DrawPoint(u16 x,
                   u16 y,
                   u16 color);

void LCD_ShowChar(u16 x,
                  u16 y,
                  char ch,
                  u16 fc,
                  u16 bc);

void LCD_ShowString(u16 x,
                    u16 y,
                    const char *str,
                    u16 fc,
                    u16 bc);
void LCD_ShowHZ16(u16 x, 
                  u16 y, 
                  u8 index, 
                  u16 fc, 
                  u16 bc);
void LCD_ShowHZ32(u16 x, u16 y, u8 index, u16 fc, u16 bc);
										
static void LCD_Address_Set_Raw(u16 x1, u16 y1, u16 x2, u16 y2);
static void LCD_Clear_GRAM(u16 color);	
void LCD_Demo(void);
void LCD_SHT31_stastic(void);
void LCD_SHT31_UpdateValue(float temp, float humi);
void MLX90614_Stastic(void);
	void MLX90614_UpdateValue(float body_temp);
#endif
