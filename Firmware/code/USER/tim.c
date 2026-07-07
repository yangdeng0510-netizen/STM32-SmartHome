#include "tim.h"
/*电风扇*/
void TIM3_CH3_PC8_Init(void)
{ 
	  /*配置GPIOC8*/
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	  GPIO_InitTypeDef GPTIM;
	  GPIO_StructInit(&GPTIM);
	  GPTIM.GPIO_Mode=GPIO_Mode_AF;
    GPTIM.GPIO_OType=GPIO_OType_PP;
	  GPTIM.GPIO_Pin= GPIO_Pin_8;
	  // GPTIM.GPIO_PuPd=GPIO_PuPd_NOPULL;
	  GPTIM.GPIO_Speed=GPIO_Medium_Speed;
	  GPIO_Init( GPIOC ,&GPTIM);
	  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);
	 /*配置TIM3  42MHZ*/
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	   TIM_TimeBaseInitTypeDef  TIMSTRUCT;
		 TIM_TimeBaseStructInit(&TIMSTRUCT);
	   TIMSTRUCT.TIM_ClockDivision= TIM_CKD_DIV1 ;
	   TIMSTRUCT.TIM_CounterMode=TIM_CounterMode_Up;
	   TIMSTRUCT.TIM_Period=1000-1;
	   TIMSTRUCT.TIM_Prescaler=42-1;//周期1ms
	   //TIMSTRUCT.TIM_RepetitionCounter
	   TIM_TimeBaseInit(TIM3 ,&TIMSTRUCT);
		/*配置OCInit结构体*/
			TIM_OCInitTypeDef TIM3OCSTU;
			TIM_OCStructInit(&TIM3OCSTU);
			TIM3OCSTU.TIM_OCMode=TIM_OCMode_PWM1;
			TIM3OCSTU.TIM_OCPolarity=TIM_OCPolarity_High;
			TIM3OCSTU.TIM_OutputState=TIM_OutputState_Enable;
			TIM3OCSTU.TIM_Pulse= Electric_fan;
			TIM_OC3Init(TIM3,&TIM3OCSTU);
			TIM_Cmd(TIM3, ENABLE);
}

/*舵机初始化*/
void TIM5_CH2_PA1_init(void)
{

  /*配置GPIOA1*/
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	  GPIO_InitTypeDef GPTIM;
	  GPIO_StructInit(&GPTIM);
	  GPTIM.GPIO_Mode=GPIO_Mode_AF;
    GPTIM.GPIO_OType=GPIO_OType_PP;
	  GPTIM.GPIO_Pin= GPIO_Pin_1;
	  // GPTIM.GPIO_PuPd=GPIO_PuPd_NOPULL;
	  GPTIM.GPIO_Speed=GPIO_Medium_Speed;
	  GPIO_Init( GPIOA ,&GPTIM);
	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);
	 /*配置TIM5  虽然APB1挂载时钟42MHZ，但是定时器会自动变成84MHZ*/
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	   TIM_TimeBaseInitTypeDef  TIMSTRUCT;
		 TIM_TimeBaseStructInit(&TIMSTRUCT);
	   TIMSTRUCT.TIM_ClockDivision= TIM_CKD_DIV1 ;
	   TIMSTRUCT.TIM_CounterMode=TIM_CounterMode_Up;
	   TIMSTRUCT.TIM_Period=20000-1;
	   TIMSTRUCT.TIM_Prescaler=84-1;//周期20ms 84*10000/42000000
	   //TIMSTRUCT.TIM_RepetitionCounter
	   TIM_TimeBaseInit(TIM5 ,&TIMSTRUCT);
		/*配置OCInit结构体*/
			TIM_OCInitTypeDef TIM5OCSTU;
			TIM_OCStructInit(&TIM5OCSTU);
			TIM5OCSTU.TIM_OCMode=TIM_OCMode_PWM1;
			TIM5OCSTU.TIM_OCPolarity=TIM_OCPolarity_High;
			TIM5OCSTU.TIM_OutputState=TIM_OutputState_Enable;
			TIM5OCSTU.TIM_Pulse= 0;
			TIM_OC2Init(TIM5,&TIM5OCSTU);
			TIM_Cmd(TIM5, ENABLE);
}



/*改变TIM3的输出比较值*/
void SetCompare_TIM3_CH3(u32 Compare)
{
    TIM_SetCompare3(TIM3, Compare);
}


void servo_angle (float angle)
{
   u16 pluse;
	 pluse=500+2000*angle/180;
   TIM_SetCompare2(TIM5, pluse);
}
	
















