//触摸屏
#include "cst816s.h"
#include "systick.h"

/* ============================================================
   开发板实际连接：
   SCL  -> PA8
   SDA  -> PC9
   RST  -> PD5
   INT  -> PD1

   软件 IIC：
   SCL/SDA 使用开漏输出 + 上拉
   GPIO_SetBits()  表示释放 SDA/SCL
   GPIO_ResetBits() 表示拉低 SDA/SCL
   ============================================================ */

/* ================= GPIO 定义 ================= */
#define CST816S_SCL_PORT      GPIOA
#define CST816S_SCL_PIN       GPIO_Pin_8
#define CST816S_SCL_CLK       RCC_AHB1Periph_GPIOA

#define CST816S_SDA_PORT      GPIOC
#define CST816S_SDA_PIN       GPIO_Pin_9
#define CST816S_SDA_CLK       RCC_AHB1Periph_GPIOC

#define CST816S_RST_PORT      GPIOD
#define CST816S_RST_PIN       GPIO_Pin_5
#define CST816S_RST_CLK       RCC_AHB1Periph_GPIOD

#define CST816S_INT_PORT      GPIOD
#define CST816S_INT_PIN       GPIO_Pin_1
#define CST816S_INT_CLK       RCC_AHB1Periph_GPIOD

/* ================= 软件 IIC 宏 ================= */
#define CST816S_SCL_L()       GPIO_ResetBits(CST816S_SCL_PORT, CST816S_SCL_PIN)
#define CST816S_SCL_RELEASE() GPIO_SetBits(CST816S_SCL_PORT, CST816S_SCL_PIN)

#define CST816S_SDA_L()       GPIO_ResetBits(CST816S_SDA_PORT, CST816S_SDA_PIN)
#define CST816S_SDA_RELEASE() GPIO_SetBits(CST816S_SDA_PORT, CST816S_SDA_PIN)

#define CST816S_SDA_READ()    GPIO_ReadInputDataBit(CST816S_SDA_PORT, CST816S_SDA_PIN)

#define CST816S_RST_L()       GPIO_ResetBits(CST816S_RST_PORT, CST816S_RST_PIN)
#define CST816S_RST_H()       GPIO_SetBits(CST816S_RST_PORT, CST816S_RST_PIN)

/* IIC 半周期延时，约 100kHz 以内，稳定优先 */
#define CST816S_IIC_DELAY_US  5

static void CST816S_IIC_Delay(void)
{
    delay_us(CST816S_IIC_DELAY_US);
}

/* ================= 底层 IIC ================= */

static void CST816S_IIC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(CST816S_SCL_CLK | CST816S_SDA_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = CST816S_SCL_PIN;
    GPIO_Init(CST816S_SCL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = CST816S_SDA_PIN;
    GPIO_Init(CST816S_SDA_PORT, &GPIO_InitStructure);

    CST816S_SCL_RELEASE();
    CST816S_SDA_RELEASE();
    CST816S_IIC_Delay();
}

static void CST816S_RST_INT_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(CST816S_RST_CLK | CST816S_INT_CLK, ENABLE);

    /* RST：推挽输出 */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   = CST816S_RST_PIN;
    GPIO_Init(CST816S_RST_PORT, &GPIO_InitStructure);

    /* INT：上拉输入 */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin   = CST816S_INT_PIN;
    GPIO_Init(CST816S_INT_PORT, &GPIO_InitStructure);
}

static void CST816S_IIC_Start(void)
{
    CST816S_SDA_RELEASE();
    CST816S_SCL_RELEASE();
    CST816S_IIC_Delay();

    CST816S_SDA_L();
    CST816S_IIC_Delay();

    CST816S_SCL_L();
    CST816S_IIC_Delay();
}

static void CST816S_IIC_Stop(void)
{
    CST816S_SCL_L();
    CST816S_SDA_L();
    CST816S_IIC_Delay();

    CST816S_SCL_RELEASE();
    CST816S_IIC_Delay();

    CST816S_SDA_RELEASE();
    CST816S_IIC_Delay();
}

static void CST816S_IIC_SendByte(u8 data)
{
    u8 i;

    for(i = 0; i < 8; i++)
    {
        CST816S_SCL_L();

        if(data & 0x80)
        {
            CST816S_SDA_RELEASE();
        }
        else
        {
            CST816S_SDA_L();
        }

        CST816S_IIC_Delay();

        CST816S_SCL_RELEASE();
        CST816S_IIC_Delay();

        data <<= 1;
    }

    CST816S_SCL_L();
    CST816S_IIC_Delay();
}

/*
返回值：
0：收到 ACK
1：收到 NACK
*/
static u8 CST816S_IIC_WaitAck(void)
{
    u8 ack;

    CST816S_SCL_L();

    /* 关键：释放 SDA，让从机可以拉低 ACK */
    CST816S_SDA_RELEASE();
    CST816S_IIC_Delay();

    CST816S_SCL_RELEASE();
    CST816S_IIC_Delay();

    if(CST816S_SDA_READ() == Bit_SET)
    {
        ack = 1;    /* NACK */
    }
    else
    {
        ack = 0;    /* ACK */
    }

    CST816S_SCL_L();
    CST816S_IIC_Delay();

    return ack;
}

/*
ack = 0：发送 ACK
ack = 1：发送 NACK
*/
static void CST816S_IIC_SendAck(u8 ack)
{
    CST816S_SCL_L();

    if(ack)
    {
        CST816S_SDA_RELEASE();  /* NACK */
    }
    else
    {
        CST816S_SDA_L();        /* ACK */
    }

    CST816S_IIC_Delay();

    CST816S_SCL_RELEASE();
    CST816S_IIC_Delay();

    CST816S_SCL_L();
    CST816S_SDA_RELEASE();
    CST816S_IIC_Delay();
}

static u8 CST816S_IIC_ReadByte(void)
{
    u8 i;
    u8 data = 0;

    CST816S_SDA_RELEASE();

    for(i = 0; i < 8; i++)
    {
        data <<= 1;

        CST816S_SCL_L();
        CST816S_IIC_Delay();

        CST816S_SCL_RELEASE();
        CST816S_IIC_Delay();

        if(CST816S_SDA_READ() == Bit_SET)
        {
            data |= 0x01;
        }
    }

    CST816S_SCL_L();
    CST816S_IIC_Delay();

    return data;
}

/* 可选：总线恢复，防止 SDA 被异常拉低 */
static void CST816S_IIC_BusRecover(void)
{
    u8 i;

    CST816S_SDA_RELEASE();
    CST816S_SCL_RELEASE();
    CST816S_IIC_Delay();

    for(i = 0; i < 9; i++)
    {
        CST816S_SCL_L();
        CST816S_IIC_Delay();

        CST816S_SCL_RELEASE();
        CST816S_IIC_Delay();
    }

    CST816S_IIC_Stop();
}

/* ================= 寄存器读写 ================= */

u8 CST816S_WriteReg(u8 reg, u8 data)
{
    CST816S_IIC_Start();

    CST816S_IIC_SendByte(CST816S_ADDR_WRITE);
    if(CST816S_IIC_WaitAck())
    {
        CST816S_IIC_Stop();
        return 0;
    }

    CST816S_IIC_SendByte(reg);
    if(CST816S_IIC_WaitAck())
    {
        CST816S_IIC_Stop();
        return 0;
    }

    CST816S_IIC_SendByte(data);
    if(CST816S_IIC_WaitAck())
    {
        CST816S_IIC_Stop();
        return 0;
    }

    CST816S_IIC_Stop();
    return 1;
}

u8 CST816S_ReadRegs(u8 reg, u8 *buf, u8 len)
{
    u8 i;

    if(buf == 0 || len == 0)
    {
        return 0;
    }

    CST816S_IIC_Start();

    CST816S_IIC_SendByte(CST816S_ADDR_WRITE);
    if(CST816S_IIC_WaitAck())
    {
        CST816S_IIC_Stop();
        return 0;
    }

    CST816S_IIC_SendByte(reg);
    if(CST816S_IIC_WaitAck())
    {
        CST816S_IIC_Stop();
        return 0;
    }

    CST816S_IIC_Start();

    CST816S_IIC_SendByte(CST816S_ADDR_READ);
    if(CST816S_IIC_WaitAck())
    {
        CST816S_IIC_Stop();
        return 0;
    }

    for(i = 0; i < len; i++)
    {
        buf[i] = CST816S_IIC_ReadByte();

        if(i < len - 1)
        {
            CST816S_IIC_SendAck(0);     /* ACK */
        }
        else
        {
            CST816S_IIC_SendAck(1);     /* 最后一个字节 NACK */
        }
    }

    CST816S_IIC_Stop();

    return 1;
}

u8 CST816S_ReadReg(u8 reg)
{
    u8 data = 0;

    if(CST816S_ReadRegs(reg, &data, 1))
    {
        return data;
    }

    return 0;
}

u8 CST816S_ReadID(void)
{
    return CST816S_ReadReg(CST816S_REG_CHIP_ID);
}

u8 CST816S_ReadINT(void)
{
    return GPIO_ReadInputDataBit(CST816S_INT_PORT, CST816S_INT_PIN);
}

/* ================= CST816S 初始化 ================= */

void CST816S_init(void)
{
    CST816S_IIC_GPIO_Init();
    CST816S_RST_INT_GPIO_Init();

    CST816S_IIC_BusRecover();

    CST816S_RST_L();
    delay_ms(20);

    CST816S_RST_H();
    delay_ms(120);

    /*
       关闭自动休眠：
       0xFE = 0：允许自动进入低功耗
       0xFE != 0：禁止自动进入低功耗
       调试触摸时建议关闭自动休眠
    */
    CST816S_WriteReg(CST816S_REG_DIS_AUTO_SLEEP, 0x01);
}

/* ================= 触摸读取 ================= */

int CST816S_ReadTouch(CST816S_Info_t *info)
{
    u8 buf[6];

    if(info == 0)
    {
        return 0;
    }

    info->gesture = 0;
    info->finger  = 0;
    info->x = 0;
    info->y = 0;

    /*
       从 0x01 连续读取 6 字节：
       buf[0] = GestureID
       buf[1] = FingerNum
       buf[2] = XposH
       buf[3] = XposL
       buf[4] = YposH
       buf[5] = YposL
    */
    if(CST816S_ReadRegs(CST816S_REG_GESTURE_ID, buf, 6) == 0)
    {
        return 0;
    }

    info->gesture = buf[0];
    info->finger  = buf[1];

    if(info->finger == 0)
    {
        return 0;
    }

    info->x = ((u16)(buf[2] & 0x0F) << 8) | buf[3];
    info->y = ((u16)(buf[4] & 0x0F) << 8) | buf[5];

    return 1;
}

int CST816S_read_xyvalue(u16 *x, u16 *y)
{
    CST816S_Info_t info;

    if(x == 0 || y == 0)
    {
        return 0;
    }

    if(CST816S_ReadTouch(&info) == 0)
    {
        return 0;
    }

    *x = info.x;
    *y = info.y;

    return 1;
}

