#include "st7789.h"
#include "systick.h"
#include "ziku.h"

static void LCD_SPI_WriteByte(u8 data)
{
    while (SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(LCD_SPI, data);
    while (SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) == SET);
}

static void LCD_SetCS(BitAction value)
{
    GPIO_WriteBit(LCD_CS_PORT, LCD_CS_PIN, value);
}

static void LCD_SetDC(BitAction value)
{
    GPIO_WriteBit(LCD_DC_PORT, LCD_DC_PIN, value);
}

static void LCD_SetBLK(BitAction value)
{
    GPIO_WriteBit(LCD_BLK_PORT, LCD_BLK_PIN, value);
}

void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(LCD_GPIO_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
    GPIO_InitStructure.GPIO_Pin = LCD_SCL_PIN | LCD_SDA_PIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(LCD_SCL_PORT, LCD_SCL_SOURCE, GPIO_AF_SPI1);
    GPIO_PinAFConfig(LCD_SDA_PORT, LCD_SDA_SOURCE, GPIO_AF_SPI1);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
    GPIO_InitStructure.GPIO_Pin = LCD_CS_PIN;
    GPIO_Init(LCD_CS_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LCD_DC_PIN | LCD_BLK_PIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    LCD_SetCS(Bit_SET);
    LCD_SetDC(Bit_SET);
    LCD_SetBLK(Bit_RESET);
}

void LCD_SPI_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;

    LCD_SPI_RCC_CMD(LCD_SPI_RCC, ENABLE);
    SPI_I2S_DeInit(LCD_SPI);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(LCD_SPI, &SPI_InitStructure);
    SPI_Cmd(LCD_SPI, ENABLE);
}

void LCD_WR_REG(u8 data)
{
    LCD_SetDC(Bit_RESET);
    LCD_SPI_WriteByte(data);
}

void LCD_WR_DATA8(u8 data)
{
    LCD_SetDC(Bit_SET);
    LCD_SPI_WriteByte(data);
}

void LCD_WR_DATA(u16 data)
{
    LCD_WR_DATA8((u8)(data >> 8));
    LCD_WR_DATA8((u8)data);
}

void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
	
	  x1 += LCD_X_OFFSET;
    x2 += LCD_X_OFFSET;
    y1 += LCD_Y_OFFSET;
    y2 += LCD_Y_OFFSET;
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(x1);
    LCD_WR_DATA(x2);
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(y1);
    LCD_WR_DATA(y2);
    LCD_WR_REG(0x2C);
}
static void LCD_Address_Set_Raw(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(x1);
    LCD_WR_DATA(x2);

    LCD_WR_REG(0x2B);
    LCD_WR_DATA(y1);
    LCD_WR_DATA(y2);

    LCD_WR_REG(0x2C);
}

static void LCD_Clear_GRAM(u16 color)
{
    u32 i;

    LCD_Address_Set_Raw(0, 0, 239, 319);

    for (i = 0; i < 240UL * 320UL; i++)
    {
        LCD_WR_DATA(color);
    }
}
void LCD_Clear(u16 color)
{
    u32 i;

    LCD_Address_Set(0, 0, LCD_W - 1, LCD_H - 1);
    for (i = 0; i < (u32)LCD_W * LCD_H; i++)
    {
        LCD_WR_DATA(color);
    }
}

void LCD_Fill(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u32 i;
    u32 total;

    if (x1 >= LCD_W || y1 >= LCD_H) return;
    if (x2 >= LCD_W) x2 = LCD_W - 1;
    if (y2 >= LCD_H) y2 = LCD_H - 1;
    if (x2 < x1 || y2 < y1) return;

    LCD_Address_Set(x1, y1, x2, y2);
    total = (u32)(x2 - x1 + 1) * (y2 - y1 + 1);
    for (i = 0; i < total; i++)
    {
        LCD_WR_DATA(color);
    }
}

void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
    if (x >= LCD_W || y >= LCD_H) return;

    LCD_Address_Set(x, y, x, y);
    LCD_WR_DATA(color);
}

void LCD_ShowChar(u16 x, u16 y, char ch, u16 fc, u16 bc)
{
    u8 i;
    u8 j;
    u8 index;
    const unsigned char *font;

    if (ch >= 'A' && ch <= 'Z') index = ch - 'A';
    else if (ch >= '1' && ch <= '9') index = ch - '1' + 26;
    else if (ch == '0') index = 35;
    else if (ch == ':') index = 47;
    else if (ch == ' ') index = 51;
    else index = 51;

    font = &zfm[index * 16];
    LCD_Address_Set(x, y, x + 7, y + 15);
    for (i = 0; i < 16; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (font[i] & (0x80 >> j)) LCD_WR_DATA(fc);
            else LCD_WR_DATA(bc);
        }
    }
}

void LCD_ShowString(u16 x, u16 y, const char *str, u16 fc, u16 bc)
{
    while (*str != '\0')
    {
        if (x + 8 > LCD_W)
        {
            x = 0;
            y += 16;
        }
        if (y + 16 > LCD_H) break;
        LCD_ShowChar(x, y, *str, fc, bc);
        x += 8;
        str++;
    }
}
void LCD_ShowHZ16(u16 x, u16 y, u8 index, u16 fc, u16 bc)
{
    u8 i, j;
    u8 temp;
    const unsigned char *p = &hzm16[index * 32];

    for(i = 0; i < 16; i++)
    {
        for(j = 0; j < 16; j++)
        {
            temp = p[i * 2 + j / 8];

            if(temp & (0x80 >> (j % 8)))
                LCD_DrawPoint(x + j, y + i, fc);
            else
                LCD_DrawPoint(x + j, y + i, bc);
        }
    }
}
void LCD_ShowHZ32(u16 x, u16 y, u8 index, u16 fc, u16 bc)
{
    u8 i, j;
    u8 temp;
    const unsigned char *p = &hzm32[index * 128];

    for(i = 0; i < 32; i++)
    {
        for(j = 0; j < 32; j++)
        {
            temp = p[i * 4 + j / 8];

            if(temp & (0x80 >> (j % 8)))
                LCD_DrawPoint(x + j, y + i, fc);
            else
                LCD_DrawPoint(x + j, y + i, bc);
        }
    }
}
void LCD_Init(void)
{
    LCD_GPIO_Init();
    LCD_SPI_Init();
    LCD_SetCS(Bit_RESET);
    delay_ms(120);

    LCD_WR_REG(0x11);
    delay_ms(120);

    LCD_WR_REG(0x36);
    if (USE_HORIZONTAL == 0) LCD_WR_DATA8(0x00);
    else if (USE_HORIZONTAL == 1) LCD_WR_DATA8(0xC0);
    else if (USE_HORIZONTAL == 2) LCD_WR_DATA8(0x70);
    else LCD_WR_DATA8(0xA0);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA8(0x35);

    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x19);

    LCD_WR_REG(0xC0);
    LCD_WR_DATA8(0x2C);

    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x12);

    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20);

    LCD_WR_REG(0xC6);
    LCD_WR_DATA8(0x0F);

    LCD_WR_REG(0xD0);
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x2B);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x54);
    LCD_WR_DATA8(0x4C);
    LCD_WR_DATA8(0x18);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x0B);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x23);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x2C);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x44);
    LCD_WR_DATA8(0x51);
    LCD_WR_DATA8(0x2F);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x20);
    LCD_WR_DATA8(0x23);

    LCD_WR_REG(0x21);
    LCD_WR_REG(0x29);
    delay_ms(20);
    LCD_SetBLK(Bit_SET);
		LCD_Clear_GRAM(BLACK);
    LCD_Clear(BLACK);
}
 void LCD_Demo(void)
{
    LCD_Clear(BLACK);
    
    /************ 顶部标题栏 ************/
    // 区域：y = 0 ~ 29，高度 30
	  u8 zhjj_x_status=60;
	  u8 zhjj_y_status=10;
    LCD_Fill(0, 0, 239, 50, BLUE);
    LCD_ShowHZ32(zhjj_x_status ,   zhjj_y_status, 0, WHITE, BLACK);
	  LCD_ShowHZ32(zhjj_x_status+32 ,zhjj_y_status, 1, WHITE, BLACK);
	  LCD_ShowHZ32(zhjj_x_status+64 ,zhjj_y_status, 2, WHITE, BLACK);
	  LCD_ShowHZ32(zhjj_x_status+96 ,zhjj_y_status, 3, WHITE, BLACK);


    /************ 测量环境温度卡片 ************/
    u8 hjwsd_x_status=5;
	  u8 hjwsd_y_status=72;
    LCD_Fill(0, 60, 239, 100, BROWN); //0156478
    LCD_ShowHZ16(hjwsd_x_status,    hjwsd_y_status, 0, WHITE, BLACK);  // 测
    LCD_ShowHZ16(hjwsd_x_status+16, hjwsd_y_status, 1, WHITE, BLACK);  // 量
    LCD_ShowHZ16(hjwsd_x_status+32, hjwsd_y_status, 5, WHITE, BLACK);  // 环
    LCD_ShowHZ16(hjwsd_x_status+48, hjwsd_y_status, 6, WHITE, BLACK);  // 境
		LCD_ShowHZ16(hjwsd_x_status+64, hjwsd_y_status, 4, WHITE, BLACK);  // 温
		LCD_ShowHZ16(hjwsd_x_status+80, hjwsd_y_status, 7, WHITE, BLACK);  // 湿
		LCD_ShowHZ16(hjwsd_x_status+96, hjwsd_y_status, 8, WHITE, BLACK);  // 度
		
		    /************ 测量体温卡片 ************/
    u8 rttw_x_status=5;
	  u8 rttw_y_status=122;
    LCD_Fill(0, 110, 239, 150, BROWN); //012348
    LCD_ShowHZ16(rttw_x_status,    rttw_y_status, 0, WHITE, BLACK);  // 测
    LCD_ShowHZ16(rttw_x_status+16, rttw_y_status, 1, WHITE, BLACK);  // 量
    LCD_ShowHZ16(rttw_x_status+32, rttw_y_status, 2, WHITE, BLACK);  // 人
    LCD_ShowHZ16(rttw_x_status+48, rttw_y_status, 3, WHITE, BLACK);  // 体
		LCD_ShowHZ16(rttw_x_status+64, rttw_y_status, 4, WHITE, BLACK);  // 温
		LCD_ShowHZ16(rttw_x_status+80, rttw_y_status, 8, WHITE, BLACK);  // 度
		
				    /************ RGB灯带设置 ************/
    u8 rgb_x_status=15;
	  u8 rgb_y_status=172;
    LCD_Fill(0, 160, 115, 200, BROWN); //012348
		LCD_Fill(125, 160, 239, 200, BROWN); //012348
		
		for(u8 i=0;i<2;i++)
		{
		LCD_ShowString(rgb_x_status+i*120, rgb_y_status, "RGB", WHITE, BLACK);
		LCD_ShowHZ16(rgb_x_status+24+i*120, rgb_y_status, 17+i*6, WHITE, BLACK);  // 打/关
    LCD_ShowHZ16(rgb_x_status+40+i*120, rgb_y_status, 18+i*6, WHITE, BLACK); // 开
		}
		
	
		
					/************ 舵机 ************/
			u8 dj_x_status=5;
			u8 dj_y_status=220;
      LCD_Fill(0, 210, 70, 249, BROWN); 
			LCD_Fill(80, 210, 150, 249, BROWN); 
			LCD_Fill(160, 210, 239, 249, BROWN); 
			for(u8 i=0;i<3;i++)
			{ 
				u8 x_distance=80;
			LCD_ShowHZ16(dj_x_status+i*x_distance, dj_y_status, 15, WHITE, BLACK);  // 舵
      LCD_ShowHZ16(dj_x_status+16+i*x_distance, dj_y_status, 16, WHITE, BLACK);  // 机
			if(i==0){LCD_ShowString(dj_x_status+32+i*x_distance, dj_y_status, "0", WHITE, BLACK);}
	  	if(i==1){LCD_ShowString(dj_x_status+32+i*x_distance, dj_y_status, "90", WHITE, BLACK);}
			if(i==2){LCD_ShowString(dj_x_status+32+i*x_distance, dj_y_status, "180", WHITE, BLACK);}
      LCD_ShowHZ16(dj_x_status+40+i*(x_distance+8), dj_y_status, 8, WHITE, BLACK);  // 度
			}
     
 } 
    void LCD_SHT31_stastic(void)
		{
		   
		    LCD_Fill(0, 50, 239, 100, RED);
			  LCD_Fill(0, 120, 239, 170, BLUE); 
			  u8 hjwsd_x_status=5;
	      u8 hjwsd_y_status=70;
			  LCD_ShowHZ16(hjwsd_x_status+16, hjwsd_y_status, 5, WHITE, BLACK);  // 环
        LCD_ShowHZ16(hjwsd_x_status+32, hjwsd_y_status, 6, WHITE, BLACK);  // 境
			  LCD_ShowHZ16(hjwsd_x_status+48, hjwsd_y_status, 4, WHITE, BLACK);  // 温
			  LCD_ShowHZ16(hjwsd_x_status+64, hjwsd_y_status, 8, WHITE, BLACK);  // 度
			  LCD_ShowHZ16(hjwsd_x_status+16, hjwsd_y_status+70, 5, WHITE, BLACK);  // 环
        LCD_ShowHZ16(hjwsd_x_status+32, hjwsd_y_status+70, 6, WHITE, BLACK);  // 境
	      LCD_ShowHZ16(hjwsd_x_status+48, hjwsd_y_status+70, 7, WHITE, BLACK);  // 湿
    	  LCD_ShowHZ16(hjwsd_x_status+64, hjwsd_y_status+70, 8, WHITE, BLACK);  // 度
		}
		
			void LCD_SHT31_UpdateValue(float temp, float humi)
			{
					char buf[20];

					int temp_x10;
					int humi_x10;
					if(temp >= 0)
							temp_x10 = (int)(temp * 10 + 0.5f);
					else
							temp_x10 = (int)(temp * 10 - 0.5f);

					humi_x10 = (int)(humi * 10 + 0.5f);

					/*************** 刷新温度数值区域 ***************/
					  LCD_Fill(85, 70, 150, 90, RED); // 擦除旧温度数字，背景色要和页面一致
					  
         
					if(temp_x10 >= 0)
					{
							sprintf(buf, "%d.%d C", temp_x10 / 10, temp_x10 % 10);
					}
					else
					{
							temp_x10 = -temp_x10;
							sprintf(buf, "-%d.%d C", temp_x10 / 10, temp_x10 % 10);
					}

					LCD_ShowString(100, 70, buf, WHITE, RED);


					/*************** 刷新湿度数值区域 ***************/
				  LCD_Fill(85, 70+70, 150, 90+70, BLUE); // 擦除旧湿度数字，背景色要和页面一致

					sprintf(buf, "%d.%d %%", humi_x10 / 10, humi_x10 % 10);

					LCD_ShowString(100, 140, buf, WHITE, BLUE);
			}
			
			void MLX90614_Stastic(void)
			{
			   LCD_Fill(0, 50, 239, 100, RED);
				u8 hjwsd_x_status=5;
	      u8 hjwsd_y_status=70;
			  LCD_ShowHZ16(hjwsd_x_status+16, hjwsd_y_status, 2, WHITE, BLACK);  // 环
        LCD_ShowHZ16(hjwsd_x_status+32, hjwsd_y_status, 3, WHITE, BLACK);  // 境
			  LCD_ShowHZ16(hjwsd_x_status+48, hjwsd_y_status, 4, WHITE, BLACK);  // 温
			  LCD_ShowHZ16(hjwsd_x_status+64, hjwsd_y_status, 8, WHITE, BLACK);  // 度
				
			}
		void MLX90614_UpdateValue(float body_temp)
{
    char buf[20];
    int temp_x10;

    /*
     * 体温一般是 35.0 ~ 42.0
     * 放大 10 倍，保留 1 位小数
     */
    if(body_temp >= 0)
        temp_x10 = (int)(body_temp * 10 + 0.5f);
    else
        temp_x10 = (int)(body_temp * 10 - 0.5f);

    /*
     * 擦除旧体温数字区域
     * 你的红色区域是 y=50~100，文字 y=70
     */
    LCD_Fill(85, 70, 170, 90, RED);

    /*
     * 先用整数形式显示，例如 36 C
     * 这样不受小数点字库影响
     */
    sprintf(buf, "%d C", temp_x10 / 10);

    LCD_ShowString(100, 70, buf, WHITE, RED);
}

