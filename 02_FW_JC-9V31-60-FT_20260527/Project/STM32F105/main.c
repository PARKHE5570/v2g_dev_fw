
//=============================================================================
//                                                                             
//      COPYRIGHT (c) EVSIS Co., Ltd. 2023                          
//                                                                             
//      The copyright to the document(s) herein is the property of             
//      EVSIS Co., Ltd.                                             
//                                                                             
//      The document(s) may be used  and/or copied only with the written       
//      permission from EVSIS or in accordance with the terms and   
//      conditions stipulated in the agreement/contract under which the        
//      document(s) have been supplied.                                        
//                                                                             
//=============================================================================
//
//                                                                             
//  File........: main.c
//                                                                             
//  Description.: JC-9V31-22-FT                                                             
//                                                                             
//  Revisions...:                                                              
//                                                                             
//  YYYYMMDD - VER.   - COMMENT                                       - SIGN.    
//                                                                             

//                                                                             
//=============================================================================
/*
CAN1 (CN7)      CAN2.0B   125kbps       - POWER_MODULE (종단저항이 없으면 통신 안됨)
CAN2 (CN11)     CAN2.0B   500kbps       - GRIDWIZ PLC, SENDYNE IMD
UART1(CN14)     RS-232   460800bps       - FAULT_CHECK Terminal
UART4(CN6)      RS-485    38400bps       - TOUCH PC(HMI) RS-485
UART3(CN10)     RS-485     9600bps       - ACREL DC METER, EASTRON AC METER (HMI에서 받을수도 있음)
Attenuation normal range : 25~35, 작을수록 센 것임, 감쇠율, 입력된 전력을 dB 단위로 변환하여 표기하며, 해당 감쇠기를 통하여서 입력된 포트에서 출력되는 포트로 얼마만큼의 감쇠된 값을 얻을지 지표가 됩니다.
전원이 꺼진 상태에서 뱅크와 스탠드가 연결되었을 때 CAN H와 L 사이가 저항값을 측정하면 60옴, 뱅크와 스탠드가 연결이 안 되어 있을 때는 각각 120옴이 나와야 한다. 
마지막 스탠드와 뱅크의 중간에 설치 된 스탠드는 종단 저항이 없어야 한다.
*/

#define _MAIN_C

#include <stdio.h>
#include <stdbool.h>
#include "stdlib.h"
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "stm32f105_init.h"

#include "main.h"
#include "digital_io.h"
#include "PLC_3_0.h"
#include "pt1000.h"
#include "INFY_V2G.h"

#include "sequence.h"
#include "HMI_modbus.h"
#include "FirmwareUpdate.h"
#include "Terminal.h"
#include "fault_check.h"
#include "self_check.h"
#include "SIM_100.h"
#include "UART.h"
#include "sim200_protocol.h"
#include "sim200_can_sched.h"



//1. auto self test
//2. manual control(I/O)
//3. main_program_proc(normal combo sequence)
//4. force charging
//5. load test

// 일반적으로 터미널 모드는 사용하지 않는다. 자가 진단 모드에서는 시퀀스가 동작하지 말아야한다. 자가 진단 후 결과를 UI로 전송 
// 릴레이를 한 쪽만 동작시키고 모듈 각각 전압을 출력해서 DCGF가 동작하는지?
// 충전이 느리다고 하는 사용자 확인을 위해 - 충전중에 요청전류와 출력전류를 비교하여 일정비율 이하로 출력? 모듈이 통신을 안 할 때는 알림메시지(개발자모드에서 알 수 있게)

// CRC 통신 폴트 체크 필요
// 자가 진단을 위해서 모듈 개별제어 통합필요, 모듈 통신에러인지 출력 실패 에러인지 구분 필요
// 충전중에 UI 화면에서 몇 번 스탠드인지 표시 요청(사용자가 에러 화면을 찍어서 보내는데 알 수 없음)


uint8_t rxok_DM_recovery_count;
uint8_t module_init_done;
uint16_t module_init_cnt;
RCC_ClocksTypeDef       rcc_clocks;
int main(void)
{
  System_Init();                        // 모듈에 따라서 CAN filter 다르게 적용
  //Digital_output.O_MC1 = 1;
  RCC_GetClocksFreq(&rcc_clocks);
  U2_Enable;
  Module_Tx_Init();
  Variable_Initialize_1CH();             // GRIDWIZ PLC modem default value
  send_uart_term_system_info();         // 테라텀에서 정상적으로 한글을 보려면 setup - terminal - Coding (receive) KS5601로 선택
  UART1_PutStr("\r\n\n 충전기 메인 프로그램을 시작합니다. \r\n\n");
  
  U2_Disable;
  
  WWDG_Configuration();
  IWDG_Configuration();
  
  HMI_memory_reset();
  
  SIM200_Init();


/*아래는 Control board에서 어떤 계량기 값을 읽어오느냐에 따라 power_meter_tx_data 초기값을 설정하는 부분*/
#if defined (ACPM_DCPM_READ) || defined(ONLY_DCPM_READ) || defined(DISABLE_PM_DATA)
  power_meter_tx_data = acrel_from_0;
#elif defined (ONLY_ACPM_READ)
  power_meter_tx_data = eastron_read_input_registers_1;
#endif
/**************************************************************************************/
  
  make_Txd_Acrel_data_from_0();                 //Acrel DC meter data request buffer (fixed)
  make_read_input_eastron_data_001_049();       //Eastron AC meter data request buffer (fixed)
  make_read_input_eastron_data_053_083();       //Eastron AC meter data request buffer (fixed)
  make_read_input_eastron_data_201_207();       //Eastron AC meter data request buffer (fixed)
  make_read_input_eastron_data_235_255();       //Eastron AC meter data request buffer (fixed)
  make_read_input_eastron_data_335_381();       //Eastron AC meter data request buffer (fixed)

  USART_ITConfig(USART3, USART_IT_TXE, ENABLE); //USART3 송신 가능 인터럽트

  while (1)
  {
    if( _10ms_flag == true) { _10ms_flag = false;}
    if(_100ms_flag == true) { _100ms_flag = false;	 term_get_cmd_excute();}
    
    if(history_flag == 1) fault_history_function();
    
    /* 1ms routine ---------------------------------------------------------------*/
    if(timer_1ms == 1)
    {
      timer_1ms = 0;
      
      Rxd_SCI_2(); // HMI
      Txd_SCI_2(); // HMI
      
      Fault_Check_1CH();

      timer_5ms++;
      timer_10ms++;
      timer_50ms++; 
      timer_100ms++;
      timer_200ms++;
      timer_1s++;
      
      SIM200_ProcessTxPoll();
      SIM200_Process();
//	  if(module_init_done == 0 && Digital_output.O_MC1 == 1)
//	  {
//		module_init_cnt++;
//		Module_Dial_Switch();
//		if(module_init_cnt>=100) 
//		{
//		  
//		  module_init_done = 1;
//		  module_init_cnt = 0;
//		}
//	  }
//	  if(Digital_output.O_MC1 == 0) //20260609 PHE 
//	  {
//		module_init_done = 0;
//	  }
	
    }

    /* 5ms routine ---------------------------------------------------------------*/      
    if(timer_5ms >= 5)
    {
      timer_5ms = 0;
      MODULE_INFO_TRANS();
	  

    }

    /* 10ms routine --------------------------------------------------------------*/      
    if (timer_10ms >= 10)
    {
      timer_10ms = 0; 
      _10ms_flag = true;
      
      timeout_timer_1CH++;
      
      Get_coupler_temperature_1CH();
      
      PLC_3_0_Msg_Send_1CH(); // Gridwiz PLC
      

      
      if(self_check_start == true && sequence_step_1CH == false) 
      {
        Automatic_self_check();
      }
      else
      {
        if(charging_type_select_1CH != load_test) Sequence_control_1CH();
        else if(charging_type_select_1CH == load_test)
        {
          if(Holding_Memory[1].Cable_Type == COMBO_Test)
          {
            Combo_Load_Test_1CH();
          }
          else if(Holding_Memory[1].Cable_Type == AC3)
          {
            V2G_inverter_mode_test();
          }
        }
      }
      
      Digital_Input_Control();
      Digital_Output_Control();
      HMI_tx_data_make();
      HMI_rx_data_parse();
      
      WWDG_SetCounter(127);
    }

    /* 50ms routine --------------------------------------------------------------*/              
    if(timer_50ms >= 50) 
    {
      timer_50ms = 0;

      SIM_100_Transmit();
	  
	  Set_Module_Data();
      Read_Module_Data();
	  
      if(rxok_DM == 0){             //계량기 통신 중간에 중단 시 회복시도
        ++rxok_DM_recovery_count;
        if(rxok_DM_recovery_count >= 100){
          rxok_DM_recovery_count = 0;
          rxok_DM = 1;
        }
      }
      else if(rxok_DM == 1){
        rxok_DM_recovery_count = 0;
      }
    }

    /* 100ms routine -------------------------------------------------------------*/              
    if(timer_100ms >= 100)
    {
      timer_100ms = 0; 
      _100ms_flag = true;
      
      total_operating_time();
      
      if(self_check_start == true && (timer_500ms++ >= 5))  {timer_500ms = 0; LED_DISPLAY_RGBY_TEST();}
      else if(self_check_start == false)                    {                 LED_DISPLAY_and_FAN_Service();}
      
      if(rxok_DM == 1)
      {
        rxok_DM = 0;
        USART_ITConfig(USART3, USART_IT_TXE, ENABLE); // 전력량계 송신 인터럽트 허용
      }
	  
      
      Rxd_Acrel_ch1_data();
      Rxd_Eastron_data_1();
      Rxd_Eastron_data_2();
      Rxd_Eastron_data_3();
      Rxd_Eastron_data_4();
      Rxd_Eastron_data_5();
      
      communication_fault_check(COMMUNICATION_FAULT_CHECK_SET_TIME);   // 10 seconds time setting
      PLC_Reboot_1CH();
      make_random_number();
      IWDG_ReloadCounter();
    }

    /* 200ms routine -------------------------------------------------------------*/
    if(timer_200ms >= 200)
    {
      timer_200ms = 0;      
    }

    /* 1sec routine --------------------------------------------------------------*/              
    if(timer_1s >= 1000)
    {
      timer_1s = 0;
      Acrel_ch1_Communication_Err_cnt++;
    }
  }
}