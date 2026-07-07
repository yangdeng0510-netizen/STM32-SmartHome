#include "main.h"

/*
 * page_cmd 是全局页面切换请求变量。
 * 语音模块和 MQTT 模块都可以修改它。
 */
volatile u8 page_cmd = PAGE_CMD_NONE;





int main(void)
{
    /******************** 1. 局部变量定义 ********************/
    u16 x = 0;
    u16 y = 0;

    u8 touch_lock = 0;
    u8 touch_event = 0;

    /*
     * touch_flag 表示当前页面：
     * PAGE_HOME      ：主界面
     * PAGE_SHT31     ：环境温湿度页面
     * PAGE_MLX90614  ：人体温度页面
     */
    u8 touch_flag = PAGE_HOME;

    u8 sht31_cnt = 0;
    u8 mlx90614_cnt = 0;

    u16 mqtt_heart_cnt = 0;
    u16 mqtt_upload_cnt = 0;

    /*
     * mqtt_ok = 1：ThingsCloud 已连接，可以执行云端收发。
     * mqtt_ok = 0：云端不可用，本地触摸/语音/按键照常工作。
     */
    u8 mqtt_ok = 0;

    float temp = 0;
    float humi = 0;
    float humen_temp = 0;

    extern volatile uint8_t hlk_rx_flag;


    /******************** 2. 基础外设初始化 ********************/
    delay_ms(1000);

    WS2812_GPIO_Init_Manual();
    WS2812_DWT_Init();
    WS2812_LOW;

    KEY_Init();
    LED_Init();

    TIM3_CH3_PC8_Init();
    TIM5_CH2_PA1_init();

    uart1_init();
    SHT31_GPIO_Init();

    printf("STM32F405 LCD start\r\n");

    LCD_Init();
    CST816S_init();
    LCD_Demo();

    delay_ms(500);


    /******************** 3. 本地功能优先启动 ********************/
    /*
     * 关键修改：
     * HLK_Init() 提前到 WiFi/MQTT 前面。
     * 这样即使 ThingsCloud 连接失败，语音模块也已经初始化。
     */
    WS2812_Off_Manual();

    HLK_Init();

    printf("Local control ready\r\n");


    /******************** 4. WiFi + ThingsCloud 可选启动 ********************/
    /*
     * USART2 是 ESP8266 WiFi 模块用的。
     * 云端连接失败，不影响后续 while(1) 本地控制。
     */
    USART2_Init(115200);
    delay_ms(1000);

    mqtt_ok = ThingsCloud_Connect();

    if (mqtt_ok)
    {
        printf("Cloud online\r\n");
    }
    else
    {
        printf("Cloud offline, local control still enabled\r\n");
    }


    /******************** 5. 主循环 ********************/
    while (1)
    {
        /******************** 5.1 触摸读取，只生成一次点击事件 ********************/
        touch_event = 0;

        if (CST816S_read_xyvalue(&x, &y))
        {
            if (touch_lock == 0)
            {
                touch_lock = 1;
                touch_event = 1;

                printf("Touch: %d,%d\r\n", x, y);
            }
        }
        else
        {
            touch_lock = 0;
            x = 0;
            y = 0;
        }


        /******************** 5.2 语音命令处理 ********************/
        if (hlk_rx_flag)
        {
            hlk_rx_flag = 0;

            /* 打开风扇 */
            if (HLK_Cmd_Check(0X4F, 0X00, 0X02, 0XF4))
            {
                SetCompare_TIM3_CH3(900);
            }

            /* 关闭风扇 */
            else if (HLK_Cmd_Check(0X4F, 0X00, 0X01, 0XF4))
            {
                SetCompare_TIM3_CH3(0);
            }

            /* 打开灯 */
            else if (HLK_Cmd_Check(0X4F, 0X00, 0X25, 0XF4))
            {
                WS2812_Show_Manual();
            }

            /* 关闭灯 */
            else if (HLK_Cmd_Check(0X4F, 0X00, 0X26, 0XF4))
            {
                WS2812_Off_Manual();
            }

            /* 舵机转到 0 度 */
            else if (HLK_Cmd_Check(0X4F, 0X00, 0X03, 0XF4))
            {
                servo_angle(0);
            }

            /* 舵机转到 90 度 */
            else if (HLK_Cmd_Check(0X4F, 0X00, 0X04, 0XF4))
            {
                servo_angle(90);
            }

            /* 舵机转到 180 度 */
            else if (HLK_Cmd_Check(0X4F, 0X00, 0X05, 0XF4))
            {
                servo_angle(180);
            }

            /* 语音：测量环境温湿度 */
            else if (HLK_Cmd_Check(0X4F, 0X00, 0X06, 0XF4))
            {
                page_cmd = PAGE_CMD_SHT31;
            }

            /* 语音：测量人体温度 */
            else if (HLK_Cmd_Check(0X4F, 0X00, 0X07, 0XF4))
            {
                page_cmd = PAGE_CMD_MLX;
            }
        }


        /******************** 5.3 MQTT 接收、心跳、属性上传 ********************/
        /*
         * 关键修改：
         * 只有 mqtt_ok == 1 时才执行 MQTT。
         * 云端失败时，不再执行：
         * MQTT_CheckRecv()
         * _mqtt.SendHeart()
         * _mqtt.PublishData()
         *
         * 这样不会因为 ESP8266 / ThingsCloud 超时导致本地控制卡死。
         */
        if (mqtt_ok)
        {
            /*
             * 如果 ESP8266 收到 CLOSED，认为云端断线。
             * 之后停止 MQTT 心跳和上传，避免反复 CIPSEND 卡住。
             */
            if ((WIFI_Rec.flag) && (strstr(WIFI_Rec.buf, "CLOSED") != NULL))
            {
                printf("MQTT CLOSED, cloud offline\r\n");
                mqtt_ok = 0;
                WIFI_Rec_Clear();
            }
            else
            {
                MQTT_CheckRecv();
            }

            mqtt_heart_cnt++;

            /*
             * 主循环末尾 delay_ms(20)。
             * 1500 * 20ms = 30000ms = 30s。
             * 每 30 秒发送一次 MQTT 心跳。
             */
            if (mqtt_heart_cnt >= 1500)
            {
                mqtt_heart_cnt = 0;

                _mqtt.SendHeart();
                printf("MQTT Heart Send\r\n");
            }

            /*
             * 每 5 秒上传一次环境温湿度和人体温度。
             * 250 * 20ms = 5000ms。
             */
            mqtt_upload_cnt++;

            if (mqtt_upload_cnt >= 250)
            {
                char json_buf[128];

                int temp10;
                int humi10;
                int body10;

                mqtt_upload_cnt = 0;

                SHT31_ReadTempHumi(&temp, &humi);
                MLX90614_ReadHumen_Temp(&humen_temp);

                temp10 = (int)(temp * 10 + 0.5f);
                humi10 = (int)(humi * 10 + 0.5f);
                body10 = (int)(humen_temp * 10 + 0.5f);

                sprintf(json_buf,
                        "{\"temperature\":%d.%d,\"humidity\":%d.%d,\"body_temp\":%d.%d}",
                        temp10 / 10, temp10 % 10,
                        humi10 / 10, humi10 % 10,
                        body10 / 10, body10 % 10);

                _mqtt.PublishData("attributes", json_buf, 0);

                printf("Upload Attributes: %s\r\n", json_buf);
            }
        }


        /******************** 5.4 统一处理页面切换请求 ********************/
        /*
         * page_cmd 的来源：
         * 1. 语音命令
         * 2. MQTT 命令
         *
         * 统一在这里切页面，避免语音、MQTT、触摸各写一套页面逻辑。
         */
        if (page_cmd == PAGE_CMD_SHT31)
        {
            touch_flag = PAGE_SHT31;

            sht31_cnt = 0;
            mlx90614_cnt = 0;

            LCD_Clear(BLACK);
            LCD_SHT31_stastic();

            page_cmd = PAGE_CMD_NONE;
            touch_event = 0;
            x = 0;
            y = 0;
        }
        else if (page_cmd == PAGE_CMD_MLX)
        {
            touch_flag = PAGE_MLX90614;

            sht31_cnt = 0;
            mlx90614_cnt = 0;

            LCD_Clear(BLACK);
            MLX90614_Stastic();

            page_cmd = PAGE_CMD_NONE;
            touch_event = 0;
            x = 0;
            y = 0;
        }
        else if (page_cmd == PAGE_CMD_HOME)
        {
            touch_flag = PAGE_HOME;

            sht31_cnt = 0;
            mlx90614_cnt = 0;

            LCD_Clear(BLACK);
            LCD_Demo();

            page_cmd = PAGE_CMD_NONE;
            touch_event = 0;
            x = 0;
            y = 0;
        }


        /******************** 5.5 当前页面逻辑处理 ********************/
        if (touch_flag == PAGE_HOME)
        {
            /*
             * 主界面只处理一次新的触摸事件。
             * 这样长按不会反复触发，旧坐标也不会乱触发。
             */
            if (touch_event)
            {
                /***************** RGB 触摸控制 *******************/
                if ((y >= 180 && y <= 220) && (x > 0 && x < 120))
                {
                    printf("RGB ON touch trigger\r\n");

                    WS2812_Show_Manual();

                    x = 0;
                    y = 0;
                }
                else if ((y >= 180 && y <= 220) && (x > 120 && x < 239))
                {
                    printf("RGB OFF touch trigger\r\n");

                    WS2812_Off_Manual();

                    x = 0;
                    y = 0;
                }

                /***************** 舵机触摸控制 *******************/
                else if ((y >= 220 && y <= 280) && (x > 0 && x < 80))
                {
                    servo_angle(0);

                    x = 0;
                    y = 0;
                }
                else if ((y >= 220 && y <= 280) && (x > 80 && x < 160))
                {
                    servo_angle(90);

                    x = 0;
                    y = 0;
                }
                else if ((y >= 220 && y <= 280) && (x > 160 && x < 239))
                {
                    servo_angle(180);

                    x = 0;
                    y = 0;
                }

                /***************** 触摸进入环境温湿度页面 *******************/
                else if (y >= 60 && y <= 120)
                {
                    touch_flag = PAGE_SHT31;

                    sht31_cnt = 0;
                    mlx90614_cnt = 0;

                    LCD_Clear(BLACK);
                    LCD_SHT31_stastic();

                    x = 0;
                    y = 0;
                }

                /***************** 触摸进入人体温度页面 *******************/
                else if (y >= 130 && y <= 170)
                {
                    touch_flag = PAGE_MLX90614;

                    sht31_cnt = 0;
                    mlx90614_cnt = 0;

                    LCD_Clear(BLACK);
                    MLX90614_Stastic();

                    x = 0;
                    y = 0;
                }
            }
        }
        else if (touch_flag == PAGE_SHT31)
        {
            /***************** 环境温湿度页面动态刷新 *******************/
            sht31_cnt++;

            if (sht31_cnt >= 50)
            {
                sht31_cnt = 0;

                SHT31_ReadTempHumi(&temp, &humi);
                LCD_SHT31_UpdateValue(temp, humi);
            }

            /*
             * 触摸退出。
             * 使用 touch_event，避免旧坐标 y>=170 一直触发。
             */
            if (touch_event && y >= 170)
            {
                LCD_Clear(BLACK);
                LCD_Demo();

                touch_flag = PAGE_HOME;

                sht31_cnt = 0;
                mlx90614_cnt = 0;

                x = 0;
                y = 0;
            }
        }
        else if (touch_flag == PAGE_MLX90614)
        {
            /***************** 人体温度页面动态刷新 *******************/
            mlx90614_cnt++;

            if (mlx90614_cnt >= 50)
            {
                mlx90614_cnt = 0;

                MLX90614_ReadHumen_Temp(&humen_temp);
                MLX90614_UpdateValue(humen_temp);
            }

            /*
             * 触摸退出。
             * 使用 touch_event，避免旧坐标 y>=172 一直触发。
             */
            if (touch_event && y >= 172)
            {
                LCD_Clear(BLACK);
                LCD_Demo();

                touch_flag = PAGE_HOME;

                sht31_cnt = 0;
                mlx90614_cnt = 0;

                x = 0;
                y = 0;
            }
        }


        /******************** 5.6 独立按键逻辑 ********************/
        switch (KEY_Scan())
        {
            case 1:
                LED1_ON;
                LED2_OFF;
                LED3_OFF;

                servo_angle(0);
                SetCompare_TIM3_CH3(300);
                break;

            case 2:
                LED1_OFF;
                LED2_ON;
                LED3_OFF;

                servo_angle(90);
                SetCompare_TIM3_CH3(600);
                break;

            case 3:
                LED1_OFF;
                LED2_OFF;
                LED3_ON;

                servo_angle(180);
                SetCompare_TIM3_CH3(900);
                break;

            default:
                break;
        }

        delay_ms(20);
    }
}