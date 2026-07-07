# include "uart.h"
void uart1_init(void)
{
	/*开启RCC时钟*/
	RCC->AHB1ENR|=(1<<0);
	RCC->APB2ENR|=(1<<4);
	/*配置GPIO工作模式为复用*/
	GPIOA->MODER&=~(0xf<<18);//18-21位清零
	GPIOA->MODER|=(0xA<<18);
	/*配置PA9 PA10的复用引脚AF7*/
  GPIOA->AFR[1]&=~((0xf<<4)|(0xf<<8));
	GPIOA->AFR[1]|=(0x7<<4);
	GPIOA->AFR[1]|=(0x7<<4*2);
	/*上拉引脚保证tx rx 高电平稳定*/
	GPIOA->PUPDR &= ~(0xF << 18); // 18-21位清零
	GPIOA->PUPDR|=(5<<18);
	/*配置GPIO工作速度*/
	GPIOA->OSPEEDR&=~(0xf<<18);//18-21位清零
	GPIOA->OSPEEDR|=(0xA<<18);
  /*使能GPIO寄存器*/
	//GPIOA->BSRRL=(1<<9)|(1<<10);
	
	/*停止位*/
	USART1->CR2&=~(3<<12);
	/*校验位*/
	USART1->CR1&=~((1<<10)|(1<<9));
	/*波特率*/
	usart1_set_baudrate(84000000, 115200);
	/*使能*/
	USART1->CR1|=(1<<13)|(1<<2)|(1<<3);
}
/**
 * @brief  自定义的串口1波特率设置函数（纯寄存器版）
 * @param  pclk: 串口挂载的总线时钟频率（USART1 挂在 APB2 上，通常是 84000000）
 * @param  baudrate: 你想要设置的波特率（如 115200, 9600 等）
 */
void usart1_set_baudrate(uint32_t pclk, uint32_t baudrate)
{
    uint32_t integerdivider = 0x00;
    uint32_t fractionaldivider = 0x00;
    uint32_t tmpreg = 0x00;

    // 1. 采用官方的“放大 100 倍”整数算法，规避浮点数运算
    // 这里默认采用 16 倍过采样（即 OVER8 = 0）
    integerdivider = ((25 * pclk) / (4 * baudrate));    

    // 2. 提取并算好整数部分（Mantissa），并向左移位 4 位留出低 4 位空间
    tmpreg = (integerdivider / 100) << 4;

    // 3. 提取出纯粹的小数部分（放大了 100 倍的十进制数）
    fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

    // 4. 将十进制小数换算成寄存器需要的 4 位二进制，+50 用来实现无浮点四舍五入
    tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & 0x0F;
    
    // 5. 最终合体，精准无误地写入波特率寄存器 BRR
    USART1->BRR = (uint16_t)tmpreg;
}

void uart1_send_byte(u8 ch)
{
   while(!((USART1->SR)&(1<<7))){}
	 USART1->DR=ch;
	 //while(!((USART1->SR) & (1 << 6))) {}
}
void uart1_send_string(char *str)
{
   while(*str)
	 {
	    uart1_send_byte(*str);
		  str++;
	 }
	 while(!((USART1->SR) & (1 << 6))) {}
}

#include "stdio.h"
int fputc(int ch,FILE *f)
{
    uart1_send_byte(ch);
     return ch;
}

#include "stdio.h"

// 重定向 stdout (printf) 之前写过了，这里补充 stdin (scanf)
int fgetc(FILE *f)
{
    // 直接调用你写好的、会死等数据的单字节接收函数
    return (int)uart1_receive_byte();
}


char uart1_receive_byte(void)
{
   while(!((USART1->SR)&(1<<5))){}
	  return USART1->DR;
}











