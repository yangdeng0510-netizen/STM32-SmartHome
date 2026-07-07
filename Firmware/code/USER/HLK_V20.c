/*

语音识别

*/
# include "HLK_V20.h"
#include "stdio.h"
volatile uint8_t hlk_rx_buf[4];
volatile uint8_t hlk_rx_cnt = 0;
volatile uint8_t hlk_rx_flag = 0;
/************************************************************
函数名：void GPIO_HLK_Init
功能：
参数：
返回值：
备注：PD8-U3TX
      PD9-U3RX
************************************************************/
void HLK_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStruct;

    // 1. 开时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    // 2. AF复用
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

    // 3. GPIO配置
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOD, &GPIO_InitStruct);

    // 4. USART配置
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;

    USART_InitStructure.USART_Mode =
        USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);

    // 5. 开接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    // 6. NVIC配置
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStruct);

    // 7. 使能USART3
    USART_Cmd(USART3, ENABLE);
}

 uint8_t HLK_Cmd_Check(uint8_t d0,
                      uint8_t d1,
                      uint8_t d2,
                      uint8_t d3)
{
    if(hlk_rx_buf[0] == d0 &&
       hlk_rx_buf[1] == d1 &&
       hlk_rx_buf[2] == d2 &&
       hlk_rx_buf[3] == d3)
    {
        return 1;
    }
    return 0;
}













