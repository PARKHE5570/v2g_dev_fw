#define __SEQUENCE_C

#include "main.h"
#include "sequence.h"
#include "stm32f105_init.h"
#include "PLC_3_0.h"
#include "INFY_V2G.h"
#include "HMI_modbus.h"
#include "Terminal.h"
#include "digital_io.h"
#include "fault_check.h"
#include "SIM_100.h"
#include "UART.h"
#include "pt1000.h"
#include "sim200_protocol.h"
#include "sim200_can_sched.h"
//#include "io.h"
//#include "ADC.h"



void Sequence_control_1CH()
{
  Power_Control_Value_1CH();
  EVSE_Msg[1].EVSE_Present_Voltage_Current.evsePresentVoltage = (uint16_t)(Vout_G1 * 10);
  EVSE_Msg[1].EVSE_Present_Voltage_Current.evsePresentCurrent = (uint16_t)(Iout_Module_G1 * 10);
  
  switch (sequence_step_1CH)
  {
  case 0:
	eSysState_1CH = STATE_STANDBY; // blue led on
	
	if(control_pilot_1CH) module_on_off_G1 = MODULE_OFF;
	//    my_stand_standby_1CH = 1; // 대기중, 나머지 스탠드 충전 가능
	//    user_stop = 0;
	//    charging_restart_counter = 0;
	//    timer_step3_1CH = 0;
	//    restart_flag_1CH = 0;
	//    send_uart_term_start = 0;
	
	Input_Memory[1].Charger_Status.bit.Charging_Run = 0; //Charging RUN
	Input_Memory[1].Charger_Status.bit.Charging_Finish = 0; //Charging Finish
	Input_Memory[1].Charger_Status.bit.Controlboard_Ready = 1; //DSP Ready  
	Input_Memory[1].Charger_Status.bit.Charger_Ready = 1; //Charging Ready
	module_can_communication_start_G1 = 1;
	
	Variable_Initialize_1CH();
	
	EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.triggerStateF = Enabled;  // - 12V
	if (Combo_start_button_1CH)              // Initialize_parameters
	{
	  stop_code_1CH = 0;                      // variable clear
	  send_uart_term_start = 1;           // for logging
	  my_stand_standby_1CH = 0;               // 충전중, 나머지 스탠드 충전 불가
	  EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.triggerStateF = Disabled; // 12V -> 9V
	  EVSE_Msg[1].EVSE_Status.chargingControl_1 = Initialize_parameters; // 2023/03/23
	  
	  Init_RxMes(&RxMessage2, CAN_ID_EXT); // 필요한가?
	  Init_TxMes(&TxMessage2, CAN_ID_EXT); // 필요한가?
	  
	  Input_Memory[1].Charger_Status.bit.Controlboard_Ready = 0; //DSP Ready  
	  Input_Memory[1].Charger_Status.bit.Charger_Ready = 0; //Charging Ready
	  
	  sequence_step_1CH = 998;
	  timeout_timer_1CH = 0;
	  dc_electric_energy = 0;
	  
	  SOC_1CH = 0;
	  SOC_old = 0;
	  start_SOC = 0;
	}			
	
	
	
	break;
	
  case 998:
	if(timeout_timer_1CH >= 500) {sequence_step_1CH = 999; timeout_timer_1CH = 0;}
	break;
	
  case 999: // Start_charging
	if((SECC_Msg[1].SECC_Status_1.seccStatusCode == Idle) || (SECC_Msg[1].SECC_Status_1.seccStatusCode == Ready))
	{
	  EVSE_Msg[1].EVSE_Status.chargingControl_1 = Start_charging; // PWM ON
	  restart_flag_1CH = 0;
	  sequence_step_1CH = 1020;
	  timeout_timer_1CH = 0;
	}
	else if(timeout_timer_1CH > 3000)
	{
	  Input_Memory[1].ERROR1.bit.PLC_Idle_Error = 1;  //PLC 초기화 에러
	  sequence_step_1CH = Sequence_Stop; 
	}
	break;
	
  case 1020: // SLAC
	if(SECC_Msg[1].SECC_Status_1.seccStatusCode == High_Level_Comm || (SECC_Msg[1].SECC_Status_1.seccStatusCode == Authorization_ContractAuthentication))
	{
	  Input_Memory[1].Charger_Status.bit.Charging_Run = 1; //Charging RUN, CATL battery 를 사용한 중국 버스는 SessionSetup 후에 바로 타 채널 pwm을 시작해야 한다.(ev relay 3ea, -는 공통)
	  
	  if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Authorization_ContractAuthentication) 
	  {
		EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.evseProcessing_AuthEIM = Finished; 
		sequence_step_1CH = 1035;
		timeout_timer_1CH = 0;
	  }
	}
	else if(timeout_timer_1CH > 6000) //LLC -> HLC timeout
	{
	  Input_Memory[1].ERROR1.bit.Slac_Req_Error = 1; //SLAC 응답 없음으로 처리
	  sequence_step_1CH = Sequence_Stop;
	}
	else if(Combo_stop_button_1CH == 1) // 사용자에 의한 정지
	{
	  sequence_step_1CH = Sequence_Stop; 
	  stop_code_1CH = 2;
	  user_stop = 1;
	}
	break;
	
  case 1035: // evseProcessing_CPD
	if(SECC_Msg[1].SECC_Status_1.seccStatusCode == ChargeParameterDiscovery && (SECC_Msg[1].SECC_EV_Service_Selection.selectedEnergyTransferMode_3 ==  DC_extended))
	{
	  EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.evseProcessing_CPD = Finished;
	  sequence_step_1CH = 1040;
	  timeout_timer_1CH = 0;
	}
	else if(timeout_timer_1CH > 6000) 
	{
	  Input_Memory[1].ERROR1.bit.Discovery_Req_Error = 1; //디스커버리 응답없음 에러 , // 중국버스들... 대일버스 현장에서 주로 나오는 에러
	  sequence_step_1CH = Sequence_Stop;
	}
	else if(Combo_stop_button_1CH == 1) // 사용자에 의한 정지
	{
	  sequence_step_1CH = Sequence_Stop; 
	  stop_code_1CH = 2;
	  user_stop = 1;
	}
	
	break;
	
  case 1040:
	if(SECC_Msg[1].SECC_Status_1.seccStatusCode == CableCheck) // CableCheck 40s 이내 Finished 응답
	{
	  Channel1_RUN = 1; // for led indicator
	  Digital_output.O_RY1 = 1; // 2020.11.20 DC+
	  Digital_output.O_RY2 = 1; // 2020.11.20 DC-
	  EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.isolationMonitoringActive = Active;
	  
	  module_rectification = 1;
	  module_on_off_G1 = MODULE_ON;
	  Vout_ref_temp_G1 = ISOLATION_SET_VOLTAGE;
	  Iout_ref_temp_G1 = ISOLATION_SET_CURRENT;
	  
	  sequence_step_1CH = 1055;
	  timeout_timer_1CH = 0;
	}
	else if(timeout_timer_1CH > 6000)
	{
	  Input_Memory[1].ERROR1.bit.CableCheck_Req_Error = 1; //케이블체크 응답없음 에러
	  sequence_step_1CH = Sequence_Stop;
	}
	else if(Combo_stop_button_1CH == 1) // 사용자에 의한 정지
	{
	  sequence_step_1CH = Sequence_Stop; 
	  stop_code_1CH = 2;
	  user_stop = 1;
	}
	break;
	
  case 1055:
	if((Vout_Module_G1 >= (ISOLATION_SET_VOLTAGE * 0.9F)) || (Vout_G1 >= (ISOLATION_SET_VOLTAGE * 0.9F)))
	{
	  Vout_ref_temp_G1 = DEFAULT_SET_VOLTAGE; // 2022.09.27 다시 살림. 동양 범한버스는 precharge 339V 요청함.
	  Iout_ref_temp_G1 = DEFAULT_SET_CURRENT;
	  module_on_off_G1 = MODULE_OFF;
	  module_rectification = 0;
	  module_set_data.set_single_pm_ac_basic_information.working_mode = invalid;
	  sequence_step_1CH = 1065;
	  timeout_timer_1CH = 0;            
	}
	else if(timeout_timer_1CH > 1500)
	{
	  Input_Memory[1].ERROR1.bit.CableCheck1_Error = 1; //케이블체크 1 에러, 전압 출력 실패 ->
	  sequence_step_1CH = Sequence_Stop;
	}
	else if(Combo_stop_button_1CH == 1) // 사용자에 의한 정지
	{
	  sequence_step_1CH = Sequence_Stop; 
	  stop_code_1CH = 2;
	  user_stop = 1;
	}
	break;
	
  case 1065: //20260403 박형은 - IMD가 절연을 정상적으로 측정하기위해서는 최소 6초가 필요한데, 전압상승 후 대기시간이 없어보임. 10초 정도의 전압 유지 고려. 1040 -> 10xx 대기추가 -> 1065
	if((Vout_Module_G1 <= 50) || (Vout_G1 <=50)) // Vout_G1 -> Vout_Module, 2022.10.05 중국버스에서 릴레이를 먼저 붙였을 때 고려 (윈라인 모듈만 해당, 전압센싱을 출력 다이오드 1차측에서 함)
	{
	  module_rectification = 1; //20260403 박형은 - 60kw 모듈 3개 제어시 변경해야함 TODO
	  module_on_off_G1 = MODULE_ON;
	  Vout_ref_temp_G1 = DEFAULT_SET_VOLTAGE;
	  Iout_ref_temp_G1 = DEFAULT_SET_CURRENT;
	  
	  EVSE_Msg[1].EVSE_Status.evseIsolationStatus_4 = Valid; // 절연테스트 정상
	  EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.evseProcessing_CableCheck = Finished;
	  sequence_step_1CH = 1070;
	  timeout_timer_1CH = 0;
	  
	}
	else if(timeout_timer_1CH > 2000)
	{
	  Input_Memory[1].ERROR1.bit.CableCheck2_Error = 1; //케이블체크 2 에러, 방전 실패
	  sequence_step_1CH = Sequence_Stop;
	}
	else if(Combo_stop_button_1CH == 1) // 사용자에 의한 정지
	{
	  sequence_step_1CH = Sequence_Stop; 
	  stop_code_1CH = 2;
	  user_stop = 1;
	}
	break;
	
  case 1070:
	if(SECC_Msg[1].SECC_Status_1.seccStatusCode == PreCharge) // +- 5V 이내, 7초 이내
	{
	  Vout_ref_temp_G1 = SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetVoltage_01 * 0.1;
	  Iout_ref_temp_G1 = DEFAULT_SET_CURRENT;
	  dc_electric_energy += Vout_G1 * Iout_Module_G1 * 2.77777778e-9; //  10ms마다 Sample/0.01s * 1hour(s), DC 전력량 [kW/h]
	  timeout_timer_1CH = 0;
	}
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Charging)
	{
	  dc_electric_energy += Vout_G1 * Iout_Module_G1 * 2.77777778e-9; //  10ms마다 Sample/0.01s * 1hour(s), DC 전력량 [kW/h]
	  sequence_step_1CH = Sequence_Charging;
	  timeout_timer_1CH = 0;
	}
	else if(timeout_timer_1CH > 2000)
	{
	  Input_Memory[1].ERROR1.bit.PreCharge_Req_Error = 1; //프리차지 응답없음 에러
	  sequence_step_1CH = Sequence_Stop;
	}
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Terminate)
	{
	  sequence_step_1CH = Sequence_Normal_Stop;
	}
	
	break;
	
  case Sequence_Charging : // 1075, //PowerDelivery, //CurrentDemand, 효율은 93%로 계산하여 입력이 계약전력을 넘지 않도록 할 것.
	
	dc_electric_energy += Vout_G1 * Iout_Module_G1 * 2.77777778e-9; // 10ms마다 Sample/0.01s * 1hour(s), DC 전력량 [kW/h]
	cp_fault_status_copy_flag = 1;
	
	SOC_1CH = SECC_Msg[1].SECC_EV_SoC_Related_Parameters.evSOC_1; // 범한버스는 처음에 SOC_1CH 데이터 10을 보내옴.
	if(SOC_old == 0 && SOC_1CH != 0) start_SOC = SOC_1CH;
	SOC_old = SOC_1CH;	
	
	if(Iout_Module_G1 <= 2 && ((abs((UINT16)(Vout_ref_temp_G1 - Vout_Module_G1))) <= 5)) ev_ry_fault_timer++; // bus relay off 판단
	else                                                                                 ev_ry_fault_timer = 0;
	
	if(EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumCurrentLimit_01 <= EVSE_Msg[1].EVSE_Present_Voltage_Current.evsePresentCurrent)   EVSE_Msg[1].EVSE_Status.LimitAchieved_7.bit.evseCurrentLimitAchieved = TRUE;
	else                                                                                                                                EVSE_Msg[1].EVSE_Status.LimitAchieved_7.bit.evseCurrentLimitAchieved = FALSE;
	if(EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumVoltageLimit_45 <= EVSE_Msg[1].EVSE_Present_Voltage_Current.evsePresentVoltage)   EVSE_Msg[1].EVSE_Status.LimitAchieved_7.bit.evseVoltageLimitAchieved = TRUE;
	else                                                                                                                                EVSE_Msg[1].EVSE_Status.LimitAchieved_7.bit.evseVoltageLimitAchieved = FALSE;
	
	Vout_ref_temp_G1 = (float)SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetVoltage_01 * 0.1F;
	
	if(SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetCurrent_23 * 0.1F > EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumCurrentLimit_01 * 0.1F) Iout_ref_temp_G1 = (float)EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumCurrentLimit_01 * 0.1F;
	else 	                                                                                                                                  Iout_ref_temp_G1 = (float)SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetCurrent_23 * 0.1F;
	
	if(SECC_Msg[1].SECC_EV_SoC_Related_Parameters._Complete_Conditioning_0.bit.chargingComplete == Complete) {sequence_step_1CH = Sequence_Normal_Stop; stop_code_1CH = 5;}
	if(SECC_Msg[1].SECC_Status_1.seccStatusCode != Charging)                                                 {sequence_step_1CH = Sequence_Normal_Stop; stop_code_1CH = 5;}
	if(Combo_stop_button_1CH == 1)                                                                           {sequence_step_1CH = Sequence_Stop;        stop_code_1CH = 2; user_stop = 1;}
	break;
	
	
  case Sequence_Stop: // EVSE STOP, USER STOP
	EVSE_Msg[1].EVSE_Status.chargingControl_1 = EVSE_initiated_normal_stop;
	Vout_ref_temp_G1 = DEFAULT_SET_VOLTAGE;
	Iout_ref_temp_G1 = 5;
	
	sequence_step_1CH = 1800;
	timeout_timer_1CH = 0;
	
	break;
	
  case Sequence_Normal_Stop:
	Vout_ref_temp_G1 = DEFAULT_SET_VOLTAGE;
	Iout_ref_temp_G1 = 5;
	
	sequence_step_1CH = 1800;
	timeout_timer_1CH = 0;
	break;
	
	
  case Sequence_Emergency_Stop:
	EVSE_Msg[1].EVSE_Status.chargingControl_1 = EVSE_initiated_emergency_stop;
	Vout_ref_temp_G1 = DEFAULT_SET_VOLTAGE;
	Iout_ref_temp_G1 = 5;
	
	sequence_step_1CH = 1800;
	timeout_timer_1CH = 0;
	
	break;
  case 1800:
	end_SOC = SOC_1CH;
	
	if(timeout_timer_1CH > 500)
	{
	  sequence_step_1CH = 1805;
	  timeout_timer_1CH = 0;
	  
	  module_on_off_G1 = MODULE_OFF;
	  module_rectification = 0;
	  module_set_data.set_single_pm_ac_basic_information.working_mode = invalid;
	  Digital_output.O_RY1 = 0;
	  Digital_output.O_RY2 = 0;
	  eSysState_1CH = STATE_FINISH; // green led on
	}
	break;
	
  case 1805:
	if((Vout_Module_G1 <= 50) ||(Vout_G1 <= 50))
	{
	  EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.triggerStateF = Enabled; //  -12V  
	  
	  if(timer_step3_1CH++ > 1000) // BYD버스에서 BMS 초기화 하는 시간 확보
	  {
		timer_step3_1CH = 0;
		EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.triggerStateF = Disabled; // 2022.07.28
		
		if(user_stop == 0)
		{
		  if(restart_flag_1CH == 1) // 사용자 정지 시 cp error 로 종료되는 경우가 있음.
		  {
			restart_flag_1CH = 0;
			if(charging_restart_counter++ < 4)
			{
			  timeout_timer_1CH = 0;
			  sequence_step_1CH = 1806;
			  stop_code_1CH = 0;
			  
			  Init_RxMes(&RxMessage2, CAN_ID_EXT); // 필요한지?
			  Init_TxMes(&TxMessage2, CAN_ID_EXT); // 필요한지?
			  
			  Variable_Initialize_1CH();
			  EVSE_Msg[1].EVSE_Status.chargingControl_1 = Initialize_parameters;
			}
			else
			{
			  timeout_timer_1CH = 0;
			  sequence_step_1CH = 1810;
			}
		  }
		  else
		  {
			timeout_timer_1CH = 0;
			sequence_step_1CH = 1810;
		  }
		}
		else
		{
		  timeout_timer_1CH = 0;
		  sequence_step_1CH = 1810;
		}
	  }
	  
	  //            timeout_timer_1CH = 0;
	}
	else if(timeout_timer_1CH > 2500)
	{
	  sequence_step_1CH = 1810;
	  timeout_timer_1CH = 0;
	}
	break;
	
  case 1806 :
	if(timeout_timer_1CH > 1000) // PLC 초기화 시간 확보
	{
	  sequence_step_1CH = 999;
	  timeout_timer_1CH = 0;
	}
	break;
	
  case 1810:
	if(timeout_timer_1CH > 300)
	{
	  sequence_step_1CH = 1815;
	  timeout_timer_1CH = 0;
	  my_stand_standby_1CH = 1; // 대기중, 나머지 스탠드 충전 가능 2022.02.09
	}
	break;
	
  case 1815:
	Channel1_RUN = 0; // led run clear, 2022.08.01
	Input_Memory[1].Charger_Status.bit.Charging_Run = 0; //No Charging RUN
	Input_Memory[1].Charger_Status.bit.Plug_Check = 0;
	
	sequence_step_1CH = 1820;
	break;
	
  case 1820:
	Input_Memory[1].Charger_Status.bit.Charging_Finish = 1; //Charging Finish
	
	if(timer_step3_1CH++ > 500) //20240327 3 -> 5초로 변경
	{
	  timer_step3_1CH = 0;
	  sequence_step_1CH = 0;
	  finish_newline_flag = 1;
	}
	break;
	
  }  // END of switch
  
} // End of Sequence_control_1CH()


#ifdef  ONLY_ACPM_READ                  //DC 계량값은 UI로부터 받아온 값 사용

void V2G_inverter_mode_test(void) //240813
{
  static int8_t v2g_inverter_mode_test_step;
  static int16_t v2g_inverter_mode_test_cnt;
  if(Holding_Memory[1].Cable_Type == AC3) 
  {
	//20260611 PHE 
	ufRefV_G1.ulValue  = (uint32_t)Holding_Memory[1].Test_Mode_Vout_Ref_high;
	ufRefV_G1.ulValue |= (uint32_t)Holding_Memory[1].Test_Mode_Vout_Ref_low << 16;
	if(ufRefV_G1.fValue < DEFAULT_SET_VOLTAGE)
	{
	  Vout_ref_temp_G1 = DEFAULT_SET_VOLTAGE;
	}else if(ufRefV_G1.fValue > MAX_OUTPUT_VOLTAGE){
	  Vout_ref_temp_G1 = MAX_OUTPUT_VOLTAGE;
	}else {
	  Vout_ref_temp_G1 = ufRefV_G1.fValue;
	}
	
	if(Combo_stop_button_1CH == 1)
	{
	  v2g_inverter_mode_test_step = 4;
	  Input_Memory[1].out_voltage = (uint16_t)Vout_Module_G1;
	  Input_Memory[1].out_current = (uint16_t)Iout_Module_G1;
	}
	
	switch(v2g_inverter_mode_test_step){
	case 0:
	  if(Combo_start_button_1CH == 1) 
	  {
		Digital_output.O_MC1 = 1;
		module_grid_connected_inverter = 1;   
		v2g_inverter_mode_test_step = 1;
	  }
	  break;
	  
	case 1:
	  if(Digital_input.bit.I_MC1 != 1) 
	  {
		//error log - MC1 is not working
		//error count, retry max 2.
		v2g_inverter_mode_test_step = 0;
	  }else if(v2g_inverter_mode_test_cnt++ > 300) // 3s
	  {
		Module_Dial_Switch();
		Digital_output.O_RY1 = 1;
		Digital_output.O_RY2 = 1;
		
		v2g_inverter_mode_test_cnt = 0;
		v2g_inverter_mode_test_step = 2;
	  }
	  break;
	  
	case 2 :
	  if((Vout_Module_G1 >= 150) && (sim200_data_4hz.v_batt > 150)) //20260526 임시로 현재 SIM 데이터 수신하는지 확인이 안돼서 삭제
	  {
		v2g_inverter_mode_test_step = 3;
		if(module_set_data.set_single_pm_ac_basic_information.working_mode == grid_connected_inverter_mode){
		  module_on_off_G1 = MODULE_ON;
		  Channel1_RUN = 1;
		}
		else if(module_set_data.set_single_pm_ac_basic_information.working_mode != grid_connected_inverter_mode){
		  module_on_off_G1 = MODULE_OFF;
		  Channel1_RUN = 0;
		  eSysState_1CH = STATE_STANDBY;
		}
	  }
	  
	  break;
	  
	case 3:
	  if((Vout_Module_G1 < 150) && (sim200_data_4hz.v_batt < 150)) //20260526 임시로 현재 SIM 데이터 수신하는지 확인이 안돼서 삭제
	  {
		v2g_inverter_mode_test_step = 4;
	  }
	  
	  break;
	  
	case 4:
	  module_grid_connected_inverter = 0;
	  module_set_data.set_single_pm_ac_basic_information.working_mode = invalid;
	  module_on_off_G1 = MODULE_OFF;
	  Channel1_RUN = 0;
	  eSysState_1CH = STATE_STANDBY;
	  
	  Digital_output.O_RY1 = 0;
	  Digital_output.O_RY2 = 0;
	  
	  if(v2g_inverter_mode_test_cnt++ > 50) // 500ms
	  {
		v2g_inverter_mode_test_cnt = 0;
		v2g_inverter_mode_test_step = 5;
	  }
	  break;
	  
	case 5:
	  v2g_inverter_mode_test_step = 0;
	  Digital_output.O_MC1 = 0;
	  Digital_output.O_RY1 = 0;
	  Digital_output.O_RY2 = 0;
	  Channel1_RUN = 0;
	  eSysState_1CH = STATE_STANDBY;
	  break;
	}
  }
}

#elif   defined ACPM_DCPM_READ                  //DC 계량값은 계량기로부터 받아온 값 직접 사용

void V2G_inverter_mode_test(void) //240813
{
  if(Combo_stop_button_1CH == 0){
	
	Digital_output.O_RY1 = 1;
	Digital_output.O_RY2 = 1;
	Input_Memory[1].out_voltage = (uint16_t) Acrel_Rx_ch1_data.DC_voltage_value;
	Input_Memory[1].out_current = (uint16_t) (Acrel_Rx_ch1_data.DC_current_value/10);
	
	if((Acrel_Rx_ch1_data.DC_voltage_value >= 150) && (SIM_info.SIM_Battery_Voltage.Vb >= 150)){
	  
	  module01_grid_connected_inverter = 1;      
	  
	  if(module_set_data.set_single_pm_ac_basic_information.working_mode == grid_connected_inverter_mode){
		module_on_off_G1 = MODULE_ON;
		Channel1_RUN = 1;
	  }
	  else if(module_set_data.set_single_pm_ac_basic_information.working_mode != grid_connected_inverter_mode){
		module_on_off_G1 = MODULE_OFF;
		Channel1_RUN = 0;
		eSysState_1CH = STATE_STANDBY;
	  }
	}
	else if((Acrel_Rx_ch1_data.DC_voltage_value < 150) && (SIM_info.SIM_Battery_Voltage.Vb < 150))
	{
	  module01_grid_connected_inverter = 0;
	  module_on_off_G1 = MODULE_OFF;
	  Channel1_RUN = 0;
	  eSysState_1CH = STATE_STANDBY;
	}
  }
  else if(Combo_stop_button_1CH == 1){
	
	module01_grid_connected_inverter = 0;
	module_set_data.set_single_pm_ac_basic_information.working_mode = invalid;
	module_on_off_G1 = MODULE_OFF;
	Channel1_RUN = 0;
	eSysState_1CH = STATE_STANDBY;
	
	Digital_output.O_RY1 = 0;
	Digital_output.O_RY2 = 0;
	
	Input_Memory[1].out_voltage = (uint16_t) SIM_info.SIM_Battery_Voltage.Vb;
	Input_Memory[1].out_current = (uint16_t) Acrel_Rx_ch1_data.DC_current_value;
  }
}
#endif

void Combo_Load_Test_1CH(void) // 2020.12.16 인증용 프로그램을 위해 기능 추가함.
{
  static int8_t test_step;
  static int16_t test_step_cnt; 
  static uint8_t ry1_error;
  static uint8_t ry2_error;
  
  if(Holding_Memory[1].Cable_Type == COMBO_Test) // 그룹1 모듈 제어
  {
	if(Holding_Memory[1].Power_Control_Value == 4) //1채널 싱글
	{
	  charge_mode_capacity = _100kW_mode;
	  switch(test_step)
	  {
	  case 0 :
		if(Combo_start_button_1CH == 1) 
		{
		  Digital_output.O_MC1 = 1;
		  test_step = 1;
		}
		break;
		
	  case 1:
		if(Digital_input.bit.I_MC1 != 1) 
		{
		  //error log - MC1 is not working
		  //error count, retry max 2.
		  test_step = 0;
		}else if(test_step_cnt++ > 300)
		{
		  Module_Dial_Switch();
		  test_step_cnt = 0;
		  test_step = 2;
		}
		break;
		
	  case 2 :
		module_rectification = 1;
		module_on_off_G1 = MODULE_ON;
		module_on_off_G2 = MODULE_ON;
		Digital_output.O_RY1 = 1;
		Digital_output.O_RY2 = 1;
		Channel1_RUN = 1;
		Vout_ref_temp_G1 = DEFAULT_SET_VOLTAGE;
		Iout_ref_temp_G1 = DEFAULT_SET_CURRENT;
		Vout_ref_temp_G2 = DEFAULT_SET_VOLTAGE;
		Iout_ref_temp_G2 = DEFAULT_SET_CURRENT;
		test_step = 3;
		break;
		
	  case 3:
		if(test_step_cnt++ > 30) //300ms wait for ry1,ry2 closing
		{
		  test_step_cnt = 0;
		  test_step = 4;
		}
		break;
		
	  case 4:
		if(Digital_input.bit.I_RY1 != 1) 
		{
		  //error log - RY1 is not working
		  //error count, retry max 2.
		  ry1_error = 1;
		}
		if(Digital_input.bit.I_RY2 != 1) 
		{
		  //error log - RY2 is not working
		  //error count, retry max 2.
		  ry2_error = 1;
		}
		if(ry1_error||ry2_error){
		  test_step = 2;
		}else
		{
		  test_step = 5;
		}
		
		break;
		
	  case 5:
		ufRefV_G1.ulValue  = (uint32_t)Holding_Memory[1].Test_Mode_Vout_Ref_high;
		ufRefV_G1.ulValue |= (uint32_t)Holding_Memory[1].Test_Mode_Vout_Ref_low << 16;
		ufRefI_G1.ulValue  = (uint32_t)Holding_Memory[1].Test_Mode_Iout_Ref_high;
		ufRefI_G1.ulValue |= (uint32_t)Holding_Memory[1].Test_Mode_Iout_Ref_low << 16;
		
		if(ufRefV_G1.fValue < DEFAULT_SET_VOLTAGE)
		{
		  Vout_ref_temp_G1 = DEFAULT_SET_VOLTAGE;
		}else if(ufRefV_G1.fValue > MAX_OUTPUT_VOLTAGE){
		  Vout_ref_temp_G1 = MAX_OUTPUT_VOLTAGE;
		}else {
		  Vout_ref_temp_G1 = ufRefV_G1.fValue;
		}
		
		if(ufRefI_G1.fValue < DEFAULT_SET_CURRENT)
		{
		  Iout_ref_temp_G1 = DEFAULT_SET_CURRENT;
		}else if(ufRefI_G1.fValue > MAX_OUTPUT_CURRENT){
		  Iout_ref_temp_G1 = MAX_OUTPUT_CURRENT;
		}else{
		  Iout_ref_temp_G1 = ufRefI_G1.fValue;
		}
		
		ufFbVout_G1.fValue = Vout_Module_G1; //Vout_Module;
		ufFbIout_G1.fValue = Iout_Module_G1;
		
		Input_Memory[1].out_voltage = (uint16_t) (Vout_Module_G1); // 출력 전압
		Input_Memory[1].out_current = (uint16_t) (Iout_Module_G1); // 출력 전류
		
		if(Combo_stop_button_1CH == 1)
		{
		  test_step = 6;
		}
		break;
		
	  case 6:
		Vout_ref_temp_G1 = DEFAULT_SET_VOLTAGE;
		Iout_ref_temp_G1 = DEFAULT_SET_CURRENT;
		if(test_step_cnt++ > 10)
		{
		  module_on_off_G1 = MODULE_OFF;
		  module_on_off_G2 = MODULE_OFF;
		  test_step_cnt = 0;
		  test_step = 7;
		}
		break;
		
	  case 7:
		if(u16_Iout_Module_G1 < 5) test_step = 8;
		break;
		
	  case 8:
		test_step = 0;
		
		Digital_output.O_RY1 = 0;
		Digital_output.O_RY2 = 0;
		Channel1_RUN = 0;
		eSysState_1CH = STATE_STANDBY;
		
		module_rectification = 0;
		module_set_data.set_single_pm_ac_basic_information.working_mode = invalid;
		
		Digital_output.O_MC1 = 0;
		break;
	  }
	  
	  
	  
	}
  }
}


void PLC_Reboot_1CH(void) // 100ms polling
{
  plc_reset_timer_1CH++;
  
  switch(plc_reset_step_1CH)
  {
  case 0 : 
	if((plc_can_communication_rx_fault_1CH == _rx_fault) && (eSysState_1CH == STATE_STANDBY))
	{
	  EVSE_SECC_cnt_1CH_reboot = 1; // power off
	  plc_reset_timer_1CH = 0; 
	  plc_reset_step_1CH++;
	}
	break;
	
  case 1 :
	if(plc_can_communication_rx_fault_1CH == _rx_non_fault)
	{
	  plc_reset_timer_1CH = 0; 
	  plc_reset_step_1CH = 0;
	}
	break;
  }
}



void Power_Control_Value_1CH(void)
{
  if(mode_change == default_)
  {
	/* 
	-set_total_power_fix : kW 단위의 전력을 의미
	-POWER_EFFICIENCY : 0.93 (93%의 효율을 임의로 설정)
	-charger_out_maximum_current_1CH : 전압 값에 따라 변동되는 최대 출력 전류 값을 설정하기 위한 변수
	-CHARGER_OUTPUT_MAX_POWER : (set_total_power_fix * 1000)kW
	-Vout_G1 : 모듈 출력 전압
	
	코드설명 : 22kW인 모듈의 스펙상 최대 출력 전류는 73.3A -> 22000kW/73.3A = 300V
	1. 300V의 0.93효율 값보다 모듈의 출력 전압이 높다면, 최대 출력 가능 전류 = ((최대출력 * 효율) / 모듈 전압) = 해당 모듈 전압일 때 출력 가능한 최대 전류
	2. 300V의 0.93효율 값보다 모듈의 출력 전압이 낮다면, 최대 출력 가능 전류 = 스펙상 최대 출력 가능 전류 (여기서는 73.3, 약 73A)
	*/
	set_total_power_fix = 22;
	EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumPowerLimit_23 = 2200; //22kW //240624
	if     (Vout_G1 >= (300 * POWER_EFFICIENCY)) charger_out_maximum_current_1CH = (uint16_t)(CHARGER_OUTPUT_MAX_POWER * POWER_EFFICIENCY / Vout_G1); //22kW기준 -> (22000W/73.3A = 300V)
	else                                         charger_out_maximum_current_1CH = 73;
	
	final_set_max_current_1CH = charger_out_maximum_current_1CH;
	
	if     (coupler_temperature_P_1CH >= 80) EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumCurrentLimit_01 = final_set_max_current_1CH * 5;            //0.5배
	else if(coupler_temperature_P_1CH <  70) EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumCurrentLimit_01 = final_set_max_current_1CH * 10;           //1배
	
	if(sequence_step_1CH == 1075)
	{
	  
	  module_rectification = 1;
	  
	  module_on_off_G1 = MODULE_ON;
	  module_on_off_G2 = MODULE_ON;
	  Vout_ref_temp_G2 = Vout_ref_temp_G1;
	  Iout_ref_temp_G2 = Iout_ref_temp_G1;
	}
	else if(sequence_step_1CH == 1800)
	{
	  module_on_off_G1 = MODULE_OFF;
	  module_on_off_G2 = MODULE_OFF;
	  module_rectification = 0;
	  module_set_data.set_single_pm_ac_basic_information.working_mode = invalid;
	}
	
	
	else if(charge_mode_capacity == _50kW_mode)
	{
	  
	  set_total_power_fix = 22;
	  EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumPowerLimit_23 = 500; //50kW
	  if     (Vout_G1 >= (400 * POWER_EFFICIENCY)) charger_out_maximum_current_1CH = (uint16_t)(CHARGER_OUTPUT_MAX_POWER * POWER_EFFICIENCY / Vout_G1); //50kW기준
	  else                                         charger_out_maximum_current_1CH = 125;
	  
	  final_set_max_current_1CH = charger_out_maximum_current_1CH;
	  
	  if     (coupler_temperature_P_1CH >= 80) EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumCurrentLimit_01 = final_set_max_current_1CH * 5;
	  else if(coupler_temperature_P_1CH <  70) EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumCurrentLimit_01 = final_set_max_current_1CH * 10;
	}
  }
  else if((sequence_step_1CH == 1075) && (mode_change == _100kW_to_50kW || mode_change == _50kW_to_100kW))
  {
	EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumCurrentLimit_01 = 20;
	Vout_ref_temp_G1 = Vout_G1; //
	Iout_ref_temp_G1 = 4;    //
	
	if(Iout_G1 <= 7 || mode_change_timer++ >= 500)
	{
	  mode_change = default_;
	  mode_change_timer = 0;
	}
	
  }
}
