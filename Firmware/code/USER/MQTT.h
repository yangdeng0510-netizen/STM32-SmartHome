#ifndef _MQTT_H_
#define _MQTT_H_

#include "stm32f4xx.h"
#include "string.h"
#include "st7789.h"
#include "Humidity_Temperature .h"
#include "MLX90614.h"

typedef struct
{
	u8 *rxbuf;u16 rxlen;
	u8 *txbuf;u16 txlen;
	
	void (*Init)(u8 *prx,u16 rxlen,u8 *ptx,u16 txlen);
	u8 (*Connect)(char *ClientID,char *Username,char *Password);
	void (*Disconnect)(void);
	u8 (*SubscribeTopic)(char *topic,u8 qos,u8 whether);
	u8 (*PublishData)(char *topic, char *message, u8 qos);
	void (*SendHeart)(void);
	void (*SendData)(u8* p,u16 len);
}_typdef_mqtt;


extern u8 txbuf[256];
extern u8 rxbuf[256];
extern _typdef_mqtt _mqtt;

void MQTT_CheckRecv(void);
u8 ThingsCloud_Connect(void);

#endif


