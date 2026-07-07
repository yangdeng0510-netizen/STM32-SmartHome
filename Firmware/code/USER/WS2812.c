/************************************************************
 * 文件名：WS2812.c
 * 功能：WS2812B RGB灯 SPI2 + DMA 稳定驱动
 * 平台：STM32F405 + 标准外设库
 * 引脚：PB15 = SPI2_MOSI
 ************************************************************/

#include "WS2812.h"
#include "string.h"

uint8_t WS2812_Color[WS2812_LED_NUM][3];

static uint8_t WS2812_SPI_Buf[WS2812_SPI_BUF_LEN];


/************************************************************
 * 函数名：WS2812_SPI_BufWriteBit
 * 功能：向 SPI bit 缓冲区写入 1bit
 ************************************************************/
static void WS2812_SPI_BufWriteBit(uint16_t *bit_index, uint8_t bit)
{
    uint16_t byte_index;
    uint8_t bit_offset;

    byte_index = (*bit_index) / 8;
    bit_offset = 7 - ((*bit_index) % 8);

    if (bit)
    {
        WS2812_SPI_Buf[byte_index] |= (1 << bit_offset);
    }

    (*bit_index)++;
}


/************************************************************
 * 函数名：WS2812_EncodeOneBit
 * 功能：把一个 WS2812 bit 编码成 3 个 SPI bit
 *
 * WS2812 0码 -> 100
 * WS2812 1码 -> 110
 ************************************************************/
static void WS2812_EncodeOneBit(uint16_t *bit_index, uint8_t bit)
{
    if (bit)
    {
        WS2812_SPI_BufWriteBit(bit_index, 1);
        WS2812_SPI_BufWriteBit(bit_index, 1);
        WS2812_SPI_BufWriteBit(bit_index, 0);
    }
    else
    {
        WS2812_SPI_BufWriteBit(bit_index, 1);
        WS2812_SPI_BufWriteBit(bit_index, 0);
        WS2812_SPI_BufWriteBit(bit_index, 0);
    }
}


/************************************************************
 * 函数名：WS2812_EncodeByte
 * 功能：编码一个字节，高位先发
 ************************************************************/
static void WS2812_EncodeByte(uint16_t *bit_index, uint8_t data)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        if (data & 0x80)
        {
            WS2812_EncodeOneBit(bit_index, 1);
        }
        else
        {
            WS2812_EncodeOneBit(bit_index, 0);
        }

        data <<= 1;
    }
}


/************************************************************
 * 函数名：WS2812_MakeSPIBuffer
 * 功能：把 RGB 数据转换成 SPI DMA 发送缓冲区
 *
 * WS2812 数据顺序：G R B
 ************************************************************/
static void WS2812_MakeSPIBuffer(void)
{
    uint8_t i;
    uint16_t bit_index;

    memset(WS2812_SPI_Buf, 0, sizeof(WS2812_SPI_Buf));

    /*
     * 前面留 120 字节 0，形成复位低电平。
     */
    bit_index = WS2812_RESET_BYTES * 8;

    for (i = 0; i < WS2812_LED_NUM; i++)
    {
        WS2812_EncodeByte(&bit_index, WS2812_Color[i][1]);  /* G */
        WS2812_EncodeByte(&bit_index, WS2812_Color[i][0]);  /* R */
        WS2812_EncodeByte(&bit_index, WS2812_Color[i][2]);  /* B */
    }

    /*
     * 后面剩余 buffer 保持 0，作为帧结束复位低电平。
     */
}


/************************************************************
 * 函数名：WS2812_GPIO_Init_Manual
 * 功能：初始化 PB15 为 SPI2_MOSI，并配置 SPI2 + DMA
 *
 * 注意：
 * 为了兼容旧 main.c，函数名仍叫 Manual。
 * 实际内部已经改成 SPI2 + DMA。
 ************************************************************/
void WS2812_GPIO_Init_Manual(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(WS2812_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(WS2812_DMA_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(WS2812_SPI_CLK, ENABLE);

    /******************** PB15 -> SPI2_MOSI ********************/
    GPIO_PinAFConfig(WS2812_GPIO_PORT, WS2812_GPIO_PINSOURCE, WS2812_GPIO_AF);

    GPIO_InitStructure.GPIO_Pin = WS2812_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(WS2812_GPIO_PORT, &GPIO_InitStructure);

    /******************** SPI2 初始化 ********************/
    SPI_Cmd(WS2812_SPI, DISABLE);
    SPI_I2S_DeInit(WS2812_SPI);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(WS2812_SPI, &SPI_InitStructure);

    SPI_NSSInternalSoftwareConfig(WS2812_SPI, SPI_NSSInternalSoft_Set);

    /******************** DMA1 Stream4 Channel0 初始化 ********************/
    DMA_Cmd(WS2812_DMA_STREAM, DISABLE);
    while (DMA_GetCmdStatus(WS2812_DMA_STREAM) != DISABLE);

    DMA_DeInit(WS2812_DMA_STREAM);

    DMA_InitStructure.DMA_Channel = WS2812_DMA_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&WS2812_SPI->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)WS2812_SPI_Buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = WS2812_SPI_BUF_LEN;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(WS2812_DMA_STREAM, &DMA_InitStructure);

    SPI_I2S_DMACmd(WS2812_SPI, SPI_I2S_DMAReq_Tx, DISABLE);
    SPI_Cmd(WS2812_SPI, ENABLE);

    WS2812_Off_Manual();
}


/************************************************************
 * 函数名：WS2812_DWT_Init
 * 功能：兼容旧接口
 *
 * SPI+DMA 版本不再需要 DWT。
 ************************************************************/
void WS2812_DWT_Init(void)
{
    /*
     * 空函数。
     * 保留它是为了 main.c 不需要修改。
     */
}


/************************************************************
 * 函数名：WS2812_SetPixel
 * 功能：设置单颗 WS2812 颜色
 ************************************************************/
void WS2812_SetPixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if (index >= WS2812_LED_NUM)
    {
        return;
    }

    WS2812_Color[index][0] = r;
    WS2812_Color[index][1] = g;
    WS2812_Color[index][2] = b;
}


/************************************************************
 * 函数名：WS2812_SPI_DMA_Send
 * 功能：通过 SPI2 + DMA 发送整帧 WS2812 数据
 ************************************************************/
static void WS2812_SPI_DMA_Send(void)
{
    DMA_Cmd(WS2812_DMA_STREAM, DISABLE);
    while (DMA_GetCmdStatus(WS2812_DMA_STREAM) != DISABLE);

    DMA_ClearFlag(WS2812_DMA_STREAM,
                  DMA_FLAG_TCIF4 |
                  DMA_FLAG_HTIF4 |
                  DMA_FLAG_TEIF4 |
                  DMA_FLAG_DMEIF4 |
                  DMA_FLAG_FEIF4);

    WS2812_DMA_STREAM->M0AR = (uint32_t)WS2812_SPI_Buf;
    DMA_SetCurrDataCounter(WS2812_DMA_STREAM, WS2812_SPI_BUF_LEN);

    SPI_I2S_DMACmd(WS2812_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(WS2812_DMA_STREAM, ENABLE);

    while (DMA_GetFlagStatus(WS2812_DMA_STREAM, DMA_FLAG_TCIF4) == RESET);

    while (SPI_I2S_GetFlagStatus(WS2812_SPI, SPI_I2S_FLAG_TXE) == RESET);
    while (SPI_I2S_GetFlagStatus(WS2812_SPI, SPI_I2S_FLAG_BSY) == SET);

    DMA_Cmd(WS2812_DMA_STREAM, DISABLE);
    SPI_I2S_DMACmd(WS2812_SPI, SPI_I2S_DMAReq_Tx, DISABLE);

    DMA_ClearFlag(WS2812_DMA_STREAM,
                  DMA_FLAG_TCIF4 |
                  DMA_FLAG_HTIF4 |
                  DMA_FLAG_TEIF4 |
                  DMA_FLAG_DMEIF4 |
                  DMA_FLAG_FEIF4);
}


/************************************************************
 * 函数名：WS2812_Refresh
 * 功能：刷新所有灯
 ************************************************************/
void WS2812_Refresh(void)
{
    WS2812_MakeSPIBuffer();
    WS2812_SPI_DMA_Send();
}


/************************************************************
 * 函数名：WS2812_Show_Manual
 * 功能：兼容旧开灯函数
 ************************************************************/
void WS2812_Show_Manual(void)
{
    /*
     * 当前颜色：
     * 第1颗：粉色
     * 第2颗：绿色
     * 第3颗：蓝色
     * 第4颗：粉色
     */
    WS2812_SetPixel(0, 255, 192, 203);
    WS2812_SetPixel(1, 0,   255, 0);
    WS2812_SetPixel(2, 0,   0,   255);
    WS2812_SetPixel(3, 255, 192, 203);

    WS2812_Refresh();
}


/************************************************************
 * 函数名：WS2812_Off_Manual
 * 功能：兼容旧关灯函数
 ************************************************************/
void WS2812_Off_Manual(void)
{
    uint8_t i;

    for (i = 0; i < WS2812_LED_NUM; i++)
    {
        WS2812_SetPixel(i, 0, 0, 0);
    }

    WS2812_Refresh();
}

