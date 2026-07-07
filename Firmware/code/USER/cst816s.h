#ifndef __CST816S_H
#define __CST816S_H

#include "stm32f4xx.h"

/* ================= CST816S IIC 地址 ================= */
#define CST816S_ADDR_7BIT        0x15
#define CST816S_ADDR_WRITE       0x2A
#define CST816S_ADDR_READ        0x2B

/* ================= CST816S 寄存器 ================= */
#define CST816S_REG_GESTURE_ID   0x01
#define CST816S_REG_FINGER_NUM   0x02
#define CST816S_REG_XPOS_H       0x03
#define CST816S_REG_XPOS_L       0x04
#define CST816S_REG_YPOS_H       0x05
#define CST816S_REG_YPOS_L       0x06

#define CST816S_REG_CHIP_ID      0xA7
#define CST816S_REG_PROJ_ID      0xA8
#define CST816S_REG_FW_VER       0xA9

#define CST816S_REG_MOTION_MASK  0xEC
#define CST816S_REG_IRQ_CTL      0xFA
#define CST816S_REG_IO_CTL       0xFD
#define CST816S_REG_DIS_AUTO_SLEEP  0xFE

/* ================= 手势值 ================= */
#define CST816S_GESTURE_NONE       0x00
#define CST816S_GESTURE_UP         0x01
#define CST816S_GESTURE_DOWN       0x02
#define CST816S_GESTURE_LEFT       0x03
#define CST816S_GESTURE_RIGHT      0x04
#define CST816S_GESTURE_CLICK      0x05
#define CST816S_GESTURE_DOUBLE     0x0B
#define CST816S_GESTURE_LONG_PRESS 0x0C

typedef struct
{
    u8 gesture;
    u8 finger;
    u16 x;
    u16 y;
} CST816S_Info_t;

/* ================= 对外接口 ================= */
void CST816S_init(void);

u8 CST816S_ReadID(void);
u8 CST816S_ReadReg(u8 reg);
u8 CST816S_WriteReg(u8 reg, u8 data);
u8 CST816S_ReadRegs(u8 reg, u8 *buf, u8 len);

/*
返回值：
0：无触摸或读取失败
1：有触摸，x/y 有效
*/
int CST816S_read_xyvalue(u16 *x, u16 *y);

/*
返回值：
0：无触摸或读取失败
1：有触摸，info 有效
*/
int CST816S_ReadTouch(CST816S_Info_t *info);

/*
读取 INT 引脚电平：
0：低电平
1：高电平
*/
u8 CST816S_ReadINT(void);

#endif

