
#define __HMI_MODBUS_C

#include "stdint.h"
#include "stm32f10x.h"
#include "HMI_modbus.h"
#include "main.h"
#include "stm32f10x_it.h"
#include "FirmwareUpdate.h"

#include "INFY_V2G.h"
#include "PLC_3_0.h"

#include "pt1000.h"
#include "digital_io.h"
#include "fault_check.h"
#include "Terminal.h"

void HMI_rx_data_parse(void) // Write (HMI -> Control Board)
{
  if(BOARD_ID_RX == 1)
  {
    if((Holding_Memory[BOARD_ID_RX].UI_Status.bit.Standby == 1) && (Holding_Memory[BOARD_ID_RX].UI_Status.bit.Start == 1)) 
      
    {
      Combo_stop_button_1CH = 0;
      Combo_start_button_1CH = 1;
      Holding_Memory[BOARD_ID_RX].UI_Status.bit.Start = 0;
    }
    else Combo_start_button_1CH = 0;
    
    if(Holding_Memory[BOARD_ID_RX].UI_Status.bit.Stop == 1) 
    {
      Combo_start_button_1CH = 0;
      Combo_stop_button_1CH = 1;
      EVSE_Msg[BOARD_ID_RX].EVSE_Status.chargingControl_1 = EVSE_initiated_normal_stop;
      stop_code_1CH = 2;
      Holding_Memory[BOARD_ID_RX].UI_Status.bit.Stop = 0;
      
      if(sequence_step_1CH == 0) Input_Memory[BOARD_ID_RX].Charger_Status.bit.Charging_Finish = 1;
    }
    
    if((Holding_Memory[BOARD_ID_RX].UI_Status.bit.FW_Update_Flag == 1) && (Holding_Memory[BOARD_ID_RX].UI_Status.bit.Ready == 1)) FW_update();
    if((Holding_Memory[BOARD_ID_RX].UI_Status.bit.ResetReq == 1) && (Holding_Memory[BOARD_ID_RX].UI_Status.bit.Ready == 1)) {if(sequence_step_1CH == 0) NVIC_SystemReset();}
    
    ufRefDCV.ulValue  = Holding_Memory[BOARD_ID_RX].DC_Output_Voltage_high;
    ufRefDCV.ulValue |= Holding_Memory[BOARD_ID_RX].DC_Output_Voltage_low << 16;
    DCVout_1CH = ufRefDCV.fValue;
    
    ufRefDCI.ulValue  = Holding_Memory[BOARD_ID_RX].DC_Output_Current_high;
    ufRefDCI.ulValue |= Holding_Memory[BOARD_ID_RX].DC_Output_Current_low << 16;
    DCIout_1CH = (uint16_t)ufRefDCI.fValue;
    

    ufRefDCW.ulValue  = Holding_Memory[BOARD_ID_RX].DC_PowerMeter_high;
    ufRefDCW.ulValue |= Holding_Memory[BOARD_ID_RX].DC_PowerMeter_low << 16;
    DCWatt_1CH = ufRefDCW.fValue;
	

    if(Holding_Memory[BOARD_ID_RX].Cable_Type == COMBO_Test)            charging_type_select_1CH = load_test;
    else if(Holding_Memory[BOARD_ID_RX].Cable_Type == AC3)              charging_type_select_1CH = load_test;
    else if(Holding_Memory[BOARD_ID_RX].Cable_Type != COMBO_Test)       charging_type_select_1CH = combo_type;
    
    if(Holding_Memory[BOARD_ID_RX].CarPayment.bit.Cert_Completed == 1)  carpayment_authentication_complated = 1;// 카페이먼트 서버 인증 완료
    else                                                                carpayment_authentication_complated = 0;
    
    if(Holding_Memory[BOARD_ID_RX].CarPayment.bit.MAC_CERT_PASS == 1)   mac_address_authentication_complated = 1;// mac 어드레스 서버 인증 완료
    else                                                                mac_address_authentication_complated = 0;
    
    if(Holding_Memory[BOARD_ID_RX].CarPayment.bit.MAC_CERT_FAIL == 1)   mac_address_authentication_failed = 1; // mac 어드레스 서버 인증 실패
    else                                                                mac_address_authentication_failed = 0;
    
    set_power                   = Holding_Memory[BOARD_ID_RX].Set_Power;
    number_of_module            = Holding_Memory[BOARD_ID_RX].Set_Module_Num;
    
	//20260611 PHE v2g discharge data parse
	dc_side_current_limit = Holding_Memory[BOARD_ID_RX].Reserved212 * 1000;
    // 남부산전은 에스트래픽과 프로토콜 길이 다름. 아래 주소 적용 안 됨.
    EVSE_ID_buf[4] = (uint16_t)(Holding_Memory[BOARD_ID_RX].Carpay_EVSE_ID1 >>8); // 2021.05.24
    EVSE_ID_buf[5] = (uint16_t)(Holding_Memory[BOARD_ID_RX].Carpay_EVSE_ID1);
    EVSE_ID_buf[6] = (uint16_t)(Holding_Memory[BOARD_ID_RX].Carpay_EVSE_ID2 >>8);
    EVSE_ID_buf[7] = (uint16_t)(Holding_Memory[BOARD_ID_RX].Carpay_EVSE_ID2);
    
	
    memcpy((void *)&Holding_Memory[BOARD_ID_RX], (const void *)&usRegHoldingBuf_2_1, sizeof(__HMI_Holding_Memory));
  }
}

void HMI_tx_data_make(void) // Read, (Control Board -> HMI)
{
  BOARD_ID_TX = 1;
  
  if(control_pilot_1CH == 1)        Input_Memory[1].Charger_Status.bit.Plug_Check = 1;
  if(control_pilot_1CH == 0)        Input_Memory[1].Charger_Status.bit.Plug_Check = 0;

  //20260610 PHE 수정 load_test_display_value
  //404 //Input_Memory[BOARD_ID_TX].Fw_Version       = fw_version; // 보드버전 ui상에서 B로 표기 B1.00
  Input_Memory[BOARD_ID_TX].Fw_Version = load_test_display_value_404;
  
  if(mac_address_recevied_complete == 1)        Input_Memory[BOARD_ID_TX].Charger_Status.bit.mac_address_recevied_complete = 1;
  else                                          Input_Memory[BOARD_ID_TX].Charger_Status.bit.mac_address_recevied_complete = 0;
  
  if(Digital_input.bit.I_DOOR != 1)             Input_Memory[1].Charger_Status.bit.Door_Check = 1;      //프로토콜에서는 close : 1로 지정
  else                                          Input_Memory[1].Charger_Status.bit.Door_Check = 0;
  
  
  
  if(BOARD_ID_TX == 1)
  {
	
    //404 //if(SECC_Msg[1].SECC_EV_SoC_Related_Parameters.remainingTimeToBulkSOC_67 != 0) Input_Memory[BOARD_ID_TX].BulkTime_SOC = SECC_Msg[1].SECC_EV_SoC_Related_Parameters.remainingTimeToBulkSOC_67 * 10;
    //else                                                                          Input_Memory[BOARD_ID_TX].BulkTime_SOC = SECC_Msg[1].SECC_EV_SoC_Related_Parameters.remainingTimeToFullSOC_45 * 10;
    Input_Memory[BOARD_ID_TX].BulkTime_SOC      = load_test_display_value_405;
	  
    Input_Memory[BOARD_ID_TX].Stop_Code           = stop_code_1CH; //
    //406 //Input_Memory[BOARD_ID_TX].EV_SOC              = (uint16_t)SOC_1CH;  // 충전량(%)  
	Input_Memory[BOARD_ID_TX].EV_SOC              = load_test_display_value_406;
	Input_Memory[BOARD_ID_TX].out_voltage         = u16_Vout_Module_G1; //20260610 PHE 408 그대로 사용
    Input_Memory[BOARD_ID_TX].out_current         = u16_Iout_Module_G1; //20260610 PHE 409 그대로 사용
    Input_Memory[BOARD_ID_TX].Coupler_TempP       = coupler_temperature_P_1CH; //20260610 PHE 417 그대로 사용
    //419 //Input_Memory[BOARD_ID_TX].Target_Voltage      = (uint16_t) ((float)SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetVoltage_01 * 0.1F); // 타겟 전압
    Input_Memory[BOARD_ID_TX].Target_Voltage      = (module_read_data[1].read_single_pm_basic_information.ac_ab_line_voltage\
	  												+ module_read_data[1].read_single_pm_basic_information.ac_bc_line_voltage\
													  +module_read_data[1].read_single_pm_basic_information.ac_ca_line_voltage)/3000;
	
	//420 //Input_Memory[BOARD_ID_TX].Demand_Current      = (uint16_t) ((float)SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetCurrent_23 * 0.1F); // 요청 전류
    Input_Memory[BOARD_ID_TX].Demand_Current	  = (((module_read_data[1].read_single_pm_ac_side_information.ac_a_phase_current\
	  												+module_read_data[1].read_single_pm_ac_side_information.ac_b_phase_current\
													+module_read_data[1].read_single_pm_ac_side_information.ac_c_phase_current)/3)
	  												+((module_read_data[2].read_single_pm_ac_side_information.ac_a_phase_current\
	  												+module_read_data[2].read_single_pm_ac_side_information.ac_b_phase_current\
													+module_read_data[2].read_single_pm_ac_side_information.ac_c_phase_current)/3)
													+((module_read_data[3].read_single_pm_ac_side_information.ac_a_phase_current\
	  												+module_read_data[3].read_single_pm_ac_side_information.ac_b_phase_current\
													+module_read_data[3].read_single_pm_ac_side_information.ac_c_phase_current)/3))/1000;
	
	//403 //Input_Memory[BOARD_ID_TX].CP_Voltage          = SECC_Msg[1].SECC_Status_2.cpVoltage_12; //CP_Voltage
    Input_Memory[BOARD_ID_TX].CP_Voltage          = load_test_display_value_403;
	//410 //Input_Memory[BOARD_ID_TX].SECC_Status_Code    = (uint8_t)SECC_Msg[1].SECC_Status_1.seccStatusCode;
	//410 //Input_Memory[BOARD_ID_TX].SECC_Error_Code     = (uint8_t)SECC_Msg[1].SECC_Status_1.seccErrorCode << 8;
	Input_Memory[BOARD_ID_TX].SECC_Status_Code    = (u16_Vout_Module_G1*u16_Iout_Module_G1)/1000;
    
    //421 //Input_Memory[BOARD_ID_TX].Module_Temp         = (uint8_t)module_internal_temperature; // 1번 모듈의 주변온도
    Input_Memory[BOARD_ID_TX].Module_Temp         = (module_read_data[1].read_single_pm_ac_side_information.total_active_power\
	  												+module_read_data[2].read_single_pm_ac_side_information.total_active_power\
													  +module_read_data[3].read_single_pm_ac_side_information.total_active_power)/1000000;
	Input_Memory[BOARD_ID_TX].EV_Error_Code       = (uint8_t)SECC_Msg[1].SECC_Status_1.evErrorCode;
    Input_Memory[BOARD_ID_TX].EV_Atten_Value      = SECC_Msg[1].SECC_EVCC_Info.evccAttn_6 << 8;
    Input_Memory[BOARD_ID_TX].evccMAC_1           = SECC_Msg[1].SECC_EVCC_Info.evccMAC_0;
    Input_Memory[BOARD_ID_TX].evccMAC_1          |= SECC_Msg[1].SECC_EVCC_Info.evccMAC_1 << 8;
    Input_Memory[BOARD_ID_TX].evccMAC_2           = SECC_Msg[1].SECC_EVCC_Info.evccMAC_2;
    Input_Memory[BOARD_ID_TX].evccMAC_2          |= SECC_Msg[1].SECC_EVCC_Info.evccMAC_3 << 8;
    Input_Memory[BOARD_ID_TX].evccMAC_3           = SECC_Msg[1].SECC_EVCC_Info.evccMAC_4;
    Input_Memory[BOARD_ID_TX].evccMAC_3          |= SECC_Msg[1].SECC_EVCC_Info.evccMAC_5 << 8;
    Input_Memory[BOARD_ID_TX].FullTime_SOC        = SECC_Msg[1].SECC_EV_SoC_Related_Parameters.remainingTimeToFullSOC_45 * 10;
    //415,416 신규추가 20260610 PHE
	module_info_ac_line_voltage_avg.fValue      = (module_read_data[1].read_single_pm_basic_information.ac_ab_line_voltage\
	  												+ module_read_data[1].read_single_pm_basic_information.ac_bc_line_voltage\
													  +module_read_data[1].read_single_pm_basic_information.ac_ca_line_voltage)/3.0f;
	Input_Memory[BOARD_ID_TX].AC_PowerMeter_high = (uint16_t)((module_info_ac_line_voltage_avg.ulValue >> 16) & 0xFFFF);
	Input_Memory[BOARD_ID_TX].AC_PowerMeter_low  = (uint16_t)(module_info_ac_line_voltage_avg.ulValue & 0xFFFF);
	//401,402 신규추가 20260610 PHE
	module_info_ac_phase_current_avg.fValue = 		(((module_read_data[1].read_single_pm_ac_side_information.ac_a_phase_current\
	  												+module_read_data[1].read_single_pm_ac_side_information.ac_b_phase_current\
													+module_read_data[1].read_single_pm_ac_side_information.ac_c_phase_current)/3)
	  												+((module_read_data[2].read_single_pm_ac_side_information.ac_a_phase_current\
	  												+module_read_data[2].read_single_pm_ac_side_information.ac_b_phase_current\
													+module_read_data[2].read_single_pm_ac_side_information.ac_c_phase_current)/3)
													+((module_read_data[3].read_single_pm_ac_side_information.ac_a_phase_current\
	  												+module_read_data[3].read_single_pm_ac_side_information.ac_b_phase_current\
													+module_read_data[3].read_single_pm_ac_side_information.ac_c_phase_current)/3));
	Input_Memory[BOARD_ID_TX].Run_Count = (uint16_t)((module_info_ac_phase_current_avg.ulValue >> 16) & 0xFFFF);
	Input_Memory[BOARD_ID_TX].Stop_Code = (uint16_t)(module_info_ac_phase_current_avg.ulValue & 0xFFFF);
	
	module_info_total_active_power_avg.fValue = (module_read_data[1].read_single_pm_ac_side_information.total_active_power\
	  												+module_read_data[2].read_single_pm_ac_side_information.total_active_power\
													  +module_read_data[3].read_single_pm_ac_side_information.total_active_power)/1000.0f;
	Input_Memory[BOARD_ID_TX].Reserved_411 = (uint16_t)((module_info_total_active_power_avg.ulValue >> 16) & 0xFFFF);;
	Input_Memory[BOARD_ID_TX].Reserved_412 = (uint16_t)(module_info_total_active_power_avg.ulValue & 0xFFFF);
	
	Input_Memory[BOARD_ID_TX].DC_PowerMeter_high = load_test_display_value_413;
	Input_Memory[BOARD_ID_TX].DC_PowerMeter_low = load_test_display_value_414;
	Input_Memory[BOARD_ID_TX].Coupler_TempN = (module_read_data[1].read_single_pm_basic_information.power_module_ambient_temperature\
	  											+module_read_data[2].read_single_pm_basic_information.power_module_ambient_temperature\
												  +module_read_data[3].read_single_pm_basic_information.power_module_ambient_temperature)/3000;
    //Coupler Detection, to HMI
    if     (Digital_input.bit.I_CABLE_1CH == 1)   Input_Memory[BOARD_ID_TX].Charger_Status.bit.Connector_Lock_COMBO = 1; // 커플러 거치 됨  ,  2021.07.01  CN5
    else                                          Input_Memory[BOARD_ID_TX].Charger_Status.bit.Connector_Lock_COMBO = 0; // 커플러 거치안됨 
    
  }

  if(BOARD_ID_TX == 1) memcpy((void *)&usRegInputBuf_2_1, (const void *)&Input_Memory[BOARD_ID_TX], sizeof(__HMI_Input_Memory));

  
}

void HMI_memory_reset(void) 
{
  if(memset_flag_1CH++ == 0) memset(&usRegHoldingBuf_2_1, 0, sizeof(__HMI_Holding_Memory));
  else memset_flag_1CH = 1;
}



void Rxd_SCI_2()
{
  u8  crc_buf_L, crc_buf_H;
  u8  j = 0, k = 0;
  
  if (rxok_2 == 1)
  {
    rxok_2 = 0;
    
    crc_buf_L = make8(CRC16 (bRxSio_2, cRxdMax_2-2), 0);
    crc_buf_H = make8(CRC16 (bRxSio_2, cRxdMax_2-2), 1);
    
    if (bRxSio_2[cRxdMax_2-2] == crc_buf_L && bRxSio_2[cRxdMax_2-1] == crc_buf_H) // data 정상
    {
      cRxd_2 = 0;
      hmi_485_communication_rx_fault_count = 0;
      
      if      (bRxSio_2[1] == 0x04) //Read
      {
        if     ((bRxSio_2[2] == 0x01) && (bRxSio_2[3] == 0x90))     { txnew_2 = 1;    }     //Read 400
        else if((bRxSio_2[2] == 0x01) && (bRxSio_2[3] == 0xF4))     { txnew_Test = 1; }     //Read(Test) 500
        else                                                        { txnew_2 = 0; txnew_Test = 0; cRxd_2 = 0;}
      }
      else if (bRxSio_2[1] == 0x10) //Write
      {
        if((bRxSio_2[2] == 0x00) && (bRxSio_2[3] == 0xC8))                  //Write 200
        {
          txnew_2 = 1;
          for(j =7; j<67; j++)
          {
              usRegHoldingBuf_2_1[k]  = (uint16_t)(bRxSio_2[j]) << 8;       //200 H
              j++;
              usRegHoldingBuf_2_1[k] |= (uint16_t)(bRxSio_2[j]);            //200 L
              k++;
          }
        }
        else if((bRxSio_2[2] == 0x01) && (bRxSio_2[3] == 0x2C))             //Write(Test) 300
        {
          txnew_Test = 1;
          for(j=7; j<15; j++)
          {
            usRegHoldingBuf_Test[k]  = (uint16_t)(bRxSio_2[j]) << 8; //300 H
            j++;
            usRegHoldingBuf_Test[k] |= (uint16_t)(bRxSio_2[j]);      //300 L
            k++;
          }
        }
        else 
        {
          txnew_2 = 0;
          txnew_Test = 0;
          cRxd_2 = 0;
        }	
      }
      else 
      {
        txnew_2 = 0;
        txnew_Test = 0;
        cRxd_2 = 0;
      }
    }
  }
}

void Txd_SCI_2()
{
  u8 j = 0, k = 0, m = 0;
  u8 buf;
  uint16_t buf_16;
  
  if (txnew_2 == 1)
  {
    txnew_2 = 0;
    U2_Enable;
    
    if((bRxSio_2[1] == 0x04) && (bRxSio_2[2] == 0x01) && (bRxSio_2[3] == 0x90))      //Read 400
    {
      bTxSio_2[0] = BOARD_ID_TX;
      bTxSio_2[1] = 0x04;
      bTxSio_2[2] = ReadCnt * 2;
      cTxd_2 = bTxSio_2[2] + 3;
      
      for(j = 3; j < cTxd_2 ; j++)        
      {
          bTxSio_2[j] = usRegInputBuf_2_1[k] >> 8;   
          j++;
          bTxSio_2[j] = usRegInputBuf_2_1[k];
          k++;
      }
      
      buf_16 = CRC16 (bTxSio_2, cTxd_2);
      buf = buf_16;
      bTxSio_2[cTxd_2++] = buf;
      buf = buf_16>>8;
      bTxSio_2[cTxd_2++] = buf;
      cTxdMax_2 = cTxd_2;
      
      DMA1_Channel7->CMAR = (uint32_t)bTxSio_2;
      DMA1_Channel7->CNDTR = cTxdMax_2;
      
      DMA_Cmd(DMA1_Channel7, ENABLE);       //Enable DMA1 Channel transfer
    }
    
    if((bRxSio_2[1] == 0x10) && (bRxSio_2[2] == 0x00) && (bRxSio_2[3] == 0xC8))     //Write 200
    {
      for(m = 0; m <6; m++)	
      {
        bTxSio_2[m] = bRxSio_2[m];
      }
      cTxd_2 = 6;
      
      buf_16 = CRC16 (bTxSio_2, cTxd_2);
      buf = buf_16;
      bTxSio_2[cTxd_2++] = buf;
      buf = buf_16>>8;
      bTxSio_2[cTxd_2++] = buf;
      
      cTxdMax_2 = cTxd_2;
      
      DMA1_Channel7->CMAR = (uint32_t)bTxSio_2;
      DMA1_Channel7->CNDTR = cTxdMax_2;
      
      DMA_Cmd(DMA1_Channel7, ENABLE);       //Enable DMA1 Channel transfer
    }
  }
  
  if(usart2_tx_ok == 1) // 전송 완료 후 수신 모드로 전환
  {
    if(++counter_ustart2_tx >= 10)
    {
      usart2_tx_ok = 0;
      U2_Disable;
    }
  }
  else        counter_ustart2_tx = 0;
  
}

uint8_t DM_CS_u8(uint8_t *Data, int n)
{
  uint8_t CS_temp = 0;
  
  for ( uint8_t i=1; i < n; i++)
  {
    CS_temp += Data[i];
  }
  return CS_temp;
}

uint16_t CRC16 (const uint8_t *nData, uint8_t wLength) // modbus
{
  static const uint16_t wCRCTable[] = {
    0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
    0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
    0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
    0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
    0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
    0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
    0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
    0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
    0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
    0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
    0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
    0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
    0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
    0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
    0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
    0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
    0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
    0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
    0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
    0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
    0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
    0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
    0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
    0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
    0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
    0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
    0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
    0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
    0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
    0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
    0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
    0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 };
  
  uint8_t nTemp;
  uint16_t wCRCWord = 0xFFFF;
  
  while (wLength--)
  {
    nTemp = *nData++ ^ wCRCWord;
    wCRCWord >>= 8;
    wCRCWord ^= wCRCTable[nTemp];
  }
  return wCRCWord;
  
}

/*******************************************************************************
* Function Name : u8 make8(u16 var, u8 offset)
* Description   : make8
* Parameters    : u16 var, u8 offset
* Return        : u8
*******************************************************************************/
u8 make8(u16 var, u8 offset)
{
  u8 test;
  test = ( (var >> (offset*8) ) & 0xff);
  return test;           
}

