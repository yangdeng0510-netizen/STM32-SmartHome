#include "usart2.h"

WIFI_RECV WIFI_Rec;

void USART2_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate            = baud;
    USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits            = USART_StopBits_1;
    USART_InitStruct.USART_Parity              = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStruct);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    WIFI_Rec_Clear();

    USART_Cmd(USART2, ENABLE);
}

void USART2_SendByte(uint8_t data)
{
    USART_SendData(USART2, data);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void USART2_SendStr(char *buf)
{
    while (*buf != '\0')
    {
        USART2_SendByte((uint8_t)(*buf));
        buf++;
    }
}

uint8_t USART2_RecvByte(void)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
    return (uint8_t)USART_ReceiveData(USART2);
}

void WIFI_Rec_Clear(void)
{
    WIFI_Rec.flag = 0;
    WIFI_Rec.len = 0;
    memset(WIFI_Rec.buf, 0, WIFI_RX_BUF_SIZE);
}

void USART2_IRQHandler(void)
{
    uint8_t data;

    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        data = (uint8_t)USART_ReceiveData(USART2);

        if (WIFI_Rec.len < WIFI_RX_BUF_SIZE - 1)
        {
            if (data == 0)
            {
                WIFI_Rec.buf[WIFI_Rec.len++] = ' ';
            }
            else
            {
                WIFI_Rec.buf[WIFI_Rec.len++] = data;
            }
        }
        else
        {
            WIFI_Rec.buf[WIFI_RX_BUF_SIZE - 1] = '\0';
            WIFI_Rec.flag = 1;
        }

        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }

    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        volatile uint32_t temp;

        temp = USART2->SR;
        temp = USART2->DR;
        (void)temp;

        WIFI_Rec.buf[WIFI_Rec.len] = '\0';
        WIFI_Rec.flag = 1;

        /*
           注意：
           这里不建议直接 printf("%s\r\n", WIFI_Rec.buf);
           因为 printf 本身可能走 USART1，
           在 USART2 中断里打印容易造成中断嵌套、卡顿或数据乱。
           调试时可以临时打开，正式工程建议放到 main 循环里打印。
        */
    }
}

