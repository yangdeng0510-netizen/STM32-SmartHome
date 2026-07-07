/**
  ******************************************************************************
  * @file    EXTI/EXTI_Example/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include  "main.h"
# include "HLK_V20.h"
/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */
#include "stm32f4xx.h"                  // Device header

/** @addtogroup EXTI_Example
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/


	
/****USART3_IRQHandler****/


void USART3_IRQHandler(void)
{
    uint8_t data;
   
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        data = (uint8_t)USART_ReceiveData(USART3);

        hlk_rx_buf[hlk_rx_cnt++] = data;

        if (hlk_rx_cnt >= 4)
        {
            hlk_rx_cnt = 0;
            hlk_rx_flag = 1;
        }
    }
}












