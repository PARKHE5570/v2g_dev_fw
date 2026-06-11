
#define __SELF_CHECK_C

#include "stdint.h"
#include "stm32f10x.h"
#include "HMI_modbus.h"
#include "main.h"
#include "stm32f10x_it.h"
#include "FirmwareUpdate.h"
//#include "io.h"
#include "INFY_V2G.h"
#include "PLC_3_0.h"
//#include "ctype.h"
#include "pt1000.h"
#include "digital_io.h"
#include "Terminal.h"
#include "fault_check.h"
#include "self_check.h"
#include "SIM_100.h"





uint8_t Automatic_self_check (void)
{
  
  self_check_timer++;
  
  switch(test_step)
  {
  case 0 : 
	UART1_PutStr(T_BrWHITE); UART1_PutStr("\r\n\n 충전기 자가 진단을 시작합니다. ");UART1_PutStr("진단 실패 시 "); UART1_PutStr(T_RED);UART1_PutStr("실패"); 
	UART1_PutStr(T_BrWHITE);UART1_PutStr(" 메시지와 컴퓨터 스피커로"); UART1_PutStr(T_BrGREEN);UART1_PutStr(" 비프음"); UART1_PutStr(T_BrWHITE);UART1_PutStr("이 출력됩니다.\r\n");
	
	UART1_PutStr("\r\n ********************************************** IMPORTANT !! ********************************************** \r");
	UART1_PutStr("\r\n  비상스위치가 눌려있으면 다음 테스트를 진행할 수 없습니다. 비상스위치가 눌리면 회로(물리)적으로 릴레이가 모두 꺼집니다.");
	UART1_PutStr("\r\n ********************************************************************************************************** \r\n");
	
	module_can_communication_start_G1 = 1;
	module_can_communication_start_G2 = 1;
	self_check_timer = 0;
	timeout_timer_1CH = 0; test_step++;
	break;
	
  case 1 : 
	UART1_PutStr("\r\n 0.   [CN20] 1CH LED가 꺼져 있지는 않은지 확인해 주세요. \033[30;41m빨강 -> \033[30;42m초록 -> \033[30;44m파랑 -> \033[30;43m노랑   "); UART1_PutStr(T_RESET); UART1_PutStr(T_BrWHITE); UART1_PutStr("순서로 바뀌는 LED를 직접 확인해 주세요. \n");
	//    UART1_PutStr("\r\n      [CN22] 2CH LED가 꺼져 있지는 않은지 확인해 주세요. \033[30;41m빨강 -> \033[30;42m초록 -> \033[30;44m파랑 -> \033[30;43m노랑   "); UART1_PutStr(T_RESET); UART1_PutStr(T_BrWHITE); UART1_PutStr("순서로 바뀌는 LED를 직접 확인해 주세요. \n");
	UART1_PutStr("\r\n 1.   [CN2] 비상스위치 상태 점검 시작                          -> ");  
	UART1_PutStr(T_RESET);
	timeout_timer_1CH = 0,  test_step++;
	break;
	
  case 2 :
	if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)) 
	{
	  UART1_PutStr("비정상. \n");
	  UART1_PutStr("\r\n  비상스위치가 눌려있으면 다음 테스트를 진행할 수 없습니다. 비상스위치가 눌리면 회로(물리)적으로 릴레이가 모두 꺼집니다.");
	  UART1_PutStr("\r\n  비상 스위치 또는 배선을 점검해주세요. 점검 후 자가 진단 프로그램을 재시작해주세요. \r\n");
	  UART1_PutStr(T_BELL);
	  timeout_timer_1CH = 0, test_step = 59;
	  break;
	}
	else 
	{
	  UART1_PutStr("정상 \n"); check_list_ok++;
	  timeout_timer_1CH = 0,  test_step++;
	}
	break;
	
  case 3:
	UART1_PutStr("\r\n 2.   [CON1_CN19] 릴레이1  ON 상태 진단 시작                 -> "); Digital_output.O_RY1 = 1;
	UART1_PutStr(T_RESET);
	timeout_timer_1CH = 0,  test_step++;
	break;
	
  case 4:
	if(timeout_timer_1CH >= DIGITAL_INPUT_OUTPUT_TIMEOUT_SET)
	{
	  if(!Digital_input.bit.I_RY1) {UART1_PutStr(T_BrRED); UART1_PutStr("실패. \033[0m 릴레이, 제어 배선, 접점 배선을 점검하세요. \r\n"); UART1_PutStr(T_BELL);}
	  else                         {UART1_PutStr("정상 \n"); check_list_ok++; }	
	  UART1_PutStr("\r\n 3.   [CON1_CN19] 릴레이1 OFF 상태 진단 시작                 -> "); Digital_output.O_RY1 = 0;
	  UART1_PutStr(T_RESET);
	  timeout_timer_1CH = 0,  test_step++;
	}
	break;
	
  case 5:
	if(timeout_timer_1CH > DIGITAL_INPUT_OUTPUT_TIMEOUT_SET)
	{							  
	  if(Digital_input.bit.I_RY1)  {UART1_PutStr(T_BrRED); UART1_PutStr("실패. \033[0m 릴레이, 제어 배선, 접점 배선을 점검하세요. \r\n"); UART1_PutStr(T_BELL);}
	  else                         {UART1_PutStr("정상 \n"); check_list_ok++; }
	  
	  UART1_PutStr("\r\n 4.   [CON1_CN33] 릴레이2  ON 상태 진단 시작                 -> "); Digital_output.O_RY2 = 1;
	  UART1_PutStr(T_RESET);
	  timeout_timer_1CH = 0,  test_step++;
	}
	break;
	
  case 6:
	if(timeout_timer_1CH > DIGITAL_INPUT_OUTPUT_TIMEOUT_SET)
	{
	  if(!Digital_input.bit.I_RY2) {UART1_PutStr(T_BrRED); UART1_PutStr("실패. \033[0m 릴레이, 제어 배선, 접점 배선을 점검하세요. \r\n"); UART1_PutStr(T_BELL);}
	  else                         {UART1_PutStr("정상 \n"); check_list_ok++; }
	  
	  UART1_PutStr("\r\n 5.   [CON1_CN33] 릴레이2 OFF 상태 진단 시작                 -> "); Digital_output.O_RY2 = 0;
	  UART1_PutStr(T_RESET);
	  timeout_timer_1CH = 0,  test_step++;
	}
	break;
	
  case 7:
	if(timeout_timer_1CH > DIGITAL_INPUT_OUTPUT_TIMEOUT_SET)
	{							  
	  if(Digital_input.bit.I_RY2) 
	  {
		UART1_PutStr(T_BrRED); UART1_PutStr("실패. \033[0m 릴레이, 제어 배선, 접점 배선을 점검하세요. \r\n"); UART1_PutStr(T_BELL);
	  }
	  else                        
	  {
		UART1_PutStr("정상 \n"); 
		check_list_ok++; 
	  }
	  //		      
	  //		      UART1_PutStr("\r\n 6.   [RY_CON1_CN5] 릴레이5  ON 상태 진단 시작                 -> "); Digital_output.O_RY5 = 1;
	  UART1_PutStr(T_RESET);
	  timeout_timer_1CH = 0,  test_step++;
	}
	break;
	
  case 8:
	//    if(timeout_timer_1CH >= DIGITAL_INPUT_OUTPUT_TIMEOUT_SET)
	//    {
	//      if(!Digital_input.bit.I_RY5) {UART1_PutStr(T_BrRED); UART1_PutStr("실패. \033[0m 릴레이, 제어 배선, 접점 배선을 점검하세요. \r\n"); UART1_PutStr(T_BELL);}
	//      else                         {UART1_PutStr("정상 \n"); check_list_ok++; }	
	//      
	//      UART1_PutStr("\r\n 7.   [RY_CON2_CN5] 릴레이5 OFF 상태 진단 시작                 -> "); Digital_output.O_RY5 = 0;
	//      UART1_PutStr(T_RESET);
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 9:
	//    if(timeout_timer_1CH > DIGITAL_INPUT_OUTPUT_TIMEOUT_SET)
	//    {							  
	//      if(Digital_input.bit.I_RY5)  {UART1_PutStr(T_BrRED); UART1_PutStr("실패. \033[0m 릴레이, 제어 배선, 접점 배선을 점검하세요. \r\n"); UART1_PutStr(T_BELL);}
	//      else                         {UART1_PutStr("정상 \n"); check_list_ok++; }
	//      
	//      UART1_PutStr("\r\n 8.   [RY_CON1_CN6] 릴레이6  ON 상태 진단 시작                 -> "); Digital_output.O_RY6 = 1;
	//      UART1_PutStr(T_RESET);
	//      timeout_timer_1CH = 0;test_step++;
	//    }
	test_step++;
	break;
	
  case 10:
	//    if(timeout_timer_1CH > DIGITAL_INPUT_OUTPUT_TIMEOUT_SET)
	//    {
	//      if(!Digital_input.bit.I_RY6) {UART1_PutStr(T_BrRED); UART1_PutStr("실패. \033[0m 릴레이, 제어 배선, 접점 배선을 점검하세요. \r\n"); UART1_PutStr(T_BELL);}
	//      else                         {UART1_PutStr("정상 \n"); check_list_ok++; }
	//      
	//      UART1_PutStr("\r\n 9.   [RY_CON2_CN6] 릴레이6 OFF 상태 진단 시작                 -> "); Digital_output.O_RY6 = 0;
	//      UART1_PutStr(T_RESET);
	//      timeout_timer_1CH = 0; test_step++;
	//      
	//    }
	test_step++;
	break;
	
  case 11:
	//    if(timeout_timer_1CH > DIGITAL_INPUT_OUTPUT_TIMEOUT_SET)
	//    {
	//      if(Digital_input.bit.I_RY6) {UART1_PutStr(T_BrRED); UART1_PutStr("실패. \033[0m 릴레이, 제어 배선, 접점 배선을 점검하세요. \r\n"); UART1_PutStr(T_BELL);}
	//      else                        {UART1_PutStr("정상 \n"); check_list_ok++; }
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 12:
	
	//		if(timeout_timer_1CH > DIGITAL_INPUT_OUTPUT_TIMEOUT_SET)
	//		{	
	//			UART1_PutStr("\r\n 6.  [CN28] 출력 퓨즈 오픈 상태 진단 시작 ");		
	//			UART1_PutStr(T_RESET);
	//			if(Digital_input.bit.I_FUSE_1CH) 
	//			{
	//				UART1_PutStr(T_BrRED); 
	//				UART1_PutStr("\r\n  -> 실패.\033[0m [CN28] 1채널 퓨즈 오픈이 감지되었습니다. 퓨즈 또는 배선을 점검해주세요. 퓨즈 접점 스위치의 1번과 4번에 기본 열림(Normal Open)으로 연결되어있어야 합니다. \r\n"); 
	//				UART1_PutStr(T_BELL);
	//			}
	//			else                              
	//			{
	//				UART1_PutStr("\r\n  -> 1채널 퓨즈: 정상 \n");
	//				check_list_ok++;
	//			}
	//			      if(Digital_input.bit.I_FUSE_2CH) {UART1_PutStr(T_BrRED); UART1_PutStr("\r\n  -> 실패.\033[0m [CN29] 2채널 퓨즈 오픈이 감지되었습니다. 퓨즈 또는 배선을 점검해주세요. 퓨즈 접점 스위치의 1번과 4번에 기본 열림(Normal Open)으로 연결되어있어야 합니다. \r\n"); UART1_PutStr(T_BELL);}
	//			      else                              {UART1_PutStr("\r\n  -> 2채널 퓨즈: 정상 \n"); check_list_ok++; }		  
	//			timeout_timer_1CH = 0; test_step++;
	//		}
	test_step++;
	break;	
	
  case 13:
	
	UART1_PutStr("\r\n 7.  [CN17] 1채널 충전 커플러 온도를 체크합니다.              -> "); 
	UART1_PutStr(T_RESET);
	if(coupler_temperature_P_1CH == 1) 
	{
	  UART1_PutStr(T_BrRED); UART1_PutStr("실패.\033[0m [CN17]에 커플러 온도 센서(PT1000)가 오픈되었거나 온도가 영하입니다. 연결을 확인해주세요.\n"); UART1_PutStr(T_BELL);
	}
	else
	{
	  UART1_PutStr("정상  "); UART1_PutDec(3, (uint16_t)coupler_temperature_P_1CH); UART1_PutStr(" [℃] \n");
	}
	timeout_timer_1CH = 0; test_step++;							
	break;
	
  case 14:
	//    UART1_PutStr("\r\n 12.  [CN18] 2채널 충전 커플러 온도를 체크합니다.              -> "); 
	//    UART1_PutStr(T_RESET);
	//    if(coupler_temperature_P_2CH == 1) 
	//    {
	//      UART1_PutStr(T_BrRED); UART1_PutStr("실패.\033[0m [CN18]에 커플러 온도 센서(PT1000)가 오픈되었거나 온도가 영하입니다. 연결을 확인해주세요.\n"); UART1_PutStr(T_BELL);
	//    }
	//    else
	//    {
	//      UART1_PutStr("정상  "); UART1_PutDec(3, (uint16_t)coupler_temperature_P_2CH); UART1_PutStr(" [℃] \n");
	//    }
	//    timeout_timer_1CH = 0; test_step++;
	test_step++;
	break;
	
  case 15:
	
	UART1_PutStr("\r\n 8.  [CN11] 1채널 PLC 모뎀 CAN 통신 진단 시작                 -> "); 
	UART1_PutStr(T_RESET);
	if(plc_can_communication_rx_fault_1CH == _rx_non_fault) 
	{
	  UART1_PutStr("정상  ");
	  UART1_PutStr("CP:");
	  
	  if(SECC_Msg[1].SECC_Status_2.cpVoltage_12 < 0) {UART1_PutStr("-");UART1_PutDec(3, ~(SECC_Msg[1].SECC_Status_2.cpVoltage_12)+1); UART1_PutStr(" [V] \n");} // short형 
	  else                                           {UART1_PutStr(" ");UART1_PutDec(3, (u16)SECC_Msg[1].SECC_Status_2.cpVoltage_12); UART1_PutStr(" [V] \n");}					
	}
	else
	{
	  UART1_PutStr(T_BrRED); UART1_PutStr("실패.\033[0m 1.모뎀 전원 확인, 2.CAN 배선 점검, 3.모뎀 부팅 확인 \n "); UART1_PutStr(T_BELL);
	}
	timeout_timer_1CH = 0; test_step++;							
	break;
	
  case 16:
	//    
	//    UART1_PutStr("\r\n 14.  [CN11] 2채널 PLC 모뎀 CAN 통신 진단 시작                 -> "); 
	//    UART1_PutStr(T_RESET);
	//    if(plc_can_communication_rx_fault_2CH == _rx_non_fault) 
	//    {
	//      UART1_PutStr("정상  ");
	//      UART1_PutStr("CP:");
	//      
	//      if(SECC_Msg[2].SECC_Status_2.cpVoltage_12 < 0) {UART1_PutStr("-");UART1_PutDec(3, ~(SECC_Msg[2].SECC_Status_2.cpVoltage_12)+1); UART1_PutStr(" [V] \n");} // short형 
	//      else                                           {UART1_PutStr(" ");UART1_PutDec(3, (u16)SECC_Msg[2].SECC_Status_2.cpVoltage_12); UART1_PutStr(" [V] \n");}					
	//    }
	//    else
	//    {
	//      UART1_PutStr(T_BrRED); UART1_PutStr("실패.\033[0m 1.모뎀 전원 확인, 2.CAN 배선 점검, 3.모뎀 부팅 확인 \n "); UART1_PutStr(T_BELL);
	//    }
	//    timeout_timer_1CH = 0; test_step++;
	test_step++;
	break;    
	
  case 17:
	
	UART1_PutStr("\r\n 9.  [CN6] 터치PC 485 통신 체크 시작                          -> "); 
	UART1_PutStr(T_RESET);
	// 케이블 연결 확인(수신 에러) or CRC 에러로 구분
	
	if(hmi_485_communication_rx_fault == _rx_fault) 
	{
	  UART1_PutStr(T_BrRED); 
	  UART1_PutStr("실패.\033[0m 1. 터치PC 충전기 프로그램이 실행 확인, 2. 통신 포트 설정 확인, 3. D+, D- 배선 확인\n"); 
	  UART1_PutStr(T_BELL);
	}
	else
	{
	  UART1_PutStr("정상 \n");
	}
	timeout_timer_1CH = 0; test_step++;
	break;
	
  case 18:
	
	//    UART1_PutStr("\r\n 15.  [CN10] IMD 485 통신 체크 시작                            -> "); 
	//    UART1_PutStr(T_RESET);
	//    
	//    if(imd_485_communication_rx_fault == _rx_fault) {UART1_PutStr(T_BrRED); UART1_PutStr("실패.\033[0m 1. 컨트롤보드[CN10]에서 IMD통신 보드[CN2]로 연결된 배선 확인 2. D+, D- 배선 확인\n"); UART1_PutStr(T_BELL);}
	//    else                                             UART1_PutStr("정상 \n");
	//    timeout_timer_1CH = 0; test_step++;
	test_step++;
	break;
	
  case 19: 
	
	UART1_PutStr("\r\n 10.  [CN7] MODULE1 CAN 통신 진단                              -> ");
	UART1_PutStr(T_RESET);
	timeout_timer_1CH = 0; test_step++;
	break;
	
  case 20:
	
	if(module1_can_communication_rx_fault == _rx_non_fault)
	{
	  UART1_PutStr("정상 \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	else
	{
	  UART1_PutStr(T_BrRED); UART1_PutStr("통신 실패,\033[0m ");
	  UART1_PutStr("  1. CAN 배선 점검, 2. 모듈 통신 커넥터 접촉 확인, 3. 종단 저항 확인, 4. 모듈 전원 확인 "); UART1_PutStr("\a \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	break;
	
  case 21: 
	
	UART1_PutStr("\r\n 11.  [CN11] IMD 상태 진단                              -> ");
	UART1_PutStr(T_RESET);
	timeout_timer_1CH = 0; 
	test_step++;
	break;
	
  case 22:
	
	if(SIM_info.SIM_Isolation_State.Status_Bits.IS1_IS0 == 0)
	{
	  UART1_PutStr("정상 \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	else
	{
	  UART1_PutStr(T_BrRED); UART1_PutStr("통신 실패,\033[0m ");
	  UART1_PutStr("  1. IMD 배선 점검, 2. IMD 커넥터 접촉 및 결선 확인  "); UART1_PutStr("\a \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	break;
	
  case 23: 		   
	
	UART1_PutStr("\r\n 12.  [CN10] DC Meter RS485 통신 진단                              -> ");
	UART1_PutStr(T_RESET);
	timeout_timer_1CH = 0;
	test_step++;
	break;
	
  case 24:
	
	if(dc_meter_communication_rx_fault == _rx_non_fault)
	{
	  UART1_PutStr("정상 \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	else if(dc_meter_communication_rx_fault == _rx_fault)
	{
	  UART1_PutStr(T_BrRED); UART1_PutStr("통신 실패,\033[0m ");
	  UART1_PutStr("  1. DC meter 통신 배선 점검, 2. 커넥터 접촉 확인, 3. 종단 저항 확인, 4. 전원 확인 "); UART1_PutStr("\a \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	break;
	
  case 25: 
	
	UART1_PutStr("\r\n 13.  [CN12] AC Meter RS485 통신 진단                              -> ");
	UART1_PutStr(T_RESET);
	timeout_timer_1CH = 0;
	test_step++;
	break;
	
  case 26:
	
	if(ac_meter_communication_rx_fault == _rx_non_fault)
	{
	  UART1_PutStr("정상 \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	else if(ac_meter_communication_rx_fault == _rx_fault)
	{
	  UART1_PutStr(T_BrRED); UART1_PutStr("통신 실패,\033[0m ");
	  UART1_PutStr("  1. AC meter 통신 배선 점검, 2. 커넥터 접촉 확인, 3. 종단 저항 확인, 4. 전원 확인 "); UART1_PutStr("\a \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	break;
	
  case 27: 
	
	UART1_PutStr("\r\n 14.  [CN11] Sendyne IMD CAN 통신 진단                              -> ");
	UART1_PutStr(T_RESET);
	timeout_timer_1CH = 0;
	test_step++;
	break;
	
  case 28:
	
	if(imd_can_communication_rx_fault == _rx_non_fault)
	{
	  UART1_PutStr("정상 \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	else if(imd_can_communication_rx_fault == _rx_fault)
	{
	  UART1_PutStr(T_BrRED); UART1_PutStr("통신 실패,\033[0m ");
	  UART1_PutStr("  1. CAN 배선 점검, 2. IMD 커넥터 접촉 확인, 3. IMD 배선 연결 확인 "); UART1_PutStr("\a \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	break;
	
  case 29: 
	
	//    UART1_PutStr("\r\n 21.  [CN7] MODULE6 CAN 통신 진단                              -> ");
	//    UART1_PutStr(T_RESET);
	//    timeout_timer_2CH = 0; test_step++;
	test_step++;
	break;
	
  case 30:
	
	//    if(module6_can_communication_rx_fault == _rx_non_fault)
	//    {
	//      UART1_PutStr("정상 \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	//    else
	//    {
	//      UART1_PutStr(T_BrRED); UART1_PutStr("통신 실패,\033[0m ");
	//      UART1_PutStr("  1. CAN 배선 점검, 2. 모듈 통신 커넥터 접촉 확인, 3. 종단 저항 확인, 4. 모듈 전원 확인 "); UART1_PutStr("\a \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 31: 
	//    
	//    UART1_PutStr("\r\n 22.  [CN7] MODULE7 CAN 통신 진단                              -> ");
	//    UART1_PutStr(T_RESET);
	//    timeout_timer_2CH = 0; test_step++;
	test_step++;
	break;
	
  case 32:
	//  
	//    if(module7_can_communication_rx_fault == _rx_non_fault)
	//    {
	//      UART1_PutStr("정상 \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	//    else
	//    {
	//      UART1_PutStr(T_BrRED); UART1_PutStr("통신 실패,\033[0m ");
	//      UART1_PutStr("  1. CAN 배선 점검, 2. 모듈 통신 커넥터 접촉 확인, 3. 종단 저항 확인, 4. 모듈 전원 확인 "); UART1_PutStr("\a \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 33: 
	
	//    UART1_PutStr("\r\n 23.  [CN7] MODULE8 CAN 통신 진단                              -> ");
	//    UART1_PutStr(T_RESET);
	//    timeout_timer_2CH = 0; test_step++;
	test_step++;
	break;
	
  case 34:
	//  
	//    if(module8_can_communication_rx_fault == _rx_non_fault)
	//    {
	//      UART1_PutStr("정상 \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	//    else
	//    {
	//      UART1_PutStr(T_BrRED); UART1_PutStr("통신 실패,\033[0m ");
	//      UART1_PutStr("  1. CAN 배선 점검, 2. 모듈 통신 커넥터 접촉 확인, 3. 종단 저항 확인, 4. 모듈 전원 확인 "); UART1_PutStr("\a \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 35: //1채널 모듈 ON 테스트
	
	Digital_output.O_RY1 = 1; Digital_output.O_RY2 = 1;
	module_group = 1; module_rectification = 1; module_on_off_G1 = 1; Vout_ref_temp_G1 = TEST_VOLTAGE_COMMAND; Iout_ref_temp_G1 = 5;
	
	UART1_PutStr("\r\n 15.  [CN7] 1채널 모듈 테스트 \n");
	UART1_PutStr("\r\n         Module ON 테스트, Vout Command = 750[V]\n");
	UART1_PutStr(T_RESET);
	timeout_timer_1CH = 0; test_step++;
	break;
	
  case 36:
	
	if(timeout_timer_1CH >= AFTER_5_SECONDS && module_info[0].ufVout.fValue >= (Vout_ref_temp_G1 * .9))
	{
	  UART1_PutStr("\r\n         Module 1 , Vout   : ");UART1_PutDec(3, (u16)(module_info[0].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	else if(timeout_timer_1CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[0].ufVout.fValue < (Vout_ref_temp_G1 * .9)) 
	{
	  UART1_PutStr(T_BrRED); 
	  UART1_PutStr("\r\n         Module 1 , Vout   : ");UART1_PutDec(3, (u16)(module_info[0].ufVout.fValue)); UART1_PutStr("[V]   -> 출력 실패.\033[0m \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	break;
	
  case 37: 
	
	//    if(timeout_timer_1CH >= AFTER_5_SECONDS && module_info[2].ufVout.fValue >= (Vout_ref_temp_G1 * .9))
	//    {
	//      UART1_PutStr("\r\n         Module 2 , Vout   : ");UART1_PutDec(3, (u16)(module_info[2].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_1CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[2].ufVout.fValue < (Vout_ref_temp_G1 * .9)) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 2 , Vout   : ");UART1_PutDec(3, (u16)(module_info[2].ufVout.fValue)); UART1_PutStr("[V]   -> 출력 실패.\033[0m \n");
	//      timeout_timer_1CH = 0; test_step++;
	//	}
	test_step++;
	break;
	
  case 38:
	
	//    if(timeout_timer_1CH >= AFTER_5_SECONDS && module_info[3].ufVout.fValue >= (Vout_ref_temp_G1 * .9))
	//    {
	//      UART1_PutStr("\r\n         Module 3 , Vout   : ");UART1_PutDec(3, (u16)(module_info[3].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_1CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[3].ufVout.fValue < (Vout_ref_temp_G1 * .9)) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 3 , Vout   : ");UART1_PutDec(3, (u16)(module_info[3].ufVout.fValue)); UART1_PutStr("[V]   -> 출력 실패.\033[0m \n");
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	test_step++;
	break; 
	
  case 39:
	
	//    if(timeout_timer_1CH >= AFTER_5_SECONDS && module_info[4].ufVout.fValue >= (Vout_ref_temp_G1 * .9))
	//    {
	//      UART1_PutStr("\r\n         Module 4 , Vout   : ");UART1_PutDec(3, (u16)(module_info[4].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_1CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[4].ufVout.fValue < (Vout_ref_temp_G1 * .9)) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 4 , Vout   : ");UART1_PutDec(3, (u16)(module_info[4].ufVout.fValue)); UART1_PutStr("[V]   -> 출력 실패.\033[0m \n");
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 40:
	
	//    if(Vout_G1 >= (Vout_ref_temp_G1 * .9))
	//    {
	//      UART1_PutStr("\r\n         IMD Sensing , Vout   : ");UART1_PutDec(3, (u16)(Vout_G1)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_1CH >= MODULE_ON_OFF_TIMEOUT_SET && Vout_G1 < (Vout_ref_temp_G1 * .9)) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         IMD Sensing , Vout   : ");UART1_PutDec(3, (u16)(Vout_G1)); UART1_PutStr("[V]   -> 출력 감지 실패.\033[0m ");
	//      UART1_PutStr("IMD의 IMD1_VX1 배선과 IMD1_VX2 배선 점검"); UART1_PutStr("\a \n");
	//      
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 41: //1채널 모듈 OFF테스트
	
	Digital_output.O_RY1 = 1; Digital_output.O_RY2 = 1;
	module_group = 1; module_rectification = 0; module_on_off_G1 = 0; Vout_ref_temp_G1 = 150; Iout_ref_temp_G1 = 5;
	
	UART1_PutStr("\r\n                         \n");
	UART1_PutStr("\r\n         Module OFF 테스트\n");
	UART1_PutStr(T_RESET);
	timeout_timer_1CH = 0; test_step++;
	break;
	
  case 42: 
	
	if(timeout_timer_1CH >= AFTER_5_SECONDS && module_info[0].ufVout.fValue < 50)
	{
	  UART1_PutStr("\r\n         Module 1 , Vout   : ");UART1_PutDec(3, (u16)(module_info[0].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	else if(timeout_timer_1CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[1].ufVout.fValue >= 50) 
	{
	  UART1_PutStr(T_BrRED); 
	  UART1_PutStr("\r\n         Module 1 , Vout   : ");UART1_PutDec(3, (u16)(module_info[0].ufVout.fValue)); UART1_PutStr("[V]   -> 방전 실패.\033[0m \n");
	  timeout_timer_1CH = 0; test_step++;
	}
	break;
	
  case 43: 
	
	//    if(timeout_timer_1CH >= AFTER_5_SECONDS && module_info[2].ufVout.fValue < 50)
	//    {
	//      UART1_PutStr("\r\n         Module 2 , Vout   : ");UART1_PutDec(3, (u16)(module_info[2].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_1CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[2].ufVout.fValue >= 50) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 2 , Vout   : ");UART1_PutDec(3, (u16)(module_info[2].ufVout.fValue)); UART1_PutStr("[V]   -> 방전 실패.\033[0m \n");
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 44:
	
	//    if(timeout_timer_1CH >= AFTER_5_SECONDS && module_info[3].ufVout.fValue < 50)
	//    {
	//      UART1_PutStr("\r\n         Module 3 , Vout   : ");UART1_PutDec(3, (u16)(module_info[3].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_1CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[3].ufVout.fValue >= 50) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 3 , Vout   : ");UART1_PutDec(3, (u16)(module_info[3].ufVout.fValue)); UART1_PutStr("[V]   -> 방전 실패.\033[0m \n");
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 45: 
	
	//    if(timeout_timer_1CH >= AFTER_5_SECONDS && module_info[4].ufVout.fValue < 50)
	//    {
	//      UART1_PutStr("\r\n         Module 4 , Vout   : ");UART1_PutDec(3, (u16)(module_info[4].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_1CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[4].ufVout.fValue >= 50) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 4 , Vout   : ");UART1_PutDec(3, (u16)(module_info[4].ufVout.fValue)); UART1_PutStr("[V]   -> 방전 실패.\033[0m \n");
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 46: 
	
	//    if(timeout_timer_1CH >= AFTER_5_SECONDS && Vout_G1 < 50)
	//    {
	//      Digital_output.O_RY1 = 0; Digital_output.O_RY2 = 0;    
	//      
	//      UART1_PutStr("\r\n         IMD Sensing , Vout   : ");UART1_PutDec(3, (u16)(Vout_G1)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_1CH >= MODULE_ON_OFF_TIMEOUT_SET && Vout_G1 >= 50) 
	//    {
	//      Digital_output.O_RY1 = 0; Digital_output.O_RY2 = 0;    
	//      
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         IMD Sensing , Vout   : ");UART1_PutDec(3, (u16)(Vout_G1)); UART1_PutStr("[V]   -> 방전 실패.\033[0m \n");
	//      UART1_PutStr("특정 모듈에서 출력이 나오고 있습니다. 확인 필요"); UART1_PutStr("\a \n");
	//      
	//      timeout_timer_1CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 47: //2채널 모듈 ON 테스트
	
	//    Digital_output.O_RY5 = 1; Digital_output.O_RY6 = 1;
	//    module_group = 2; module_on_off_G2 = 1; Vout_ref_temp_G2 = TEST_VOLTAGE_COMMAND; Iout_ref_temp_G2 = 5;
	//    
	//    UART1_PutStr("\r\n 25.  [CN7] 2채널 모듈 테스트 \n");
	//    UART1_PutStr("\r\n         Module ON 테스트, Vout Command = 750[V]\n");
	//    UART1_PutStr(T_RESET);
	//    timeout_timer_2CH = 0; test_step++;
	test_step++;
	break;
	
  case 48:
	
	//    if(timeout_timer_2CH >= AFTER_5_SECONDS && module_info[5].ufVout.fValue >= (Vout_ref_temp_G2 * .9))
	//    {
	//      UART1_PutStr("\r\n         Module 5 , Vout   : ");UART1_PutDec(3, (u16)(module_info[5].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_2CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[5].ufVout.fValue < (Vout_ref_temp_G2 * .9)) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 5 , Vout   : ");UART1_PutDec(3, (u16)(module_info[5].ufVout.fValue)); UART1_PutStr("[V]   -> 출력 실패.\033[0m \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 49: 
	
	//    if(timeout_timer_2CH >= AFTER_5_SECONDS && module_info[6].ufVout.fValue >= (Vout_ref_temp_G2 * .9))
	//    {
	//      UART1_PutStr("\r\n         Module 6 , Vout   : ");UART1_PutDec(3, (u16)(module_info[6].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_2CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[6].ufVout.fValue < (Vout_ref_temp_G2 * .9)) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 6 , Vout   : ");UART1_PutDec(3, (u16)(module_info[6].ufVout.fValue)); UART1_PutStr("[V]   -> 출력 실패.\033[0m \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 50: 
	
	//    if(timeout_timer_2CH >= AFTER_5_SECONDS && module_info[7].ufVout.fValue >= (Vout_ref_temp_G2 * .9))
	//    {
	//      UART1_PutStr("\r\n         Module 7 , Vout   : ");UART1_PutDec(3, (u16)(module_info[7].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_2CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[7].ufVout.fValue < (Vout_ref_temp_G2 * .9)) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 7 , Vout   : ");UART1_PutDec(3, (u16)(module_info[7].ufVout.fValue)); UART1_PutStr("[V]   -> 출력 실패.\033[0m \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	test_step++;
	break;  
	
  case 51: 
	
	//    if(timeout_timer_2CH >= AFTER_5_SECONDS && module_info[8].ufVout.fValue >= (Vout_ref_temp_G2 * .9))
	//    {
	//      UART1_PutStr("\r\n         Module 8 , Vout   : ");UART1_PutDec(3, (u16)(module_info[8].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_2CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[8].ufVout.fValue < (Vout_ref_temp_G2 * .9)) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 8 , Vout   : ");UART1_PutDec(3, (u16)(module_info[8].ufVout.fValue)); UART1_PutStr("[V]   -> 출력 실패.\033[0m \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 52:   
	
	//    if(Vout_G2 >= (Vout_ref_temp_G2 * .9))
	//    {
	//      UART1_PutStr("\r\n         IMD Sensing , Vout   : ");UART1_PutDec(3, (u16)(Vout_G2)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_2CH >= MODULE_ON_OFF_TIMEOUT_SET && Vout_G2 < (Vout_ref_temp_G2 * .9)) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         IMD Sensing , Vout   : ");UART1_PutDec(3, (u16)(Vout_G2)); UART1_PutStr("[V]   -> 출력 감지 실패.\033[0m ");
	//      UART1_PutStr("IMD의 IMD2_VX1 배선과 IMD2_VX2 배선 점검"); UART1_PutStr("\a \n");
	//      
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 53: //2채널 모듈 OFF테스트
	
	//    Digital_output.O_RY5 = 1; Digital_output.O_RY6 = 1;
	//    module_group = 2; module_on_off_G2 = 0; Vout_ref_temp_G2 = 200; Iout_ref_temp_G2 = 5;
	//    
	//    UART1_PutStr("\r\n                         \n");
	//    UART1_PutStr("\r\n         Module OFF 테스트\n");
	//    UART1_PutStr(T_RESET);
	//    timeout_timer_2CH = 0; test_step++;
	test_step++;
	break;
	
  case 54: 
	
	//    if(timeout_timer_2CH >= AFTER_5_SECONDS && module_info[5].ufVout.fValue < 50)
	//    {
	//      UART1_PutStr("\r\n         Module 5 , Vout   : ");UART1_PutDec(3, (u16)(module_info[5].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_2CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[5].ufVout.fValue >= 50) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 5 , Vout   : ");UART1_PutDec(3, (u16)(module_info[5].ufVout.fValue)); UART1_PutStr("[V]   -> 방전 실패.\033[0m \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 55:
	
	//    if(timeout_timer_2CH >= AFTER_5_SECONDS && module_info[6].ufVout.fValue < 50)
	//    {
	//      UART1_PutStr("\r\n         Module 6 , Vout   : ");UART1_PutDec(3, (u16)(module_info[6].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_2CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[6].ufVout.fValue >= 50) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 6 , Vout   : ");UART1_PutDec(3, (u16)(module_info[6].ufVout.fValue)); UART1_PutStr("[V]   -> 방전 실패.\033[0m \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 56:
	
	//    if(timeout_timer_2CH >= AFTER_5_SECONDS && module_info[7].ufVout.fValue < 50)
	//    {
	//      UART1_PutStr("\r\n         Module 7 , Vout   : ");UART1_PutDec(3, (u16)(module_info[7].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_2CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[7].ufVout.fValue >= 50) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 7 , Vout   : ");UART1_PutDec(3, (u16)(module_info[7].ufVout.fValue)); UART1_PutStr("[V]   -> 방전 실패.\033[0m \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 57: 
	
	//    if(timeout_timer_2CH >= AFTER_5_SECONDS && module_info[8].ufVout.fValue < 50)
	//    {
	//      UART1_PutStr("\r\n         Module 8 , Vout   : ");UART1_PutDec(3, (u16)(module_info[8].ufVout.fValue)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_2CH >= MODULE_ON_OFF_TIMEOUT_SET && module_info[8].ufVout.fValue >= 50) 
	//    {
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         Module 8 , Vout   : ");UART1_PutDec(3, (u16)(module_info[8].ufVout.fValue)); UART1_PutStr("[V]   -> 방전 실패.\033[0m \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 58: 
	
	//    if(timeout_timer_2CH >= AFTER_5_SECONDS && Vout_G2 < 50)
	//    {
	//      Digital_output.O_RY5 = 0;Digital_output.O_RY6 = 0;
	//      
	//      UART1_PutStr("\r\n         IMD Sensing , Vout   : ");UART1_PutDec(3, (u16)(Vout_G2)); UART1_PutStr("[V]   -> 정상. \n");
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	//    else if(timeout_timer_2CH >= MODULE_ON_OFF_TIMEOUT_SET && Vout_G2 >= 50) 
	//    {
	//      Digital_output.O_RY5 = 0;Digital_output.O_RY6 = 0;
	//      
	//      UART1_PutStr(T_BrRED); 
	//      UART1_PutStr("\r\n         IMD Sensing , Vout   : ");UART1_PutDec(3, (u16)(Vout_G2)); UART1_PutStr("[V]   -> 방전 실패.\033[0m \n");
	//      UART1_PutStr("특정 모듈에서 출력이 나오고 있습니다. 확인 필요"); UART1_PutStr("\a \n");
	//      
	//      timeout_timer_2CH = 0; test_step++;
	//    }
	test_step++;
	break;
	
  case 59:
	UART1_PutStr("\r\n 16.   [CN21] DC FAN 동작 확인                 -> ");
	UART1_PutStr(T_RESET);
	
	UART1_PutStr(T_BrRED);
	UART1_PutStr("FAN 10초간 정상 가동(육안 확인). \033[0m FAN이 동작하지 않을 시, FAN 배선 점검 \r\n");
	UART1_PutStr(T_RESET);
	test_step++;
	
  case 60:
	
	if(timeout_timer_1CH <= FAN_CHECK_COUNT){
	  //Digital_output.O_UVLED_1CH = 1;
	}
	else{
	  //Digital_output.O_UVLED_1CH = 0;
	  timeout_timer_1CH = 0,  test_step++;
	}
	break;
	
  case 61:
	UART1_PutStr("\r\n 17.   [CN27] DOOR SENSOR 동작 확인(Door 열어놓고 진행 시 열림표시)                 -> ");
	UART1_PutStr(T_RESET);
	
	test_step++;
	
  case 62:
	
	if(Digital_input.bit.I_DOOR){
	  UART1_PutStr(T_BrRED); 
	  UART1_PutStr("Door 열림.\033[0m Door 열림 상태 확인 \n");
	  UART1_PutStr(T_RESET);
	}
	else{
	  UART1_PutStr("Door 닫힘.\033[0m Door 닫힘 상태 확인 \n");
	  UART1_PutStr(T_RESET);
	}
	timeout_timer_1CH = 0,  test_step++;
	break;
	
  case 63:
	UART1_PutStr("\r\n 18.   [CN28] SPD 접점 확인                 -> ");
	UART1_PutStr(T_RESET);
	
	test_step++;
	
  case 64:
	
	if(Digital_input.bit.I_FUSE_8){ 
	  UART1_PutStr("정상.\n");
	  UART1_PutStr(T_RESET);
	}
	else{
	  UART1_PutStr(T_BrRED);
	  UART1_PutStr("실패.\033[0m 1. FUSE 8 확인, 2. FUSE 8 접점 커넥터 확인, 3. FUSE 8 접점 배선 확인 \n");
	  UART1_PutStr(T_RESET);
	}
	
	if(Digital_input.bit.I_FUSE_9){ 
	  UART1_PutStr("정상.\n");
	  UART1_PutStr(T_RESET);
	}
	else{
	  UART1_PutStr(T_BrRED);
	  UART1_PutStr("실패.\033[0m 1. FUSE 9 확인, 2. FUSE 9 접점 커넥터 확인, 3. FUSE 9 접점 배선 확인 \n");
	  UART1_PutStr(T_RESET);
	}
	timeout_timer_1CH = 0,  test_step++;
	break;
	
	
  case 65:
	
	Digital_output.O_RY1 = 0; Digital_output.O_RY2 = 0; 
	//    module_can_communication_start_G1 = 0; module_can_communication_start_G2 = 0;
	//						UART1_PutStr("\r\n ****************************** 로고 색상은 항상 바뀝니다. ****************************** \n"); 
	UART1_PutStr("\r\n *************************************************************************************** \n"); 
	UART1_PutStr(evsis_text1);
	UART1_PutStr(evsis_text2);
	UART1_PutStr(evsis_text3);
	UART1_PutStr(evsis_text4);
	UART1_PutStr(evsis_text5);
	UART1_PutStr(evsis_text6);
	UART1_PutStr(evsis_text7);
	
	self_check_start = false;
	test_step = 0;
	
	UART1_PutStr("\r\n ******************************* 자가 진단이 끝났습니다. ******************************* \n"); 
	UART1_PutStr("\r\n \033[36m충전기 메인 프로그램으로 복귀합니다. \033[0m\r\n");
	if(check_list_ok == 47) return 1;
	else                    return 0;
	//					 UART1_PutStr("\r\n 9. [CN50] PLC CAN COMMUNICATION CHECK START             ");	
	break;
  }
  return 0;
}