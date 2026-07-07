#include "systick.h"


void delay_us(u32 xus)
{
    u32 ticks;

    ticks = xus * (SYSTICK_CLOCK / 8 / 1000000);

    if (ticks == 0)
    {
        ticks = 1;
    }

    if (ticks > 0xFFFFFF)
    {
        ticks = 0xFFFFFF;
    }

    SysTick->CTRL = 0;          // 彻底关闭 SysTick，清掉 ENABLE/TICKINT/CLKSOURCE
    SysTick->LOAD = ticks - 1;  // 装载计数值
    SysTick->VAL  = 0;          // 清当前计数值，也清 COUNTFLAG

    SysTick->CTRL = 1;          // 只开 ENABLE，使用 AHB/8，不开中断

    while ((SysTick->CTRL & (1 << 16)) == 0);

    SysTick->CTRL = 0;          // 用完彻底关闭
}
//void  delay_us(u32 xus)
//{
//	 // 1.选择外部参考时钟作为系统定时器计数频率
//	SysTick->CTRL &=~(1<<2);
//	// 2. 写入重装载值--计数初值
//	SysTick->LOAD =xus *(SYSTICK_CLOCK/8/1000000);
//	// 3. 清零当前值计数器，让重装载值加在到计数器中
//	SysTick->VAL =0;
//	// 4. 使能定时器，开始计数
//	SysTick->CTRL |=(1<<0);
//	// 5. 等待定时时间到
//	while((SysTick->CTRL &(1<<16))==0);
//	// 4. 失能定时器，停止计数
//	SysTick->CTRL &=~(1<<0);
//}
void  delay_ms(u32 xms)
{
     while(xms--)
		 {
		   delay_us(1000);
		 }
}

void delay_s(u32 xs)
{
   while(xs--)
		 {
		   delay_ms(1000);
		 }
}

