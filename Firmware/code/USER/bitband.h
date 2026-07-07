#ifndef _BITBAND_H
#define _BITBAND_H

#include "stm32f4xx.h"

#define BIT_BAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + ((bitnum) << 2))
#define MEM_ADDR(addr, bitnum) *((volatile unsigned long *)BIT_BAND(addr, bitnum))

#define GPIOA_IDR_ADDR  (GPIOA_BASE + 0x10)
#define GPIOB_IDR_ADDR  (GPIOB_BASE + 0x10)
#define GPIOC_IDR_ADDR  (GPIOC_BASE + 0x10)
#define GPIOD_IDR_ADDR  (GPIOD_BASE + 0x10)
#define GPIOE_IDR_ADDR  (GPIOE_BASE + 0x10)
#define GPIOF_IDR_ADDR  (GPIOF_BASE + 0x10)
#define GPIOG_IDR_ADDR  (GPIOG_BASE + 0x10)

#define GPIOA_ODR_ADDR  (GPIOA_BASE + 0x14)
#define GPIOB_ODR_ADDR  (GPIOB_BASE + 0x14)
#define GPIOC_ODR_ADDR  (GPIOC_BASE + 0x14)
#define GPIOD_ODR_ADDR  (GPIOD_BASE + 0x14)
#define GPIOE_ODR_ADDR  (GPIOE_BASE + 0x14)
#define GPIOF_ODR_ADDR  (GPIOF_BASE + 0x14)
#define GPIOG_ODR_ADDR  (GPIOG_BASE + 0x14)

#define PAout(x)   MEM_ADDR(GPIOA_ODR_ADDR, x)
#define PAin(x)    MEM_ADDR(GPIOA_IDR_ADDR, x)
#define PBout(x)   MEM_ADDR(GPIOB_ODR_ADDR, x)
#define PBin(x)    MEM_ADDR(GPIOB_IDR_ADDR, x)
#define PCout(x)   MEM_ADDR(GPIOC_ODR_ADDR, x)
#define PCin(x)    MEM_ADDR(GPIOC_IDR_ADDR, x)
#define PDout(x)   MEM_ADDR(GPIOD_ODR_ADDR, x)
#define PDin(x)    MEM_ADDR(GPIOD_IDR_ADDR, x)
#define PEout(x)   MEM_ADDR(GPIOE_ODR_ADDR, x)
#define PEin(x)    MEM_ADDR(GPIOE_IDR_ADDR, x)
#define PFout(x)   MEM_ADDR(GPIOF_ODR_ADDR, x)
#define PFin(x)    MEM_ADDR(GPIOF_IDR_ADDR, x)
#define PGout(x)   MEM_ADDR(GPIOG_ODR_ADDR, x)
#define PGin(x)    MEM_ADDR(GPIOG_IDR_ADDR, x)

#endif
