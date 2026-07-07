#include "W25Q64.h"

void W25Q64_SPI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(W25Q64_CS_RCC | W25Q64_SCK_RCC | W25Q64_MISO_RCC | W25Q64_MOSI_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
    GPIO_InitStructure.GPIO_Pin = W25Q64_CS_Pin;
    GPIO_Init(W25Q64_CS_GPIO, &GPIO_InitStructure);
    W25Q64_CS_H;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
    GPIO_InitStructure.GPIO_Pin = W25Q64_SCK_Pin | W25Q64_MISO_Pin | W25Q64_MOSI_Pin;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

    RCC_APB2PeriphClockCmd(W25Q64_SPI_RCC, ENABLE);
    SPI_I2S_DeInit(W25Q64_SPI);
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(W25Q64_SPI, &SPI_InitStructure);
    SPI_Cmd(W25Q64_SPI, ENABLE);
}

u8 W25Q64_Send_Recv_Byte(u8 data)
{
    while (SPI_I2S_GetFlagStatus(W25Q64_SPI, SPI_I2S_FLAG_TXE) != SET);
    SPI_I2S_SendData(W25Q64_SPI, data);
    while (SPI_I2S_GetFlagStatus(W25Q64_SPI, SPI_I2S_FLAG_RXNE) != SET);
    return (u8)SPI_I2S_ReceiveData(W25Q64_SPI);
}

void W25Q64_Write_Enable(void)
{
    W25Q64_CS_L;
    W25Q64_Send_Recv_Byte(0x06);
    W25Q64_CS_H;
}

u8 W25Q64_Read_Status(void)
{
    u8 status;

    W25Q64_CS_L;
    W25Q64_Send_Recv_Byte(0x05);
    status = W25Q64_Send_Recv_Byte(0x00);
    W25Q64_CS_H;

    return status;
}

u8 W25Q64_GET_FLAG_BUSY(void)
{
    return (W25Q64_Read_Status() & 0x01) ? 1 : 0;
}

static void W25Q64_Send_Addr(u32 addr)
{
    W25Q64_Send_Recv_Byte((u8)((addr & 0x00FF0000) >> 16));
    W25Q64_Send_Recv_Byte((u8)((addr & 0x0000FF00) >> 8));
    W25Q64_Send_Recv_Byte((u8)(addr & 0x000000FF));
}

void W25Q64_Sector_Erase(u32 addr)
{
    W25Q64_Write_Enable();
    W25Q64_CS_L;
    W25Q64_Send_Recv_Byte(0x20);
    W25Q64_Send_Addr(addr);
    W25Q64_CS_H;
    while (W25Q64_GET_FLAG_BUSY());
}

void W25Q64_Block_Erase(u32 addr)
{
    W25Q64_Write_Enable();
    W25Q64_CS_L;
    W25Q64_Send_Recv_Byte(0xD8);
    W25Q64_Send_Addr(addr);
    W25Q64_CS_H;
    while (W25Q64_GET_FLAG_BUSY());
}

void W25Q64_Chip_Erase(void)
{
    W25Q64_Write_Enable();
    W25Q64_CS_L;
    W25Q64_Send_Recv_Byte(0xC7);
    W25Q64_CS_H;
    while (W25Q64_GET_FLAG_BUSY());
}

void W25Q64_Read_Data(u32 addr, u8 *buf, int length)
{
    int i;

    W25Q64_CS_L;
    W25Q64_Send_Recv_Byte(0x03);
    W25Q64_Send_Addr(addr);
    for (i = 0; i < length; i++)
    {
        buf[i] = W25Q64_Send_Recv_Byte(0x00);
    }
    W25Q64_CS_H;
}

void W25Q64_Page_Program(u32 addr, u8 *buf, int length)
{
    int i;

    W25Q64_Write_Enable();
    W25Q64_CS_L;
    W25Q64_Send_Recv_Byte(0x02);
    W25Q64_Send_Addr(addr);
    for (i = 0; i < length; i++)
    {
        W25Q64_Send_Recv_Byte(buf[i]);
    }
    W25Q64_CS_H;
    while (W25Q64_GET_FLAG_BUSY());
}
