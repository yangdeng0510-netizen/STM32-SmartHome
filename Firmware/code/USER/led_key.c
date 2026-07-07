#include "systick.h"
#include "led_key.h"

void KEY_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOAEN,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOBEN,ENABLE);
	  GPIO_InitTypeDef GPKEY;
	  GPKEY.GPIO_Mode=GPIO_Mode_IN;
// GPKEY.GPIO_OType=GPIO_OType_PP;
	  GPKEY.GPIO_Pin= KEY1_PIN;
	  GPKEY.GPIO_PuPd=GPIO_PuPd_NOPULL;
	  GPKEY.GPIO_Speed=GPIO_Medium_Speed;
	  GPIO_Init( KEY1_GPIO_PORT ,&GPKEY);
	
	  GPKEY.GPIO_Pin= KEY2_PIN|KEY3_PIN;
	  GPIO_Init( KEY2_GPIO_PORT ,&GPKEY);	
}	
void LED_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIODEN,ENABLE);
	  GPIO_InitTypeDef GPLED;
	  GPLED.GPIO_Mode=GPIO_Mode_OUT;
    GPLED.GPIO_OType=GPIO_OType_PP;
	  GPLED.GPIO_Pin= LED1_PIN|LED2_PIN|LED3_PIN;
	  //GPLED.GPIO_PuPd=GPIO_PuPd_NOPULL;
	  GPLED.GPIO_Speed=GPIO_Medium_Speed;
	  GPIO_Init( LED1_GPIO_PORT   ,&GPLED);
	
    LED1_OFF  ;   
    LED2_OFF  ;       
    LED3_OFF  ;       
		 
}
uint8_t KEY_Scan(void)      // É¨Ãè°´¼ü×´Ì¬
{
   if((GPIO_ReadInputDataBit(KEY1_GPIO_PORT , KEY1_PIN))== 1)
	 {
	    delay_ms(20);
		   if((GPIO_ReadInputDataBit(KEY1_GPIO_PORT , KEY1_PIN))== 1)
			 { 
				  while(GPIO_ReadInputDataBit(KEY1_GPIO_PORT , KEY1_PIN)==1)
					{}
			    return 1;
			 }
	 }
	 	
	 else if((GPIO_ReadInputDataBit(KEY2_GPIO_PORT , KEY2_PIN))== 0)
	 {
	    delay_ms(20);
		  if((GPIO_ReadInputDataBit(KEY2_GPIO_PORT , KEY2_PIN))== 0)
			{	
				while(GPIO_ReadInputDataBit(KEY2_GPIO_PORT , KEY2_PIN)==0) 
				{}
			  return 2;
			}
	 }
		
	 else if((GPIO_ReadInputDataBit(KEY3_GPIO_PORT , KEY3_PIN))== 0)
	 {
	  delay_ms(20);
		   if((GPIO_ReadInputDataBit(KEY3_GPIO_PORT , KEY3_PIN))== 0)
			 {  
//				 while(GPIO_ReadInputDataBit(KEY3_GPIO_PORT , KEY3_PIN)==0)
//				 {}
			    return 3;
			 }
	 }
		 return 0;
}






