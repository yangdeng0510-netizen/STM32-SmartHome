#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "led_key.h"
#include "systick.h"
#include "uart.h"
#include "stdio.h"
#include "tim.h"
#include "adc.h"
#include "st7789.h"
#include "WS2812.h"
#include "HLK_V20.h"
#include "Humidity_Temperature .h"
#include "MLX90614.h"
#include "cst816s.h"
#include "MQTT.h"
#include "wifi.h"

/* Ò³Ãæ×´Ì¬ */
#define PAGE_HOME        0
#define PAGE_SHT31       1
#define PAGE_MLX90614    2

/* Ò³ÃæÇÐ»»ÇëÇó */
#define PAGE_CMD_NONE    0
#define PAGE_CMD_SHT31   1
#define PAGE_CMD_MLX     2
#define PAGE_CMD_HOME    3

extern volatile u8 page_cmd;
