#include "Humidity_Temperature .h"
#include "systick.h"
#include "stdio.h"

/*
    PB6:IIC1_SCL
		PB7:IIC1_SDA
*/

 void SHT31_GPIO_Init(void)
 {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	  GPIO_InitTypeDef GPIO_InitStruct;
	  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType=GPIO_OType_OD;
	  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6 |GPIO_Pin_7;
	  GPIO_InitStruct.GPIO_PuPd=  GPIO_PuPd_NOPULL ;
	  GPIO_InitStruct.GPIO_Speed=GPIO_Fast_Speed ;
	  GPIO_Init( GPIOB ,&GPIO_InitStruct);
	  GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7);
 }
 void SHT31_IIC_Start(void)
 {
	  SDA_H();
    SCL_H();
	  delay_us(1);
	 
	  SDA_L();         //SCL高电平时拉低SDA是启动条件
	  delay_us(1);
	  SCL_L();         //SCL低电平开始写入数据
 }
 
  void SHT31_IIC_END(void)
 {
    SCL_L();
	  SDA_L();
	  delay_us(1);
	 
	  SCL_H();
	  delay_us(1);
	 
	  SDA_H();
	  delay_us(1);
 }
 
  void SHT31_SendByte(u8 data)
	{
	   for(u8 i=0;i<8;i++)
		 {
		    SCL_L();//拉低时钟信号，SDA开始写数据
			  delay_us(1);
			  if((data&(1<<7)))
				{SDA_H();}
				else
				{SDA_L();}
				SCL_H();
				delay_us(1);//等待从机读SDA值
				SCL_L();
				delay_us(1);//等待从机读SDA值
				data<<=1;
		 }
	}
/*return 0 ACK应答成功*/
	u8 SHT31_WaitAck(void)
 {
	  u16 timeout=0;
    SDA_H();//拉高SDA
	  delay_us(1);
	  SCL_H();//开始读SDA数据
	  delay_us(1);
	  while(SDA_Read())
		{
	   	timeout++;
			if(timeout>1000)
			{
				 SCL_L();
				return 1 ;
			}
		}
		SCL_L();
	  return 0 ;
 }
 
 void SHT31_SendAck(u8 ack)
 {
     SCL_L();
	   delay_us(1);
	   if(ack==0)
		 {
		   SDA_L();
		 }
		 else if(ack==1)
		 {
		    SDA_H();
		 }
		delay_us(1);
		 SCL_H();
		 
		 	delay_us(1);
		  SCL_L();
		  SDA_H();
 }
 
 u8  SHT31_ReceiveByte(u8 ack)
 {
	 u8 data=0;
   SCL_L();
	 SDA_H();
	 delay_us(1);
	 for(u8 i=0;i<8;i++)
	 {
	     data<<=1;
       SCL_H();
       delay_us(1);    // 等 SDA 稳定后再读
		  if(SDA_Read())
			{
			  data|=0X01;
			}
			SCL_L();
      delay_us(1);    // 给从机准备 SDA 的时间
	 }
	  SHT31_SendAck( ack);
	  return data;
 }
 
 u8 SHT31_ReadTempHumi(float *temp, float *humi)
 {
	  u8 Temp_H;
	  u8 Temp_L;
	  u8 Temp_CRC;
	  u8 Humi_H;
	  u8 Humi_L;
	  u8 Humi_CRC;
	  SHT31_IIC_Start();
	  SHT31_SendByte(SHT31_Addr_Write);
	  if(SHT31_WaitAck())
		{
			SHT31_IIC_END();
			return 1;
		}
	  SHT31_SendByte(0X24);
		if(SHT31_WaitAck())
		{
			SHT31_IIC_END();
			return 1;
		}
		 SHT31_SendByte(0X00);
		if(SHT31_WaitAck())
		{
			SHT31_IIC_END();
			return 1;
		}
		SHT31_IIC_END();
		
		delay_ms(20);
		SHT31_IIC_Start();
	  SHT31_SendByte(SHT31_Addr_Read);
		if(SHT31_WaitAck())
		{
			SHT31_IIC_END();
			return 1;
		}
		Temp_H= SHT31_ReceiveByte(0);
		Temp_L= SHT31_ReceiveByte(0);
		Temp_CRC= SHT31_ReceiveByte(0);
		Humi_H= SHT31_ReceiveByte(0);
		Humi_L= SHT31_ReceiveByte(0);
		Humi_CRC= SHT31_ReceiveByte(1);
		SHT31_IIC_END();
		
		u16 TEMP=((u16)Temp_H<<8)|Temp_L;
		u16 HUMI=((u16)Humi_H<<8)|Humi_L;
		
		 *temp = 175.0f*((float)TEMP/65535.0f)-45.0f;
			
		 *humi = 100.0f*(float)HUMI/65535.0f;
		return 0 ;
 }
 
 
 
 
 
 
 
 
 