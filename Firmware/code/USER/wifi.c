#include "wifi.h"



#define WIFI_SSID   "YOUR_WIFI_SSID"
#define WIFI_PWD    "YOUR_WIFI_PASSWORD"
void WIFI_TCP_Init(void)
{
	USART2_Init(115200);
	 WIFI_ExitTransmit();
	delay_ms(3000);
	WIFI_SendAT("AT\r\n","OK");
	delay_ms(3000);
	WIFI_SendAT("AT+RST\r\n","OK");
	delay_ms(3000);
	WIFI_SendAT("AT\r\n","OK");
	delay_ms(3000);
	WIFI_SendAT("AT+CWMODE=1\r\n","OK");
	delay_ms(3000);
	WIFI_ConnectAP(WIFI_SSID,WIFI_PWD);
	delay_ms(3000);
	WIFI_ConnectServer("TCP","sh-5-mqtt.iot-api.com","1883");
	delay_ms(3000);
	WIFI_SendAT("AT+CIPMODE=1\r\n","OK");
	//delay_ms(3000);
	u8 ret =WIFI_SendAT("AT+CIPSEND\r\n",">");
	if(!ret)
	{
	   _mqtt.Init(rxbuf,0,txbuf,0);	
	ret = _mqtt.Connect("WIFI_HOME",
                    "mrgvpn0tez6yj8d6",
                    "qbZguBUnYe");
		if(ret==1)
		{
			printf("MQTT接入成功！\r\n");
		}
		ret=_mqtt.SubscribeTopic("attributes/push",0,1);
		if(ret==1)
		{
			printf("MQTT订阅成功！\r\n");
		}
	}
}
void WIFI_Weather_Init(void)
{
	USART2_Init(115200);
	 WIFI_ExitTransmit();
	delay_ms(3000);
	WIFI_SendAT("AT\r\n","OK");
	delay_ms(3000);
	WIFI_SendAT("AT+RST\r\n","OK");
	delay_ms(3000);
	WIFI_SendAT("AT\r\n","OK");
	delay_ms(3000);
	WIFI_SendAT("AT+CWMODE=1\r\n","OK");
	delay_ms(3000);
	WIFI_ConnectAP(WIFI_SSID,WIFI_PWD);
	delay_ms(3000);
	WIFI_ConnectServer("TCP","api.seniverse.com","80");
	delay_ms(3000);
	WIFI_SendAT("AT+CIPMODE=1\r\n","OK");
	delay_ms(3000);
	WIFI_SendAT("AT+CIPSEND\r\n",">");
	delay_ms(3000);
	USART2_SendStr("GET https://api.seniverse.com/v3/weather/now.json?key=YOUR_WEATHER_API_KEY&location=nanjing&language=en&unit=c\r\n");
		delay_ms(1000);
		 WIFI_ExitTransmit();
}
/************************************************************
函数名：WIFI_SendAT
功能：向WIFI模块发送AT指令
参数：char* AT ,char *respond
返回值：char 
备注：
************************************************************/
char WIFI_SendAT(char* AT,char* respond)
{
	u16 time =0;
	USART2_SendStr(AT);
	while(!WIFI_Rec.flag)
	{
		delay_ms(1);
		time++;
		if(time>500)
		{
			printf("-----TIME OUT-------\r\n");
			return 1;
		}
	}
	WIFI_Rec.flag=0;
	if(strstr(WIFI_Rec.buf,respond)==NULL)
	{
		return 2;
	}
	return 0;
}


/************************************************************
函数名：WIFI_ConnectServer
功能：WIFI模块连接连接服务器
参数：char*type,char *RemoteIP,char *RemotePort
返回值：void
备注： "AT+CIPSTART=\"type\",\"remote_IP\",remote_Port\r\n"
************************************************************/
void WIFI_ConnectServer(char*type,char *RemoteIP,char *RemotePort)
{
	char buf[80]="AT+CIPSTART=";
	strcat(buf,"\"");
  strcat(buf,type);
	strcat(buf,"\",\"");
  strcat(buf,RemoteIP);
	strcat(buf,"\",");
	strcat(buf,RemotePort);
	strcat(buf,"\r\n");
	USART2_SendStr(buf);
	while(1)
	{
		if(WIFI_Rec.flag==1)
		{
			if(strstr(WIFI_Rec.buf,"CONNECT")!=NULL)
			{
				printf("---Server Connect Succeed!----\r\n");
				WIFI_Rec.flag=0;
				return;
			}
			if(strstr(WIFI_Rec.buf,"ERROR")!=NULL)
			{
				printf("---Server Connect Failed!----\r\n");
				WIFI_Rec.flag=0;
				return;
			}
			WIFI_Rec.flag=0;
		}
	}
		
	
	
}

/************************************************************
函数名：WIFI_ExitTransmit
功能：退出透传模式
参数：void
返回值：void
备注："+++"
************************************************************/
void WIFI_ExitTransmit(void)
{
	USART2_SendStr("+++");
	delay_ms(1000);
	WIFI_SendAT("AT+CIPCLOSE\r\n","OK");
	
}
/*
 * 发送AT指令并等待指定应答
 * 返回值：
 * 0：成功
 * 1：失败
 */
uint8_t WIFI_SendCmd(char *cmd, char *ack, uint32_t wait_ms)
{
    WIFI_Rec_Clear();

    USART2_SendStr(cmd);

    delay_ms(wait_ms);

    printf("%s", WIFI_Rec.buf);

    if (strstr(WIFI_Rec.buf, ack) != NULL)
    {
        return WIFI_OK;
    }
    else
    {
        return WIFI_ERROR;
    }
}
uint8_t WIFI_Init(void)
{
    uint8_t ret;

    USART2_Init(115200);
    delay_ms(1000);

    printf("\r\n========== WIFI INIT ==========\r\n");

    ret = WIFI_SendCmd("AT\r\n", "OK", 1000);
    if (ret != WIFI_OK)
    {
        printf("AT test failed\r\n");
        return WIFI_ERROR;
    }
    printf("AT test ok\r\n");

    ret = WIFI_SendCmd("AT+CWMODE=1\r\n", "OK", 1000);
    if (ret != WIFI_OK)
    {
        printf("Set CWMODE failed\r\n");
        return WIFI_ERROR;
    }
    printf("Set CWMODE ok\r\n");

    return WIFI_OK;
}
/*
 * 连接热点
 */
uint8_t WIFI_ConnectAP(char *ssid, char *password)
{
    char cmd[128];
    uint8_t ret;

    printf("\r\n========== WIFI CONNECT AP ==========\r\n");

    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);

    ret = WIFI_SendCmd(cmd, "WIFI GOT IP", 15000);

    if (ret != WIFI_OK)
    {
        printf("Connect AP failed\r\n");
        return WIFI_ERROR;
    }

    printf("Connect AP ok\r\n");
    return WIFI_OK;
}
/*
 * 查询IP地址
 */
uint8_t WIFI_GetIP(void)
{
    uint8_t ret;

    printf("\r\n========== WIFI GET IP ==========\r\n");

    ret = WIFI_SendCmd("AT+CIFSR\r\n", "STAIP", 2000);

    if (ret != WIFI_OK)
    {
        printf("Get IP failed\r\n");
        return WIFI_ERROR;
    }

    printf("Get IP ok\r\n");
    return WIFI_OK;
}
uint8_t WIFI_TCP_Test(void)
{
    uint8_t ret;

    printf("\r\n===== TCP TEST =====\r\n");

    ret = WIFI_SendCmd(
        "AT+CIPSTART=\"TCP\",\"sh-1-mqtt.iot-api.com\",1883\r\n",
        "CONNECT",
        1000);

    if(ret == WIFI_OK)
    {
        printf("TCP Connect OK\r\n");
        return WIFI_OK;
    }

    printf("TCP Connect FAIL\r\n");
    return WIFI_ERROR;
}

void WIFI_Check_Mode(void)
{
    WIFI_Rec_Clear();

    USART2_SendStr("AT+CIPMODE?\r\n");

    delay_ms(1000);

    printf("\r\n===== CIPMODE =====\r\n");
    printf("%s\r\n", WIFI_Rec.buf);
}
void WIFI_Echo_Off(void)
{
    WIFI_Rec_Clear();

    USART2_SendStr("ATE0\r\n");

    delay_ms(1000);

    printf("\r\n===== ATE0 =====\r\n");
    printf("%s\r\n", WIFI_Rec.buf);
}

