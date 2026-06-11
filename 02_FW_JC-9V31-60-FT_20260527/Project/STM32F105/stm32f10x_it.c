/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "main.h"
#include "PLC_3_0.h"

#include "INFY_V2G.h"
#include "stm32f105_init.h"
#include "HMI_modbus.h"
#include "sequence.h"
#include "Terminal.h"
#include "fault_check.h"
#include "SIM_100.h"
#include "UART.h"
#include "sim200_can_sched.h"


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/





/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}



void DMA1_Channel1_IRQHandler() // rx
{
  if (DMA_GetITStatus(DMA1_IT_TC1))
  {
    DMA_ClearITPendingBit(DMA1_IT_TC1);
  }
}

void DMA1_Channel2_IRQHandler() // rx
{
  
  if (DMA_GetITStatus(DMA1_IT_TC2))
  {
    
    DMA_ClearITPendingBit(DMA1_IT_TC2);
  }
}

void DMA1_Channel3_IRQHandler()
{
  if (DMA_GetITStatus(DMA1_IT_TC3))
  {
    
    
    DMA_ClearITPendingBit(DMA1_IT_TC3);
    
  }
}
uint16_t  tx_dma1=0, tx_dma2=0;
void DMA1_Channel7_IRQHandler()
{
  if (DMA_GetITStatus(DMA1_IT_TC7))
  {
    while(!DMA_GetFlagStatus(DMA1_FLAG_TC7));
    DMA_ClearITPendingBit(DMA1_IT_TC7);
    DMA_Cmd(DMA1_Channel7, DISABLE);
    
    usart2_tx_ok = 1;
  }
}

void SPI1_IRQHandler()
{
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
  if (TIM_GetFlagStatus(TIM1, TIM_FLAG_Update) != RESET)
  {
    TIM_ClearFlag(TIM1, TIM_IT_Update);
    timer_1ms = 1;
    SIM200_MillisTick();
    
    uint8_t i;
    for (i=0; i<MAX_GCNT; i++) (gCNT[i]++); // timer interrupt  MAX_GCNT = 100
  }
  
}

void ADC1_2_IRQHandler(void)
{
  if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != RESET)
  {
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
  }
  
}

void TIM3_IRQHandler(void)
{
  if (TIM_GetFlagStatus(TIM3, TIM_FLAG_CC1) != RESET)
  {
    TIM_ClearFlag(TIM3, TIM_FLAG_CC1);
  }
  
}


void TIM4_IRQHandler(void)
{
  if (TIM_GetFlagStatus(TIM4, TIM_FLAG_CC1) != RESET)
  {
    TIM_ClearFlag(TIM4, TIM_FLAG_CC1);
  }
  
  if (TIM_GetFlagStatus(TIM4, TIM_FLAG_CC2) != RESET)
  {
    TIM_ClearFlag(TIM4, TIM_FLAG_CC2);
  }
  
  if (TIM_GetFlagStatus(TIM4, TIM_FLAG_CC3) != RESET)
  {
    TIM_ClearFlag(TIM4, TIM_FLAG_CC3);
  }
  
  if (TIM_GetFlagStatus(TIM4, TIM_FLAG_CC4) != RESET)
  {
    TIM_ClearFlag(TIM4, TIM_FLAG_CC4);
  }
  
  
}


void USART3_IRQHandler(void)
{
/**************************************송신**************************************/
#ifdef ACPM_DCPM_READ
  uint8_t   temp_DM;
    
    if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET) // Tansmit Data Register empty interrupt
    {
      
      if(power_meter_tx_data == acrel_from_0)
      {
        U3_Enable;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 수신 인터럽트 DISABLE
        
        USART_SendData(USART3, Acrel_DC_Power_Meter_ch1_Tx_from_0[Uart3_Trnsmt_count]); /* Send Transaction data */ 
        
        if (Uart3_Trnsmt_count++ >= 7) // 8Bytes            
        {
          Uart3_Trnsmt_count = 0;
          USART_ClearFlag(USART3, USART_FLAG_TXE);
          
          USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
          USART_ITConfig(USART3, USART_IT_TC, ENABLE);   // 전송 완료 인터럽트를 활성화 한다
        }
      }
      else if(power_meter_tx_data == eastron_read_input_registers_1)
      {
        U3_Enable;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 수신 인터럽트 DISABLE
        
        USART_SendData(USART3, Eastron_AC_Power_Meter_Tx_read_input_registers_1[Uart3_Trnsmt_count]); /* Send Transaction data */ 
        
        if (Uart3_Trnsmt_count++ >= 7) // 8Bytes
        {
          Uart3_Trnsmt_count = 0;
          USART_ClearFlag(USART3, USART_FLAG_TXE);
          
          USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
          USART_ITConfig(USART3, USART_IT_TC, ENABLE);   // 전송 완료 인터럽트를 활성화 한다
        }
      }
      else if(power_meter_tx_data == eastron_read_input_registers_2)
      {
        U3_Enable;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 수신 인터럽트 DISABLE
        
        USART_SendData(USART3, Eastron_AC_Power_Meter_Tx_read_input_registers_2[Uart3_Trnsmt_count]); /* Send Transaction data */ 
        
        if (Uart3_Trnsmt_count++ >= 7) // 8Bytes            
        {
          Uart3_Trnsmt_count = 0;
          USART_ClearFlag(USART3, USART_FLAG_TXE);
          
          USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
          USART_ITConfig(USART3, USART_IT_TC, ENABLE);   // 전송 완료 인터럽트를 활성화 한다
        }
      }
      else if(power_meter_tx_data == eastron_read_input_registers_3)
      {
        U3_Enable;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 수신 인터럽트 DISABLE
        
        USART_SendData(USART3, Eastron_AC_Power_Meter_Tx_read_input_registers_3[Uart3_Trnsmt_count]); /* Send Transaction data */ 
        
        if (Uart3_Trnsmt_count++ >= 7) // 8Bytes            
        {
          Uart3_Trnsmt_count = 0;
          USART_ClearFlag(USART3, USART_FLAG_TXE);
          
          USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
          USART_ITConfig(USART3, USART_IT_TC, ENABLE);   // 전송 완료 인터럽트를 활성화 한다
        }
      }
      else if(power_meter_tx_data == eastron_read_input_registers_4)
      {
        U3_Enable;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 수신 인터럽트 DISABLE
        
        USART_SendData(USART3, Eastron_AC_Power_Meter_Tx_read_input_registers_4[Uart3_Trnsmt_count]); /* Send Transaction data */ 
        
        if (Uart3_Trnsmt_count++ >= 7) // 8Bytes            
        {
          Uart3_Trnsmt_count = 0;
          USART_ClearFlag(USART3, USART_FLAG_TXE);
          
          USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
          USART_ITConfig(USART3, USART_IT_TC, ENABLE);   // 전송 완료 인터럽트를 활성화 한다
        }
      }
      else if(power_meter_tx_data == eastron_read_input_registers_5)
      {
        U3_Enable;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 수신 인터럽트 DISABLE
        
        USART_SendData(USART3, Eastron_AC_Power_Meter_Tx_read_input_registers_5[Uart3_Trnsmt_count]); /* Send Transaction data */ 
        
        if (Uart3_Trnsmt_count++ >= 7) // 8Bytes            
        {
          Uart3_Trnsmt_count = 0;
          USART_ClearFlag(USART3, USART_FLAG_TXE);
          
          USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
          USART_ITConfig(USART3, USART_IT_TC, ENABLE);   // 전송 완료 인터럽트를 활성화 한다
        }
      }
      
    }
    else
    {
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE); // 버퍼가 비어 있다면 TXE 인터럽트를 비활성화 한다
    }
    
    if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
    {
      Uart3_Trnsmt_count = 0;
      USART_ITConfig(USART3, USART_IT_TC, DISABLE);
      
      USART_ClearFlag(USART3, USART_FLAG_TC);  
      
      USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
      U3_Disable;
    }
    
#elif defined ONLY_ACPM_READ
      uint8_t   temp_DM;
    
    if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET) // Tansmit Data Register empty interrupt
    {
      if(power_meter_tx_data == eastron_read_input_registers_1)
      {
        U3_Enable;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 수신 인터럽트 DISABLE
        
        USART_SendData(USART3, Eastron_AC_Power_Meter_Tx_read_input_registers_1[Uart3_Trnsmt_count]); /* Send Transaction data */ 
        
        if (Uart3_Trnsmt_count++ >= 7) // 8Bytes
        {
          Uart3_Trnsmt_count = 0;
          USART_ClearFlag(USART3, USART_FLAG_TXE);
          
          USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
          USART_ITConfig(USART3, USART_IT_TC, ENABLE);   // 전송 완료 인터럽트를 활성화 한다
        }
      }
      else if(power_meter_tx_data == eastron_read_input_registers_2)
      {
        U3_Enable;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 수신 인터럽트 DISABLE
        
        USART_SendData(USART3, Eastron_AC_Power_Meter_Tx_read_input_registers_2[Uart3_Trnsmt_count]); /* Send Transaction data */ 
        
        if (Uart3_Trnsmt_count++ >= 7) // 8Bytes            
        {
          Uart3_Trnsmt_count = 0;
          USART_ClearFlag(USART3, USART_FLAG_TXE);
          
          USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
          USART_ITConfig(USART3, USART_IT_TC, ENABLE);   // 전송 완료 인터럽트를 활성화 한다
        }
      }
      else if(power_meter_tx_data == eastron_read_input_registers_3)
      {
        U3_Enable;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 수신 인터럽트 DISABLE
        
        USART_SendData(USART3, Eastron_AC_Power_Meter_Tx_read_input_registers_3[Uart3_Trnsmt_count]); /* Send Transaction data */ 
        
        if (Uart3_Trnsmt_count++ >= 7) // 8Bytes            
        {
          Uart3_Trnsmt_count = 0;
          USART_ClearFlag(USART3, USART_FLAG_TXE);
          
          USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
          USART_ITConfig(USART3, USART_IT_TC, ENABLE);   // 전송 완료 인터럽트를 활성화 한다
        }
      }
      else if(power_meter_tx_data == eastron_read_input_registers_4)
      {
        U3_Enable;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 수신 인터럽트 DISABLE
        
        USART_SendData(USART3, Eastron_AC_Power_Meter_Tx_read_input_registers_4[Uart3_Trnsmt_count]); /* Send Transaction data */ 
        
        if (Uart3_Trnsmt_count++ >= 7) // 8Bytes            
        {
          Uart3_Trnsmt_count = 0;
          USART_ClearFlag(USART3, USART_FLAG_TXE);
          
          USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
          USART_ITConfig(USART3, USART_IT_TC, ENABLE);   // 전송 완료 인터럽트를 활성화 한다
        }
      }
      else if(power_meter_tx_data == eastron_read_input_registers_5)
      {
        U3_Enable;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 수신 인터럽트 DISABLE
        
        USART_SendData(USART3, Eastron_AC_Power_Meter_Tx_read_input_registers_5[Uart3_Trnsmt_count]); /* Send Transaction data */ 
        
        if (Uart3_Trnsmt_count++ >= 7) // 8Bytes            
        {
          Uart3_Trnsmt_count = 0;
          USART_ClearFlag(USART3, USART_FLAG_TXE);
          
          USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
          USART_ITConfig(USART3, USART_IT_TC, ENABLE);   // 전송 완료 인터럽트를 활성화 한다
        }
      }
    }
    else
    {
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE); // 버퍼가 비어 있다면 TXE 인터럽트를 비활성화 한다
    }
    
    if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
    {
      Uart3_Trnsmt_count = 0;
      USART_ITConfig(USART3, USART_IT_TC, DISABLE);
      
      USART_ClearFlag(USART3, USART_FLAG_TC);  
      
      USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
      U3_Disable;
    }
    
#elif defined ONLY_DCPM_READ
      uint8_t   temp_DM;
    
    if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET) // Tansmit Data Register empty interrupt
    {
      
      if(power_meter_tx_data == acrel_from_0)
      {
        U3_Enable;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 수신 인터럽트 DISABLE
        
        USART_SendData(USART3, Acrel_DC_Power_Meter_ch1_Tx_from_0[Uart3_Trnsmt_count]); /* Send Transaction data */ 
        
        if (Uart3_Trnsmt_count++ >= 7) // 8Bytes            
        {
          Uart3_Trnsmt_count = 0;
          USART_ClearFlag(USART3, USART_FLAG_TXE);
          
          USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
          USART_ITConfig(USART3, USART_IT_TC, ENABLE);   // 전송 완료 인터럽트를 활성화 한다
        }
      }
    }
    else
    {
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE); // 버퍼가 비어 있다면 TXE 인터럽트를 비활성화 한다
    }
    
    if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
    {
      Uart3_Trnsmt_count = 0;
      USART_ITConfig(USART3, USART_IT_TC, DISABLE);
      
      USART_ClearFlag(USART3, USART_FLAG_TC);  
      
      USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
      U3_Disable;
    }
    
#elif defined DISABLE_PM_DATA
          uint8_t   temp_DM;
    
    if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET) // Tansmit Data Register empty interrupt
    {
      
      if(power_meter_tx_data == acrel_from_0)
      {
        U3_Enable;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 수신 인터럽트 DISABLE
        
        USART_SendData(USART3, Acrel_DC_Power_Meter_ch1_Tx_from_0[Uart3_Trnsmt_count]); /* Send Transaction data */ 
        
        if (Uart3_Trnsmt_count++ >= 7) // 8Bytes            
        {
          Uart3_Trnsmt_count = 0;
          USART_ClearFlag(USART3, USART_FLAG_TXE);
          
          USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
          USART_ITConfig(USART3, USART_IT_TC, ENABLE);   // 전송 완료 인터럽트를 활성화 한다
        }
      }
    }
#endif
/**************************************수신**************************************/
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
    uart3_rx_isr_cnt++;
    
    temp_DM = USART_ReceiveData(USART3);
    
    switch(power_meter_tx_data)
    {
#ifdef ACPM_DCPM_READ
      
    case acrel_from_0:
      
        Acrel_DC_Power_Meter_ch1_Rx_from_0[cRxd_DM++] = temp_DM;
        switch(cRxd_DM)
        {
        case 1:     if (temp_DM != 0x01)            cRxd_DM = 0;	   // Address
        break;
        case 2:     if (temp_DM != 0x03)		  cRxd_DM = 0;     // Function code
        break;       
        case 3:     if (temp_DM == (Request_word_data_length_from_0 * 2)) {DM_cRxdMax = 3 + temp_DM + 2;} // data length, ex) 0x1E ->  add + func + data_length + 30 byte + crc16 = 35byte
        else                            cRxd_DM = 0;
        break;
        }
        if (cRxd_DM >= DM_cRxdMax) // 무조건 수신 후 복사 데이터 길이를 알게 때문에 이렇게 프로그램 할수 있고 다른방식으로 바꿔야 함.
        {
          rxok_DM = 1;
          cRxd_DM = 0;
          dc_meter_communication_rx_fault_count = 0;
          power_meter_tx_data = eastron_read_input_registers_1;
        }
        break;
        
    case eastron_read_input_registers_1:
    
      Eastron_AC_Power_Meter_Rx_read_input_registers_1[cRxd_DM++] = temp_DM;
      switch(cRxd_DM)
      {
      case 1:     if (temp_DM != 0x02)            cRxd_DM = 0;	   // Address
      break;
      case 2:     if (temp_DM != 0x04)		  cRxd_DM = 0;     // Function code
      break;       
      case 3:     if (temp_DM == (Request_word_data_length_read_input_register_1 * 2)) {DM_cRxdMax = 3 + temp_DM + 2;} // data length, ex) 0x1E ->  add + func + data_length + 30 byte + crc16 = 35byte
      else                            cRxd_DM = 0;
      break;
      }
      if (cRxd_DM >= DM_cRxdMax) // 무조건 수신 후 복사 데이터 길이를 알게 때문에 이렇게 프로그램 할수 있고 다른방식으로 바꿔야 함.
      {
        rxok_DM = 1;
        cRxd_DM = 0;
        ac_meter_communication_rx_fault_count[1] = 0;
        power_meter_tx_data = eastron_read_input_registers_2;
      }
      break;
      
      case eastron_read_input_registers_2:
    
      Eastron_AC_Power_Meter_Rx_read_input_registers_2[cRxd_DM++] = temp_DM;
      switch(cRxd_DM)
      {
      case 1:     if (temp_DM != 0x02)            cRxd_DM = 0;	   // Address
      break;
      case 2:     if (temp_DM != 0x04)		  cRxd_DM = 0;     // Function code
      break;       
      case 3:     if (temp_DM == (Request_word_data_length_read_input_register_2 * 2)) {DM_cRxdMax = 3 + temp_DM + 2;} // data length, ex) 0x1E ->  add + func + data_length + 30 byte + crc16 = 35byte
      else                            cRxd_DM = 0;
      break;
      }
      if (cRxd_DM >= DM_cRxdMax) // 무조건 수신 후 복사 데이터 길이를 알게 때문에 이렇게 프로그램 할수 있고 다른방식으로 바꿔야 함.
      {
        rxok_DM = 1;
        cRxd_DM = 0;
        ac_meter_communication_rx_fault_count[2] = 0;
        power_meter_tx_data = eastron_read_input_registers_3;
      }
      break;
      
      case eastron_read_input_registers_3:
    
      Eastron_AC_Power_Meter_Rx_read_input_registers_3[cRxd_DM++] = temp_DM;
      switch(cRxd_DM)
      {
      case 1:     if (temp_DM != 0x02)            cRxd_DM = 0;	   // Address
      break;
      case 2:     if (temp_DM != 0x04)		  cRxd_DM = 0;     // Function code
      break;       
      case 3:     if (temp_DM == (Request_word_data_length_read_input_register_3 * 2)) {DM_cRxdMax = 3 + temp_DM + 2;} // data length, ex) 0x1E ->  add + func + data_length + 30 byte + crc16 = 35byte
      else                            cRxd_DM = 0;
      break;
      }
      if (cRxd_DM >= DM_cRxdMax) // 무조건 수신 후 복사 데이터 길이를 알게 때문에 이렇게 프로그램 할수 있고 다른방식으로 바꿔야 함.
      {
        rxok_DM = 1;
        cRxd_DM = 0;
        ac_meter_communication_rx_fault_count[3] = 0;
        power_meter_tx_data = eastron_read_input_registers_4;
      }
      break;
      
      case eastron_read_input_registers_4:
    
      Eastron_AC_Power_Meter_Rx_read_input_registers_4[cRxd_DM++] = temp_DM;
      switch(cRxd_DM)
      {
      case 1:     if (temp_DM != 0x02)            cRxd_DM = 0;	   // Address
      break;
      case 2:     if (temp_DM != 0x04)		  cRxd_DM = 0;     // Function code
      break;       
      case 3:     if (temp_DM == (Request_word_data_length_read_input_register_4 * 2)) {DM_cRxdMax = 3 + temp_DM + 2;} // data length, ex) 0x1E ->  add + func + data_length + 30 byte + crc16 = 35byte
      else                            cRxd_DM = 0;
      break;
      }
      if (cRxd_DM >= DM_cRxdMax) // 무조건 수신 후 복사 데이터 길이를 알게 때문에 이렇게 프로그램 할수 있고 다른방식으로 바꿔야 함.
      {
        rxok_DM = 1;
        cRxd_DM = 0;
        ac_meter_communication_rx_fault_count[4] = 0;
        power_meter_tx_data = eastron_read_input_registers_5;
      }
      break;
      
      case eastron_read_input_registers_5:
    
      Eastron_AC_Power_Meter_Rx_read_input_registers_5[cRxd_DM++] = temp_DM;
      switch(cRxd_DM)
      {
      case 1:     if (temp_DM != 0x02)            cRxd_DM = 0;	   // Address
      break;
      case 2:     if (temp_DM != 0x04)		  cRxd_DM = 0;     // Function code
      break;       
      case 3:     if (temp_DM == (Request_word_data_length_read_input_register_5 * 2)) {DM_cRxdMax = 3 + temp_DM + 2;} // data length, ex) 0x1E ->  add + func + data_length + 30 byte + crc16 = 35byte
      else                            cRxd_DM = 0;
      break;
      }
      if (cRxd_DM >= DM_cRxdMax) // 무조건 수신 후 복사 데이터 길이를 알게 때문에 이렇게 프로그램 할수 있고 다른방식으로 바꿔야 함.
      {
        rxok_DM = 1;
        cRxd_DM = 0;
        ac_meter_communication_rx_fault_count[5] = 0;
        power_meter_tx_data = acrel_from_0;
      }
      break;
      
#elif defined ONLY_ACPM_READ

    case eastron_read_input_registers_1:
    
      Eastron_AC_Power_Meter_Rx_read_input_registers_1[cRxd_DM++] = temp_DM;
      switch(cRxd_DM)
      {
      case 1:     if (temp_DM != 0x02)            cRxd_DM = 0;	   // Address
      break;
      case 2:     if (temp_DM != 0x04)		  cRxd_DM = 0;     // Function code
      break;       
      case 3:     if (temp_DM == (Request_word_data_length_read_input_register_1 * 2)) {DM_cRxdMax = 3 + temp_DM + 2;} // data length, ex) 0x1E ->  add + func + data_length + 30 byte + crc16 = 35byte
      else                            cRxd_DM = 0;
      break;
      }
      if (cRxd_DM >= DM_cRxdMax) // 무조건 수신 후 복사 데이터 길이를 알게 때문에 이렇게 프로그램 할수 있고 다른방식으로 바꿔야 함.
      {
        rxok_DM = 1;
        cRxd_DM = 0;
        ac_meter_communication_rx_fault_count[1] = 0;
        power_meter_tx_data = eastron_read_input_registers_2;
      }
      break;
      
      case eastron_read_input_registers_2:
    
      Eastron_AC_Power_Meter_Rx_read_input_registers_2[cRxd_DM++] = temp_DM;
      switch(cRxd_DM)
      {
      case 1:     if (temp_DM != 0x02)            cRxd_DM = 0;	   // Address
      break;
      case 2:     if (temp_DM != 0x04)		  cRxd_DM = 0;     // Function code
      break;       
      case 3:     if (temp_DM == (Request_word_data_length_read_input_register_2 * 2)) {DM_cRxdMax = 3 + temp_DM + 2;} // data length, ex) 0x1E ->  add + func + data_length + 30 byte + crc16 = 35byte
      else                            cRxd_DM = 0;
      break;
      }
      if (cRxd_DM >= DM_cRxdMax) // 무조건 수신 후 복사 데이터 길이를 알게 때문에 이렇게 프로그램 할수 있고 다른방식으로 바꿔야 함.
      {
        rxok_DM = 1;
        cRxd_DM = 0;
        ac_meter_communication_rx_fault_count[2] = 0;
        power_meter_tx_data = eastron_read_input_registers_3;
      }
      break;
      
      case eastron_read_input_registers_3:
    
      Eastron_AC_Power_Meter_Rx_read_input_registers_3[cRxd_DM++] = temp_DM;
      switch(cRxd_DM)
      {
      case 1:     if (temp_DM != 0x02)            cRxd_DM = 0;	   // Address
      break;
      case 2:     if (temp_DM != 0x04)		  cRxd_DM = 0;     // Function code
      break;       
      case 3:     if (temp_DM == (Request_word_data_length_read_input_register_3 * 2)) {DM_cRxdMax = 3 + temp_DM + 2;} // data length, ex) 0x1E ->  add + func + data_length + 30 byte + crc16 = 35byte
      else                            cRxd_DM = 0;
      break;
      }
      if (cRxd_DM >= DM_cRxdMax) // 무조건 수신 후 복사 데이터 길이를 알게 때문에 이렇게 프로그램 할수 있고 다른방식으로 바꿔야 함.
      {
        rxok_DM = 1;
        cRxd_DM = 0;
        ac_meter_communication_rx_fault_count[3] = 0;
        power_meter_tx_data = eastron_read_input_registers_4;
      }
      break;
      
      case eastron_read_input_registers_4:
    
      Eastron_AC_Power_Meter_Rx_read_input_registers_4[cRxd_DM++] = temp_DM;
      switch(cRxd_DM)
      {
      case 1:     if (temp_DM != 0x02)            cRxd_DM = 0;	   // Address
      break;
      case 2:     if (temp_DM != 0x04)		  cRxd_DM = 0;     // Function code
      break;       
      case 3:     if (temp_DM == (Request_word_data_length_read_input_register_4 * 2)) {DM_cRxdMax = 3 + temp_DM + 2;} // data length, ex) 0x1E ->  add + func + data_length + 30 byte + crc16 = 35byte
      else                            cRxd_DM = 0;
      break;
      }
      if (cRxd_DM >= DM_cRxdMax) // 무조건 수신 후 복사 데이터 길이를 알게 때문에 이렇게 프로그램 할수 있고 다른방식으로 바꿔야 함.
      {
        rxok_DM = 1;
        cRxd_DM = 0;
        ac_meter_communication_rx_fault_count[4] = 0;
        power_meter_tx_data = eastron_read_input_registers_5;
      }
      break;
      
      case eastron_read_input_registers_5:
    
      Eastron_AC_Power_Meter_Rx_read_input_registers_5[cRxd_DM++] = temp_DM;
      switch(cRxd_DM)
      {
      case 1:     if (temp_DM != 0x02)            cRxd_DM = 0;	   // Address
      break;
      case 2:     if (temp_DM != 0x04)		  cRxd_DM = 0;     // Function code
      break;       
      case 3:     if (temp_DM == (Request_word_data_length_read_input_register_5 * 2)) {DM_cRxdMax = 3 + temp_DM + 2;} // data length, ex) 0x1E ->  add + func + data_length + 30 byte + crc16 = 35byte
      else                            cRxd_DM = 0;
      break;
      }
      if (cRxd_DM >= DM_cRxdMax) // 무조건 수신 후 복사 데이터 길이를 알게 때문에 이렇게 프로그램 할수 있고 다른방식으로 바꿔야 함.
      {
        rxok_DM = 1;
        cRxd_DM = 0;
        ac_meter_communication_rx_fault_count[5] = 0;
        power_meter_tx_data = eastron_read_input_registers_1;
      }
      break;
      
#elif defined ONLY_DCPM_READ
          case acrel_from_0:
      
        Acrel_DC_Power_Meter_ch1_Rx_from_0[cRxd_DM++] = temp_DM;
        switch(cRxd_DM)
        {
        case 1:     if (temp_DM != 0x01)            cRxd_DM = 0;	   // Address
        break;
        case 2:     if (temp_DM != 0x03)		  cRxd_DM = 0;     // Function code
        break;       
        case 3:     if (temp_DM == (Request_word_data_length_from_0 * 2)) {DM_cRxdMax = 3 + temp_DM + 2;} // data length, ex) 0x1E ->  add + func + data_length + 30 byte + crc16 = 35byte
        else                            cRxd_DM = 0;
        break;
        }
        if (cRxd_DM >= DM_cRxdMax) // 무조건 수신 후 복사 데이터 길이를 알게 때문에 이렇게 프로그램 할수 있고 다른방식으로 바꿔야 함.
        {
          rxok_DM = 1;
          cRxd_DM = 0;
          dc_meter_communication_rx_fault_count = 0;
          power_meter_tx_data = acrel_from_0;
        }
        break;
      
#elif defined DISABLE_PM_DATA
          case acrel_from_0:
            
            dc_meter_communication_rx_fault_count = 0;
            power_meter_tx_data = acrel_from_0;
      
        Acrel_DC_Power_Meter_ch1_Rx_from_0[cRxd_DM++] = temp_DM;
        switch(cRxd_DM)
        {
        case 1:     if (temp_DM != 0x01)            cRxd_DM = 0;	   // Address
        break;
        case 2:     if (temp_DM != 0x03)		  cRxd_DM = 0;     // Function code
        break;       
        case 3:     if (temp_DM == (Request_word_data_length_from_0 * 2)) {DM_cRxdMax = 3 + temp_DM + 2;} // data length, ex) 0x1E ->  add + func + data_length + 30 byte + crc16 = 35byte
        else                            cRxd_DM = 0;
        break;
        }
        if (cRxd_DM >= DM_cRxdMax) // 무조건 수신 후 복사 데이터 길이를 알게 때문에 이렇게 프로그램 할수 있고 다른방식으로 바꿔야 함.
        {
          rxok_DM = 1;
          cRxd_DM = 0;
          dc_meter_communication_rx_fault_count = 0;
          power_meter_tx_data = eastron_read_input_registers_1;
        }
        break;
      
#endif
      
    }

    USART_ClearITPendingBit(USART3, USART_IT_RXNE);  
  }
}

uint8_t temp_2=0, temp_3=0, temp_4=0;

void USART2_IRQHandler(void) //수신 되는 보드ID
{
  uint8_t temp1;
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // receive error check 이게 맞나?
  {
    temp1 = USART_ReceiveData(USART2);
    bRxSio_2[cRxd_2++] = (uint8_t)temp1;
    
    switch(cRxd_2)
    {
    case 1:
      if     (temp1 == 0x01)    BOARD_ID_RX = 1;
      else if(temp1 == 0x02)    BOARD_ID_RX = 2;
      else                      cRxd_2 = 0;
      
      break;
      
    case 2:
      if     (temp1 == 0x04)      temp_2 = (0x04); //Read
      else if(temp1 == 0x10)      temp_2 = (0x10); //Write
      else                        cRxd_2 = 0;
      break;
      
    case 3:
      if     (temp1 == 0x00)      temp_3 = (0x01); //Write 200                                     temp_3 ??? 
      else if(temp1 == 0x01)      temp_3 = (0x02); //Write(Test) 300, Read 400, Read(Test) 500     temp_3 ???
      else                        cRxd_2 = 0;
      break;
      
    case 4:
      if     ((temp_2 == 0x04) && (temp_3 == 0x02) && (temp1 == 0x90))    { cRxdMax_2 =  8; temp_2 = 0; temp_3 = 0; } // Read 400
      else if((temp_2 == 0x10) && (temp_3 == 0x01) && (temp1 == 0xC8))    { cRxdMax_2 = 71; temp_2 = 0; temp_3 = 0; } // Write 200
      else                                                                { cRxd_2 = 0; }
      break;
      
    case 5:
      if(temp1 != 0x00)           cRxd_2 = 0;
      break;
      
    case 6:
      ReadCnt = temp1; //전송갯수
      break;
    }
    
    if(cRxd_2 >= cRxdMax_2)         
    {
      rxok_2 = 1;
      cRxd_2 = 0;
    }
    else                            
    {rxok_2 = 0;}
    
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  }
}


uint8_t test_rx_chartohex[29];

void USART1_IRQHandler(void)
{  
  u8 temp;
  u8 i;
  uart1_cnt++;
  cRxdMax_1 = 26;       // 패킷 길이
  
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
  {
    temp = USART_ReceiveData(USART1);
    bRxSio_1[cRxd_1++] = temp;
    
    if(temp == '\r') 
    {
      for(i=0; i<cRxd_1-1; i++)
      {
        term_rx_msg_buffer_copy[i] = bRxSio_1[i];
        test_rx_chartohex[i] =  AsciiToHex(bRxSio_1[i]);
      }
      for (i=cRxd_1-1; i<29; i++)
      {
        term_rx_msg_buffer_copy[i] = 0;
      }
      
      if     (term_rx_msg_buffer_copy[0] == 'c' && term_rx_msg_buffer_copy[1] == 'i' && term_rx_msg_buffer_copy[2] == 'n' && term_rx_msg_buffer_copy[3] == 'f' && term_rx_msg_buffer_copy[4] == 'o')                                            {system_cmd = charging_info_start_cmd;}
      else if(term_rx_msg_buffer_copy[0] == 's' && term_rx_msg_buffer_copy[1] == 't' && term_rx_msg_buffer_copy[2] == 'o' && term_rx_msg_buffer_copy[3] == 'p')                                                                                 {system_cmd = charging_info_stop_cmd;}
      else if(term_rx_msg_buffer_copy[0] == 'r' && term_rx_msg_buffer_copy[1] == 'e' && term_rx_msg_buffer_copy[2] == 's' && term_rx_msg_buffer_copy[3] == 'e' && term_rx_msg_buffer_copy[4] == 't')                                            {system_cmd = system_reset_cmd;}
      else if(term_rx_msg_buffer_copy[0] == 'u' && term_rx_msg_buffer_copy[1] == 'p' && term_rx_msg_buffer_copy[2] == 'd' && term_rx_msg_buffer_copy[3] == 'a' && term_rx_msg_buffer_copy[4] == 't' && term_rx_msg_buffer_copy[5] == 'e')       {system_cmd = fw_update_cmd;}
      else if(term_rx_msg_buffer_copy[0] == 'v' && term_rx_msg_buffer_copy[1] == 'e' && term_rx_msg_buffer_copy[2] == 'r' && term_rx_msg_buffer_copy[3] == 's' && term_rx_msg_buffer_copy[4] == 'i' && term_rx_msg_buffer_copy[5] == 'o' && term_rx_msg_buffer_copy[6] == 'n') {system_cmd = version_cmd;}
      else if(term_rx_msg_buffer_copy[0] == 's' && term_rx_msg_buffer_copy[1] == 't' && term_rx_msg_buffer_copy[2] == 'a' && term_rx_msg_buffer_copy[3] == 't' && term_rx_msg_buffer_copy[4] == 'u' && term_rx_msg_buffer_copy[5] == 's')       {system_cmd = charging_status;} 
      else if(term_rx_msg_buffer_copy[0] == 't' && term_rx_msg_buffer_copy[1] == 'e' && term_rx_msg_buffer_copy[2] == 's' && term_rx_msg_buffer_copy[3] == 't')                                                                                 {self_check_start = 1;} 
      else if(term_rx_msg_buffer_copy[0] == 'h' && term_rx_msg_buffer_copy[1] == 'i' && term_rx_msg_buffer_copy[2] == 's')                                                                                                                      {system_cmd = history_cmd;} 
      else                                                                                                                                                                                                                                      {system_cmd = wrong_cmd;}
      cRxd_1 = 0;
    }
    
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);    
  }   
  
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {         
    if (Uart1_Trnsmt_count <= 25)
    {
      /* Send Transaction data */
      USART_SendData(USART1, bTxSio_1[Uart1_Trnsmt_count]);
      Uart1_Trnsmt_count++;    
    }
    else
    {
      Uart1_Trnsmt_count = 0;
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
    
  }
  
  
}
void CAN1_RX0_IRQHandler(void) //CAN1_RX0_IRQn
{
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage1);
  CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
  
  Rx_Module(RxMessage1.ExtId);
}


void CAN2_RX0_IRQHandler(void) //CAN2_RX0 (PLC_modem/IMD_Sendyne)
{
  CAN_Receive(CAN2, CAN_FIFO0, &RxMessage2);
  CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
  
  if(RxMessage2.ExtId != SID_ID_EVSE)           SECC_Msg_Receive_1CH();
  
  if (RxMessage2.IDE == CAN_ID_EXT && RxMessage2.ExtId == SIM200_CAN_ID_SIM200_TX) {
    CAN2_RxISR_Handler((uint8_t)RxMessage2.DLC, RxMessage2.Data);
    return;
  }
}

void CAN2_TX_IRQHandler(void)
{
  if (CAN_GetITStatus(CAN2,CAN_IT_TME))
  {
    CAN_Transmit(CAN2, &TxMessage2);
    CAN_ClearITPendingBit(CAN2,CAN_IT_TME);
    
  } 
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
