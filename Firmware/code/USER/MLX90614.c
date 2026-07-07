//红外测温
#include "MLX90614.h"
#include "systick.h"
#include "stdio.h"

/*
    PB6:IIC1_SCL
		PB7:IIC1_SDA
*/
/*红外测温*/
 void MLX90614_GPIO_Init(void)
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
 void MLX90614_IIC_Start(void)
 {
	  SDA_H();
    SCL_H();
	  delay_us(10);
	 
	  SDA_L();         //SCL高电平时拉低SDA是启动条件
	  delay_us(10);
	  SCL_L();         //SCL低电平开始写入数据
 }
 
  void MLX90614_IIC_END(void)
 {
    SCL_L();
	  SDA_L();
	  delay_us(10);
	 
	  SCL_H();
	  delay_us(10);
	 
	  SDA_H();
	  delay_us(10);
 }
 
  void MLX90614_SendByte(u8 data)
	{
	   for(u8 i=0;i<8;i++)
		 {
		    SCL_L();//拉低时钟信号，SDA开始写数据
			  delay_us(10);
			  if((data&(1<<7)))
				{SDA_H();}
				else
				{SDA_L();}
				delay_us(10);
				SCL_H();
				delay_us(10);//等待从机读SDA值
				SCL_L();
				delay_us(10);//等待从机读SDA值
				data<<=1;
		 }
	}
/*return 0 ACK应答成功*/
	u8 MLX90614_WaitAck(void)
 {
	  u16 timeout=0;
    SDA_H();//拉高SDA
	  delay_us(10);
	  SCL_H();//开始读SDA数据
	  delay_us(10);
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
 
 void MLX90614_SendAck(u8 ack)
 {
     SCL_L();
	   delay_us(10);
	   if(ack==0)
		 {
		   SDA_L();
		 }
		 else if(ack==1)
		 {
		    SDA_H();
		 }
		delay_us(10);
		 SCL_H();
		 
		 	delay_us(10);
		  SCL_L();
		  SDA_H();
 }
 
 void MLX90614_SMBus_Request(void)
{
    SDA_H();        // 释放 SDA
    SCL_H();        // 释放 SCL
    delay_us(10);

    SCL_L();        // 拉低 SCL，请求 SMBus
    delay_ms(2);    // 大于 1.44ms

    SCL_H();        // 释放 SCL
    delay_us(10);
}

 u8  MLX90614_ReceiveByte(u8 ack)
 {
	
	 u8 data=0;
   SCL_L();
	 SDA_H();
	 delay_us(10);
	 for(u8 i=0;i<8;i++)
	 {   
		   SCL_L();
	     delay_us(10); 
       SDA_H();
       delay_us(2);    // 等 SDA 稳定后再读
		   SCL_H();
	     delay_us(10); 
		  data<<=1;
		  if(SDA_Read())
			{
			  data|=0X01;
			}
			SCL_L();
      delay_us(10);    // 给从机准备 SDA 的时间
	 }
	  MLX90614_SendAck( ack);
	  return data;
 }
 
  u8 MLX90614_ReadHumen_Temp(float *humen_temp)
	{
		u8 Human_Tempure_LSB;
	  u8 Human_Tempure_MSB;
		u8 Human_Tempure_PEC;
		 MLX90614_SMBus_Request();
		delay_us(10);
	  MLX90614_IIC_Start();
		MLX90614_SendByte( MLX90614_Addr_Write);
		if( MLX90614_WaitAck())
		{
			MLX90614_IIC_END();
			return 11 ;
		}
		MLX90614_SendByte(0x07);
		if( MLX90614_WaitAck())
		{
			MLX90614_IIC_END();
			return 12 ;
		}
		 delay_us(6);
		 MLX90614_IIC_Start();
		 MLX90614_SendByte( MLX90614_Addr_Read);
			if( MLX90614_WaitAck())
		{
			MLX90614_IIC_END();
			return 13 ;
		}
	  	 Human_Tempure_LSB=	MLX90614_ReceiveByte(0);
	     Human_Tempure_MSB= MLX90614_ReceiveByte(0);
	     Human_Tempure_PEC= MLX90614_ReceiveByte(1);
		MLX90614_IIC_END();
		u16 raw = (Human_Tempure_MSB << 8) | Human_Tempure_LSB;
//		printf("LSB=0x%02X, MSB=0x%02X, PEC=0x%02X, raw=0x%04X\r\n",
//       Human_Tempure_LSB,
//       Human_Tempure_MSB,
//       Human_Tempure_PEC,
//       raw);
		if (raw & 0x8000)
    {
    return 2;   // 温度数据错误
    }
    *humen_temp = (float)raw * 0.02f - 273.15f;
		return 0;
	}
 
 
 
 
 
 
 
 