#include "MQTT.h"
#include "wifi.h"
#include "systick.h"
#include "WS2812.h"
#include "tim.h"
#include "main.h"

#define delay_ms(t)		delay_ms(t)
extern WIFI_RECV WIFI_Rec;

u8 txbuf[256]={0};
u8 rxbuf[256]={0};


//连接成功服务器回应 20 02 00 00
//客户端主动断开连接 e0 00
const u8 parket_connetAck[] = {0x20,0x02,0x00,0x00};//由于在接收字符时，当我接收到'\0'就转变成' '来存储，所以接收到的回应则是0x20 02 0x20 0x20
const u8 parket_disconnet[] = {0xe0,0x00};
const u8 parket_subAck[] = {0x90,0x03};
const u8 parket_heart[] = {0xc0,0x00};
const u8 parket_heart_reply[] = {0xd0,0x00};//由于在接收字符时，当我接收到'\0'就转变成' '来存储，所以接收到的回应则是0xd0 0x20


/*
****************************************************************************************
* LOCAL FUNCTIONS DECLARE (静态函数声明)
****************************************************************************************
*/
static void Init(u8 *prx,u16 rxlen,u8 *ptx,u16 txlen);
static u8 Connect(char *ClientID,char *Username,char *Password);
static void Disconnect(void);
static u8 SubscribeTopic(char *topic,u8 qos,u8 whether);
static u8 PublishData(char *topic, char *message, u8 qos);
static void SentHeart(void);
static void SendData(u8* p,u16 len);

/*
****************************************************************************************
* LOCAL FUNCTIONS (静态函数)
****************************************************************************************
*/
_typdef_mqtt _mqtt = 
{
	0,0,
	0,0,
	Init,
	Connect,
	Disconnect,
	SubscribeTopic,
	PublishData,
	SentHeart,
	SendData,
};

static u8 BYTE1(int num)
{
	return (u8)((num&0xFF00)>>8);
}

static u8 BYTE0(int num)
{
	return (u8)(num&0xFF);
}

/*
****************************************************************************************
* Function: Init
* Description: MQTT初始化
* Input: None
* Output: None
* Return: None
* Author: weihaoMo
****************************************************************************************
*/
static void Init(u8 *prx,u16 rxlen,u8 *ptx,u16 txlen)
{
	_mqtt.rxbuf = prx;_mqtt.rxlen = rxlen;
	_mqtt.txbuf = ptx;_mqtt.txlen = txlen;
	
//	memset(_mqtt.rxbuf,0,_mqtt.rxlen);
//	memset(_mqtt.txbuf,0,_mqtt.txlen);
//	
//	//无条件先主动断开,发送4次，保证能断开
//	_mqtt.Disconnect();delay_ms(100);
//	_mqtt.Disconnect();delay_ms(100);
//	_mqtt.Disconnect();delay_ms(100);
//	_mqtt.Disconnect();delay_ms(100);
}

/*
****************************************************************************************
* Function: Connect
* Description: 连接服务器的打包函数
* Input: ClientID：客户端标识符
				 Username：用户名
			   Password：密码
* Output: None
* Return: 1成功 0失败
****************************************************************************************
*/ static uint8_t Connect(char *ClientID,char *Username,char *Password)
{
    int ClientIDLen = strlen(ClientID);
    int UsernameLen = strlen(Username);
    int PasswordLen = strlen(Password);
    int DataLen;
    u16 time = 0;
    char *p;

    _mqtt.txlen = 0;

    DataLen = 10 + (ClientIDLen + 2) + (UsernameLen + 2) + (PasswordLen + 2);

    _mqtt.txbuf[_mqtt.txlen++] = 0x10;

    do
    {
        u8 encodedByte = DataLen % 128;
        DataLen = DataLen / 128;

        if (DataLen > 0)
        {
            encodedByte = encodedByte | 128;
        }

        _mqtt.txbuf[_mqtt.txlen++] = encodedByte;

    } while (DataLen > 0);

    _mqtt.txbuf[_mqtt.txlen++] = 0x00;
    _mqtt.txbuf[_mqtt.txlen++] = 0x04;
    _mqtt.txbuf[_mqtt.txlen++] = 'M';
    _mqtt.txbuf[_mqtt.txlen++] = 'Q';
    _mqtt.txbuf[_mqtt.txlen++] = 'T';
    _mqtt.txbuf[_mqtt.txlen++] = 'T';

    _mqtt.txbuf[_mqtt.txlen++] = 0x04;
    _mqtt.txbuf[_mqtt.txlen++] = 0xC2;

    _mqtt.txbuf[_mqtt.txlen++] = 0x00;
    _mqtt.txbuf[_mqtt.txlen++] = 0x3C;

    _mqtt.txbuf[_mqtt.txlen++] = BYTE1(ClientIDLen);
    _mqtt.txbuf[_mqtt.txlen++] = BYTE0(ClientIDLen);
    memcpy(&_mqtt.txbuf[_mqtt.txlen], ClientID, ClientIDLen);
    _mqtt.txlen += ClientIDLen;

    _mqtt.txbuf[_mqtt.txlen++] = BYTE1(UsernameLen);
    _mqtt.txbuf[_mqtt.txlen++] = BYTE0(UsernameLen);
    memcpy(&_mqtt.txbuf[_mqtt.txlen], Username, UsernameLen);
    _mqtt.txlen += UsernameLen;

    _mqtt.txbuf[_mqtt.txlen++] = BYTE1(PasswordLen);
    _mqtt.txbuf[_mqtt.txlen++] = BYTE0(PasswordLen);
    memcpy(&_mqtt.txbuf[_mqtt.txlen], Password, PasswordLen);
    _mqtt.txlen += PasswordLen;

    WIFI_Rec_Clear();

    _mqtt.SendData(_mqtt.txbuf, _mqtt.txlen);

    time = 0;

    while (time < 5000)
    {
        delay_ms(1);
        time++;

        if (WIFI_Rec.flag)
        {
            printf("\r\nMQTT RX LEN=%d\r\n", WIFI_Rec.len);
            printf("%s\r\n", WIFI_Rec.buf);

            p = strstr(WIFI_Rec.buf, "+IPD");

            if (p != NULL)
            {
                p = strchr(p, ':');

                if (p != NULL)
                {
                    p++;

                    printf("MQTT IPD DATA: %02X %02X %02X %02X\r\n",
                           (u8)p[0],
                           (u8)p[1],
                           (u8)p[2],
                           (u8)p[3]);

                    if ((u8)p[0] == 0x20 && (u8)p[1] == 0x02)
                    {
                        WIFI_Rec_Clear();
                        return 1;
                    }
                }
            }

            WIFI_Rec_Clear();
        }
    }

    return 0;
}


/*
****************************************************************************************
* Function: SubscribeTopic
* Description: MQTT订阅/取消订阅数据打包并发送
* Input: topic       主题 
				 qos         消息等级 
				 whether     1订阅/0取消订阅请求包
* Output: None
* Return: 1成功 0失败
* Author: weihaoMo
****************************************************************************************
*/
static u8 SubscribeTopic(char *topic,u8 qos,u8 whether)
{
    int topiclen = strlen(topic);
    int DataLen;
    u16 time = 0;
    char *p;

    _mqtt.txlen = 0;

    DataLen = 2 + (topiclen + 2) + (whether ? 1 : 0);

    if (whether)
        _mqtt.txbuf[_mqtt.txlen++] = 0x82;
    else
        _mqtt.txbuf[_mqtt.txlen++] = 0xA2;

    do
    {
        u8 encodedByte = DataLen % 128;
        DataLen = DataLen / 128;

        if (DataLen > 0)
            encodedByte = encodedByte | 128;

        _mqtt.txbuf[_mqtt.txlen++] = encodedByte;

    } while (DataLen > 0);

    _mqtt.txbuf[_mqtt.txlen++] = 0x00;
    _mqtt.txbuf[_mqtt.txlen++] = 0x01;

    _mqtt.txbuf[_mqtt.txlen++] = BYTE1(topiclen);
    _mqtt.txbuf[_mqtt.txlen++] = BYTE0(topiclen);

    memcpy(&_mqtt.txbuf[_mqtt.txlen], topic, topiclen);
    _mqtt.txlen += topiclen;

    if (whether)
    {
        _mqtt.txbuf[_mqtt.txlen++] = qos;
    }

    printf("\r\nSUB LEN=%d\r\n", _mqtt.txlen);

    WIFI_Rec_Clear();

    _mqtt.SendData(_mqtt.txbuf, _mqtt.txlen);

    time = 0;

    while (time < 5000)
    {
        delay_ms(1);
        time++;

        if (WIFI_Rec.flag)
        {
            printf("\r\nSUB RX LEN=%d\r\n", WIFI_Rec.len);
            printf("%s\r\n", WIFI_Rec.buf);

            p = strstr(WIFI_Rec.buf, "+IPD");

            if (p != NULL)
            {
                p = strchr(p, ':');

                if (p != NULL)
                {
                    p++;

                    printf("SUB IPD DATA: %02X %02X %02X %02X %02X\r\n",
                           (u8)p[0],
                           (u8)p[1],
                           (u8)p[2],
                           (u8)p[3],
                           (u8)p[4]);

                    if ((u8)p[0] == 0x90 && (u8)p[1] == 0x03)
                    {
                        WIFI_Rec_Clear();
                        return 1;
                    }
                }
            }

            WIFI_Rec_Clear();
        }
    }

    return 0;
}

/*
****************************************************************************************
* Function: PublishData
* Description: MQTT发布数据打包并发送
* Input: topic   主题 
				 message 消息
				 qos     消息等级 
* Output: None
* Return: 数据包长度
****************************************************************************************
*/
static uint8_t PublishData(char *topic, char *message, uint8_t qos)
{  
    int topicLength = strlen(topic);    
    int messageLength = strlen(message);     
    static u16 id=0;
	int DataLen;
	_mqtt.txlen=0;
	//有效载荷的长度这样计算：用固定报头中的剩余长度字段的值减去可变报头的长度
	//QOS为0时没有标识符
	//数据长度             主题名   报文标识符   有效载荷
    if(qos)	DataLen = (2+topicLength) + 2 + messageLength;       
    else	DataLen = (2+topicLength) + messageLength;   
 
    //固定报头
	//控制报文类型
    _mqtt.txbuf[_mqtt.txlen++] = 0x30;    // MQTT Message Type PUBLISH  
 
	//剩余长度
	do
	{
		u8 encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		_mqtt.txbuf[_mqtt.txlen++] = encodedByte;
	}while ( DataLen > 0 );	
	
    _mqtt.txbuf[_mqtt.txlen++] = BYTE1(topicLength);//主题长度MSB
    _mqtt.txbuf[_mqtt.txlen++] = BYTE0(topicLength);//主题长度LSB 
		memcpy(&_mqtt.txbuf[_mqtt.txlen],topic,topicLength);//拷贝主题
    _mqtt.txlen += topicLength;
        
	//报文标识符
    if(qos)
    {
        _mqtt.txbuf[_mqtt.txlen++] = BYTE1(id);
        _mqtt.txbuf[_mqtt.txlen++] = BYTE0(id);
        id++;
    }
		memcpy(&_mqtt.txbuf[_mqtt.txlen],message,messageLength);
    _mqtt.txlen += messageLength;
        
	_mqtt.SendData(_mqtt.txbuf,_mqtt.txlen);
    return _mqtt.txlen;
}

/*
****************************************************************************************
* Function: SentHeart
* Description: 发送心跳
* Input: None
* Output: None
* Return: None
****************************************************************************************
*/
static void SentHeart(void)
{
	_mqtt.SendData((u8 *)parket_heart,sizeof(parket_heart));
}

/*
****************************************************************************************
* Function: Disconnect
* Description: 发送Disconnect报文
* Input: None
* Output: None
* Return: None
****************************************************************************************
*/
static void Disconnect(void)
{
	_mqtt.SendData((u8 *)parket_disconnet,sizeof(parket_disconnet));
}

/*
****************************************************************************************
* Function: SendData
* Description: 函数功能：MQTT数据包发送函数
* Input: p指向待发送的数据包，数据包数据长度
* Output: None
* Return: None
****************************************************************************************
*/
static void SendData(u8* p,u16 len)
{
    char cmd[32];
    u16 i;
    u16 time;

    sprintf(cmd, "AT+CIPSEND=%d\r\n", len);

    WIFI_Rec_Clear();
    USART2_SendStr(cmd);

    time = 0;

    while (time < 3000)
    {
        delay_ms(1);
        time++;

        if (WIFI_Rec.flag)
        {
            if (strstr(WIFI_Rec.buf, ">") != NULL)
            {
                break;
            }

            WIFI_Rec_Clear();
        }
    }

    WIFI_Rec_Clear();

    for (i = 0; i < len; i++)
    {
        while ((USART2->SR & USART_FLAG_TXE) == 0);
        USART2->DR = p[i];
    }
}
void MQTT_CheckRecv(void)
{
    char *p;

    if (WIFI_Rec.flag == 0)
    {
        return;
    }

    printf("\r\nMQTT RECV RAW:\r\n");
    printf("%s\r\n", WIFI_Rec.buf);

    p = strstr(WIFI_Rec.buf, "+IPD");

    if (p != NULL)
    {
        p = strchr(p, ':');

        if (p != NULL)
        {
            p++;

            printf("MQTT MESSAGE: %s\r\n", p);

            /******************** RGB灯开关 ********************/
            if (strstr(p, "\"rgb_switch\":true") != NULL)
            {
                WS2812_Show_Manual();
                printf("CMD: rgb_switch true -> RGB_ON\r\n");
            }
            else if (strstr(p, "\"rgb_switch\":false") != NULL)
            {
                WS2812_Off_Manual();
                printf("CMD: rgb_switch false -> RGB_OFF\r\n");
            }

            /******************** 风扇开关 ********************/
            else if (strstr(p, "\"fan_switch\":true") != NULL)
            {
                SetCompare_TIM3_CH3(900);
                printf("CMD: fan_switch true -> FAN_ON\r\n");
            }
            else if (strstr(p, "\"fan_switch\":false") != NULL)
            {
                SetCompare_TIM3_CH3(0);
                printf("CMD: fan_switch false -> FAN_OFF\r\n");
            }

            /******************** 舵机三个角度 ********************/
            else if (strstr(p, "\"servo_0\":true") != NULL)
            {
                servo_angle(0);
                printf("CMD: servo_0 true -> SERVO_0\r\n");
            }
            else if (strstr(p, "\"servo_90\":true") != NULL)
            {
                servo_angle(90);
                printf("CMD: servo_90 true -> SERVO_90\r\n");
            }
            else if (strstr(p, "\"servo_180\":true") != NULL)
            {
                servo_angle(180);
                printf("CMD: servo_180 true -> SERVO_180\r\n");
            }

            /******************** 兼容原来的字符串命令 ********************/
            else if (strstr(p, "RGB_ON") != NULL)
            {
                WS2812_Show_Manual();
                printf("CMD: RGB_ON\r\n");
            }
            else if (strstr(p, "RGB_OFF") != NULL)
            {
                WS2812_Off_Manual();
                printf("CMD: RGB_OFF\r\n");
            }
            else if (strstr(p, "FAN_ON") != NULL)
            {
                SetCompare_TIM3_CH3(900);
                printf("CMD: FAN_ON\r\n");
            }
            else if (strstr(p, "FAN_OFF") != NULL)
            {
                SetCompare_TIM3_CH3(0);
                printf("CMD: FAN_OFF\r\n");
            }
            else if (strstr(p, "SERVO_0") != NULL)
            {
                servo_angle(0);
                printf("CMD: SERVO_0\r\n");
            }
            else if (strstr(p, "SERVO_90") != NULL)
            {
                servo_angle(90);
                printf("CMD: SERVO_90\r\n");
            }
            else if (strstr(p, "SERVO_180") != NULL)
            {
                servo_angle(180);
                printf("CMD: SERVO_180\r\n");
            }

            /******************** 页面切换 ********************/
            else if (strstr(p, "SHT31_PAGE") != NULL)
            {
                page_cmd = PAGE_CMD_SHT31;
                printf("CMD: SHT31_PAGE\r\n");
            }
            else if (strstr(p, "MLX_PAGE") != NULL)
            {
                page_cmd = PAGE_CMD_MLX;
                printf("CMD: MLX_PAGE\r\n");
            }
            else if (strstr(p, "HOME_PAGE") != NULL)
            {
                page_cmd = PAGE_CMD_HOME;
                printf("CMD: HOME_PAGE\r\n");
            }
        }
    }

    WIFI_Rec_Clear();
}
/*
 * 函数名：ThingsCloud_Connect
 * 功能：连接 WiFi + ThingsCloud MQTT
 * 返回：1 成功，0 失败
 *
 * 重点：
 * 1. 这个函数失败也不会影响本地触摸/语音/按键功能。
 * 2. 只有返回 1 后，主循环才会执行 MQTT 心跳、云端接收、属性上传。
 */
u8 ThingsCloud_Connect(void)
{
    printf("\r\n========== ThingsCloud Connect ==========\r\n");

    if (WIFI_Init() != WIFI_OK)
    {
        printf("WIFI Init FAIL\r\n");
        return 0;
    }

  if (WIFI_ConnectAP("YOUR_WIFI_SSID",
                   "YOUR_WIFI_PASSWORD") != WIFI_OK)
    WIFI_GetIP();

    if (WIFI_TCP_Test() != WIFI_OK)
    {
        printf("TCP Connect ThingsCloud FAIL\r\n");
        return 0;
    }

    WIFI_Rec_Clear();
    delay_ms(1000);

    _mqtt.Init(rxbuf, 256, txbuf, 256);

    if (_mqtt.Connect("YOUR_MQTT_CLIENT_ID",
                  "YOUR_MQTT_USERNAME",
                  "YOUR_MQTT_PASSWORD") == 0)
    {
        printf("MQTT Connect FAIL\r\n");
        return 0;
    }

    printf("MQTT Connect OK\r\n");

    if (_mqtt.SubscribeTopic("attributes/push", 0, 1) == 0)
    {
        printf("Subscribe FAIL\r\n");
        return 0;
    }

    printf("Subscribe OK\r\n");

    delay_ms(1000);

    /*
     * 上电初始化一次云端属性。
     * 作用：
     * 1. ThingsCloud 属性页生成这些字段。
     * 2. 看板可以绑定这些字段。
     */
    _mqtt.PublishData(
        "attributes",
        "{\"temperature\":25.6,\"humidity\":60.2,\"body_temp\":36.5,"
        "\"rgb_switch\":false,"
        "\"fan_switch\":false,"
        "\"servo_0\":false,"
        "\"servo_90\":false,"
        "\"servo_180\":false}",
        0);

    printf("Publish Attributes OK\r\n");

    delay_ms(1000);
    WIFI_Rec_Clear();

    printf("MQTT Ready To Receive CMD\r\n");

    return 1;
}
