
#define __FAULT_CHECK_C


#include "fault_check.h"
#include "stdint.h"
#include "stm32f10x.h"
#include "math.h"
#include "main.h"
#include "HMI_modbus.h"
#include "digital_io.h"
#include "PLC_3_0.h"
#include "Terminal.h"
#include "pt1000.h"
#include "INFY_V2G.h"
#include "SIM_100.h"
#include "UART.h"


void Fault_Check_1CH(void)
{
  if(Digital_input.bit.I_EMG == 0)                                                              Input_Memory[1].Fault_List_1.bit.Emergency_switch_on = 1;  //비상정지 버튼 눌림
  if(RY_open_check   (Digital_input.bit.I_RY1, Digital_output.O_RY1, &cnt_ry1_o))               Input_Memory[1].Fault_List_1.bit.RY1_Error = 1;
  if(RY_open_check   (Digital_input.bit.I_RY2, Digital_output.O_RY2, &cnt_ry2_o))               Input_Memory[1].Fault_List_1.bit.RY2_Error = 1;
  if(RY_welding_check(Digital_input.bit.I_RY1, Digital_output.O_RY1, &cnt_ry1_w))               Input_Memory[1].Fault_List_1.bit.RY1_Error = 1;
  if(RY_welding_check(Digital_input.bit.I_RY2, Digital_output.O_RY2, &cnt_ry2_w))               Input_Memory[1].Fault_List_1.bit.RY2_Error = 1;
  if(module1_can_communication_rx_fault == _rx_fault)                                           Input_Memory[1].Fault_List_1.bit.Module_Communication = 1;
  if(Digital_input.bit.I_FUSE_8 == 0)                                                           Input_Memory[1].Fault_List_1.bit.Fuse_Open = 1;
  if(Digital_input.bit.I_FUSE_9 == 0)                                                           Input_Memory[1].Fault_List_1.bit.Fuse_Open = 1; //20260403 박형은 : FUSE +.- 구별 불가. 프로토콜에 N Fuse 비트 없음.
  if(coupler_temperature_P_1CH > COUPLER_OVERTEMPRATURE_SET_VALUE)                              Input_Memory[1].Fault1.bit.Coupler_Temp_fault = 1; //(콤보에러)커넥터 과온도 오류
  if((sequence_step_1CH >=1040) && (Vout_G1 >= OVER_VOLTAGE_SET_VALUE))                         Input_Memory[1].Fault1.bit.Over_Voltage = 1;//CCS OverVoltage // 과전압 최대정격 900V의 110%에서 검출 1000V까지만 센싱가능                                                 Input_Memory[1].Fault1.bit.Over_Current = 1; // 최대정격 200A의 110%에서 검출 30kW(40A) * 5 -> MAX 200A까지 출력가능 , 정전력 모듈은 더 높게 출력가능
  if((DCIout_1CH >= OVER_CURRENT_SET_VALUE) && (Iout_Module_G1 >= OVER_CURRENT_SET_VALUE))   	Input_Memory[1].Fault1.bit.Over_Current = 1;       //22kW INFY V2G 모듈 기준, 최대 전류 73.3A의 110%인 약 80A 기준
  if(Digital_input.bit.I_SPD == 0)                                               	       	    Input_Memory[1].Fault_List_2.bit.spd_error = 1; //20260407 박형은 추가 : SPD 접점 에러
	
  
  if(SIM_info.SIM_Isolation_State.Status_Bits.IS1_IS0 == 0b11)
  {
	if(Leakage_time++ > 500)
	{
	  Leakage_time = 0;
	  if(sequence_step_1CH < 1070)                                                              Input_Memory[1].Fault1.bit.EVSE_Current_Leakage = 1;
	  if(sequence_step_1CH >= 1070)                                                             Input_Memory[1].Fault1.bit.EV_Current_Leakage = 1;
	}
  }
  else Leakage_time = 0;
  
  /**************************UI에 에러 코드 없이 에러 발생 시 아래의 조건문에 의한 에러 확인**************************/
  if(charging_type_select_1CH != load_test)
  {
    if(Input_Memory[1].Fault_List_1.all || Input_Memory[1].Fault1.all || Input_Memory[1].Fault2.all || \
      Input_Memory[1].Fault3.all     || Input_Memory[1].ERROR1.all || Input_Memory[1].ERROR2.all || hmi_485_communication_rx_fault == _rx_fault || \
        imd_can_communication_rx_fault == _rx_fault)
    {
      Combo_stop_button_1CH = 1;
      Input_Memory[1].Charger_Status.bit.Charger_Fault = 1;      // fault sum
      eFaultState_1CH = STATE_FAULT;
    }
  }
  
  /*****************************************************************************************************/
  
  if(eFaultState_1CH == STATE_FAULT)
  {
	if (fault_timer_1CH++ > 30000) // 30초 후 Fault Clear
	{
	  fault_timer_1CH = 0;
	  
	  if(hmi_485_communication_rx_fault == _rx_non_fault){           //HMI 485 통신 에러 클리어
		hmi_485_communication_rx_fault_count = 0;
	  }
	  
	  if(imd_can_communication_rx_fault  == _rx_non_fault){          //IMD CAN 통신 에러 클리어
		imd_can_communication_rx_fault_count = 0;
	  }
	  
	  if(plc_can_communication_rx_fault_1CH == _rx_non_fault){       //PLC CAN 통신 에러 클리어
		plc_can_communication_rx_fault_count_1CH = 0;
	  }
	  
	  if(ac_meter_communication_rx_fault == _rx_non_fault){          //AC METER 485통신 에러 클리어
		for(int i = 1; i <= 5; i++)
		{
		  ac_meter_communication_rx_fault_count[i] = 0;
		}
	  }
	  
	  if(dc_meter_communication_rx_fault == _rx_non_fault){          //DC METER 485통신 에러 클리어
		dc_meter_communication_rx_fault_count = 0;
	  }
	  
	  if(module1_can_communication_rx_fault == _rx_non_fault){       //모듈 CAN 통신 에러 클리어
		power_module_Communition_error_counter = 0;
	  }
	  
	  rxok_DM = 1;
	  
	  
	  Input_Memory[1].Charger_Status.bit.Charger_Fault                                  = 0; 
	  Input_Memory[1].Charger_Status.bit.Charging_Finish                                = 0;    // 충전 종료 알림
	  Input_Memory[1].Fault_List_1.all                                                  = 0;
	  Input_Memory[1].Fault_List_2.all                                                  = 0;
	  Input_Memory[1].Fault1.all							= 0;  
	  Input_Memory[1].Fault2.all							= 0;     
	  Input_Memory[1].Fault3.all							= 0;     
	  Input_Memory[1].ERROR1.all							= 0;     
	  Input_Memory[1].ERROR2.all							= 0;  
	  
	  eFaultState_1CH = STATE_NONFAULT;
	}
  }
}




void communication_fault_check(uint16_t set_time) //10 seconds time setting //HMI, IMD, PLC, AC_METER, DC_METER, POWER_MODULE
{
  if(hmi_485_communication_rx_fault_count++ >= set_time)                {hmi_485_communication_rx_fault_count = set_time;               hmi_485_communication_rx_fault  = _rx_fault;            } // 485 wire or pc power down check or check run the app
  else                                                                  {                                                               hmi_485_communication_rx_fault  = _rx_non_fault;        }
  
  if(imd_can_communication_rx_fault_count++ >= set_time)                {imd_can_communication_rx_fault_count = set_time;               imd_can_communication_rx_fault  = _rx_fault;      }
  else                                                                  {                                                               imd_can_communication_rx_fault  = _rx_non_fault;  }
  
  if(plc_can_communication_rx_fault_count_1CH++ >= set_time)            {plc_can_communication_rx_fault_count_1CH = set_time;           plc_can_communication_rx_fault_1CH = _rx_fault;         }
  else                                                                  {                                                               plc_can_communication_rx_fault_1CH = _rx_non_fault;     }
  
  if(ac_meter_communication_rx_fault_count[1]++ >= set_time)            {ac_meter_communication_rx_fault_count[1] = set_time;           ac_meter_communication_rx_fault = _rx_fault;    }
  else                                                                  {                                                               ac_meter_communication_rx_fault = _rx_non_fault;}
  
  if(ac_meter_communication_rx_fault_count[2]++ >= set_time)            {ac_meter_communication_rx_fault_count[2] = set_time;           ac_meter_communication_rx_fault = _rx_fault;    }
  else                                                                  {                                                               ac_meter_communication_rx_fault = _rx_non_fault;}
  
  if(ac_meter_communication_rx_fault_count[3]++ >= set_time)            {ac_meter_communication_rx_fault_count[3] = set_time;           ac_meter_communication_rx_fault = _rx_fault;    }
  else                                                                  {                                                               ac_meter_communication_rx_fault = _rx_non_fault;}
  
  if(ac_meter_communication_rx_fault_count[4]++ >= set_time)            {ac_meter_communication_rx_fault_count[4] = set_time;           ac_meter_communication_rx_fault = _rx_fault;    }
  else                                                                  {                                                               ac_meter_communication_rx_fault = _rx_non_fault;}
  
  if(ac_meter_communication_rx_fault_count[5]++ >= set_time)            {ac_meter_communication_rx_fault_count[5] = set_time;           ac_meter_communication_rx_fault = _rx_fault;    }
  else                                                                  {                                                               ac_meter_communication_rx_fault = _rx_non_fault;}
  
  if(dc_meter_communication_rx_fault_count++ >= set_time)               {dc_meter_communication_rx_fault_count = set_time;              dc_meter_communication_rx_fault = _rx_fault;    }
  else                                                                  {                                                               dc_meter_communication_rx_fault = _rx_non_fault;}
  
  if(module_can_communication_start_G1)                                 //모듈 1개 사용이어서 폴트 체크, 1개 이상이면 정보만 넘겨야함. (모듈 여러 개 사용 시, 하나 망가져서 충전이 멈추면 안되므로)
  {
	if(power_module_Communition_error_counter++ >= set_time)            {power_module_Communition_error_counter = set_time;     module1_can_communication_rx_fault = _rx_fault;}
	else                                                                {                                                       module1_can_communication_rx_fault = _rx_non_fault;}
	
	//    if(module2_can_communication_rx_fault_count++ >= set_time)   {module2_can_communication_rx_fault_count = set_time; module2_can_communication_rx_fault = _rx_fault;}
	//    else                                                         {                                                     module2_can_communication_rx_fault = _rx_non_fault;}
	
	
	
	if(module1_can_communication_rx_fault != _rx_non_fault) //&& (module2_can_communication_rx_fault != _rx_non_fault))
	{
	  power_module_can_communication_fail_1CH = 1; // CAN communication fail, between stand and power module, bank-power off
	  
	  //      Input_Memory[1].Module_Warning.all = 0x0001;
	  Vout_Module_G1 = 0;
	}
	else power_module_can_communication_fail_1CH = 0; // 
  }
  
  //  if(module_can_communication_start_G2)
  //  {
  //    if(module3_can_communication_rx_fault_count++ >= set_time)   {module3_can_communication_rx_fault_count = set_time; module3_can_communication_rx_fault = _rx_fault;}
  //    else                                                         {                                                     module3_can_communication_rx_fault = _rx_non_fault;}
  //    
  //    if(module4_can_communication_rx_fault_count++ >= set_time)   {module4_can_communication_rx_fault_count = set_time; module4_can_communication_rx_fault = _rx_fault;}
  //    else                                                         {                                                     module4_can_communication_rx_fault = _rx_non_fault;}
  //    
  //    if((module3_can_communication_rx_fault != _rx_non_fault) && (module4_can_communication_rx_fault != _rx_non_fault))
  //    {
  //      power_module_can_communication_fail_2CH = 1; // CAN communication fail, between stand and power module, bank-power off
  //      Vout_Module_G2 = 0;
  //    }
  //    else power_module_can_communication_fail_2CH = 0; // 
  //  }
  
  else
  {
	module1_can_communication_rx_fault = _rx_non_fault;
	//    module2_can_communication_rx_fault = _rx_non_fault;
	//    module3_can_communication_rx_fault = _rx_non_fault;
	//    module4_can_communication_rx_fault = _rx_non_fault;
	
	
	//    if(module_can_communication_start_G1 == 0)
	//    {
	//      module_info[1].ufVout.fValue = 0;
	//      module_info[2].ufVout.fValue = 0;
	//      
	//      module_info[1].ufIout.fValue = 0;
	//      module_info[2].ufIout.fValue = 0;
	//    }
	//    if(module_can_communication_start_G2 == 0)
	//    {
	//      module_info[3].ufVout.fValue = 0;
	//      module_info[4].ufVout.fValue = 0;
	//      
	//      module_info[3].ufIout.fValue = 0;
	//      module_info[4].ufIout.fValue = 0;
	//    }
  }
}


uint16_t RY_open_check(uint32_t RY_in, uint32_t RY_out, uint16_t* chk_cnt)
{    
  (*chk_cnt)++;
  if (!RY_in)   
  {
	if (RY_out)
	{
	  if (*chk_cnt >= 1500)	
	  {
		*chk_cnt = 1500;
		return 1;
	  }
	}
	else    *chk_cnt = 0;
  }
  else	*chk_cnt = 0;  
  
  return 0;    
}

uint8_t RY_welding_check(uint32_t RY_in, uint32_t RY_out, uint16_t* chk_cnt)
{    
  (*chk_cnt)++;
  if (RY_in)   
  {
	if (!RY_out)
	{
	  if (*chk_cnt >= 1500)	
	  {
		*chk_cnt = 1500;
		return 1;
	  }
	}
	else    *chk_cnt = 0;
  }
  else	*chk_cnt = 0;  
  
  return 0;    
}

void cp_fault_save (void)
{
  cp_error_chager_status[fault_memory_cnt].occur_day       = g_day;
  cp_error_chager_status[fault_memory_cnt].occur_hour      = g_hour;
  cp_error_chager_status[fault_memory_cnt].occur_min       = g_minute;
  cp_error_chager_status[fault_memory_cnt].occur_sec       = g_second;
  
  cp_error_chager_status[fault_memory_cnt].cpVoltage       = SECC_Msg[1].SECC_Status_2.cpVoltage_12;                 // short
  cp_error_chager_status[fault_memory_cnt].cpOscillator    = SECC_Msg[1].SECC_Status_2.CP_Status_0.bit.cpOscillator; // UINT8
  cp_error_chager_status[fault_memory_cnt].started_soc     = start_SOC;                                           	// UINT8
  cp_error_chager_status[fault_memory_cnt].finished_soc    = SOC_1CH;                                                 // UINT8
  cp_error_chager_status[fault_memory_cnt].evccmac0        = SECC_Msg[1].SECC_EVCC_Info.evccMAC_0;                   // UINT8
  cp_error_chager_status[fault_memory_cnt].evccmac1        = SECC_Msg[1].SECC_EVCC_Info.evccMAC_1;                   // UINT8
  cp_error_chager_status[fault_memory_cnt].evccmac2        = SECC_Msg[1].SECC_EVCC_Info.evccMAC_2;                   // UINT8
  cp_error_chager_status[fault_memory_cnt].evccmac3        = SECC_Msg[1].SECC_EVCC_Info.evccMAC_3;                   // UINT8
  cp_error_chager_status[fault_memory_cnt].evccmac4        = SECC_Msg[1].SECC_EVCC_Info.evccMAC_4;                   // UINT8
  cp_error_chager_status[fault_memory_cnt].evccmac5        = SECC_Msg[1].SECC_EVCC_Info.evccMAC_5;                   // UINT8
  cp_error_chager_status[fault_memory_cnt].targetV         = SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetVoltage_01;     // UINT16
  cp_error_chager_status[fault_memory_cnt].demendI         = SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetCurrent_23;     // UINT16
  cp_error_chager_status[fault_memory_cnt].present_V       = EVSE_Msg[1].EVSE_Present_Voltage_Current.evsePresentVoltage;    // UINT16
  cp_error_chager_status[fault_memory_cnt].present_I       = EVSE_Msg[1].EVSE_Present_Voltage_Current.evsePresentCurrent;    // UINT16
  cp_error_chager_status[fault_memory_cnt].dc_electric_energy_cpy = dc_electric_energy;
  fault_memory_cnt++;	
}