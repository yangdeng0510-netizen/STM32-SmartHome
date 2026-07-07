#include "adc.h"

void ADC1_LDR_PC0_Init(void)       // 标准库版本
{
  	/*配置GPIOC0 模拟量输入*/
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	  GPIO_InitTypeDef GPIO_LDR;
	  GPIO_StructInit(&GPIO_LDR);
	  GPIO_LDR.GPIO_Mode=GPIO_Mode_AN;
    //GPIO_LDR.GPIO_OType=GPIO_OType_PP;
	  GPIO_LDR.GPIO_Pin= GPIO_Pin_0;
	  GPIO_LDR.GPIO_PuPd=GPIO_PuPd_NOPULL;
	  GPIO_LDR.GPIO_Speed=GPIO_Fast_Speed;
	  GPIO_Init( GPIOC ,&GPIO_LDR);
	  //GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);
	
	  /*配置ADC*/
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	  ADC_InitTypeDef ADC1_LDRSTRUCT;
	  ADC_StructInit(&ADC1_LDRSTRUCT);
	  ADC1_LDRSTRUCT.ADC_Resolution= ADC_Resolution_12b  ;
	  ADC1_LDRSTRUCT.ADC_ContinuousConvMode=DISABLE;
  	ADC1_LDRSTRUCT.ADC_DataAlign= ADC_DataAlign_Right  ;
	  ADC1_LDRSTRUCT.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None;
	 // ADC1_LDRSTRUCT.ADC_ExternalTrigConv
  	ADC1_LDRSTRUCT.ADC_NbrOfConversion=1;
  	ADC1_LDRSTRUCT.ADC_ScanConvMode= DISABLE;
	  ADC_Init(ADC1, &ADC1_LDRSTRUCT);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_480Cycles);
    ADC_Cmd(ADC1, ENABLE);
		ADC_SoftwareStartConv(ADC1);
}

u16 Read_ADC1_LDR_PC0(void)
{
    return ADC_GetConversionValue(ADC1);
}




void ADC1_LDR_PC0_Reg_Init(void)  // 寄存器版本
{

}


