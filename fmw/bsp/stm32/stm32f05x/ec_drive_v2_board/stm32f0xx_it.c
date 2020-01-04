/**
  ******************************************************************************
  * @file    GPIO/GPIO_IOToggle/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    24-July-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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
#include <assert.h>
#include "stm32f0xx_it.h"
#include "egl_lib.h"
#include "ecd_bsp.h"
/** @addtogroup STM32F0xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
  assert(0);
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  assert(0);
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
  assert(0);
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
  assert(0);
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  assert(0);
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

void EXTI0_1_IRQHandler(void)
{
  egl_result_t result = EGL_SUCCESS;


  if(EXTI_GetITStatus(ECD_BLDC_HALL_1_EXTI) == SET)
    {
      result = egl_bldc_hall_handler(ecd_bldc_motor());
      if(result != EGL_SUCCESS)
      {
        EGL_TRACE_ERROR("Hall 1 handler fail. Result: %s\r\n", EGL_RESULT());
      }

      EXTI_ClearITPendingBit(ECD_BLDC_HALL_1_EXTI);

      egl_led_toggle(ecd_led());
      egl_pio_toggle(ecd_int2_pin());
    }
  
  if(EXTI_GetITStatus(ECD_BLDC_HALL_2_EXTI) == SET)
    {
      result = egl_bldc_hall_handler(ecd_bldc_motor());
      if(result != EGL_SUCCESS)
      {
        EGL_TRACE_ERROR("Hall 2 handler fail. Result: %s\r\n", EGL_RESULT());
      }
      
      EXTI_ClearITPendingBit(ECD_BLDC_HALL_2_EXTI);

      egl_led_toggle(ecd_led());
      egl_pio_toggle(ecd_int2_pin());
    }
}

void EXTI4_15_IRQHandler(void)
{
  egl_result_t result = EGL_SUCCESS;

  if(EXTI_GetITStatus(ECD_BLDC_HALL_3_EXTI) == SET)
    {
      result = egl_bldc_hall_handler(ecd_bldc_motor());
      if(result != EGL_SUCCESS)
      {
        EGL_TRACE_ERROR("Hall 3 handler fail. Result: %s\r\n", EGL_RESULT());
      }
    
      EXTI_ClearITPendingBit(ECD_BLDC_HALL_3_EXTI);

      egl_led_toggle(ecd_led());
      egl_pio_toggle(ecd_int2_pin());  
    }
}




/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
