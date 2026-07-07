# STM32 Smart Home Control System

基于 STM32F405VGT6 开发的智慧生活监测与控制系统，集成 LCD 触控、环境数据采集、人体温度检测、RGB 灯、舵机、风扇、离线语音和 MQTT 云端控制等功能。

## 项目简介

本项目以 STM32F405VGT6 为主控，通过 SPI、I2C、UART、PWM 和 DMA 等外设接口连接多种传感器和执行器。

系统支持本地触摸、按键、离线语音和 MQTT 云端等多种控制方式，可完成环境温湿度采集、人体温度检测、LCD 页面切换、RGB 灯控制、风扇控制和舵机角度控制。

## 技术栈

- MCU：STM32F405VGT6
- 开发语言：C
- 开发环境：Keil MDK
- 外设库：STM32 Standard Peripheral Library
- 通信接口：UART、I2C、SPI
- 控制接口：PWM、DMA
- 网络模块：ESP8266
- 通信协议：MQTT
- 云平台：ThingsCloud

## 硬件模块

- STM32F405VGT6
- ST7789 1.3 英寸 LCD
- CST816S 电容触摸芯片
- SHT31 温湿度传感器
- MLX90614 红外温度传感器
- WS2812B RGB 灯
- ESP8266 Wi-Fi 模块
- HLK-V20 离线语音模块
- 舵机
- 风扇
- 蜂鸣器

## 已实现功能

- ST7789 LCD 图形界面显示
- CST816S 触摸坐标读取与页面切换
- SHT31 环境温湿度采集
- MLX90614 人体温度检测
- WS2812B RGB 灯控制
- 舵机 0°、90°、180° 控制
- 风扇开关控制
- ESP8266 AT 指令通信
- MQTT 属性发布与命令订阅
- ThingsCloud 云端控制
- 离线语音控制
- 按键控制

## 项目目录

```text
STM32-SmartHome
├── Firmware
│   └── code
│       ├── APP
│       ├── CMSIS
│       ├── DebugConfig
│       ├── STM32F4xx_StdPeriph_Driver
│       ├── USER
│       └── myproject.uvprojx
├── docs
│   ├── 智慧生活系统_软件设计说明书.docx
│   └── 智慧生活系统_硬件设计说明书.docx
├── video
│   └── 项目演示视频
└── README.md
```
