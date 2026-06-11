#define _SIM_100_C

#include "stm32f10x.h"
#include "SIM_100.h"
#include "stm32f105_init.h"
#include "fault_check.h"

uint16_t SIM_100_Tx_step = 0;
uint16_t    SIM_Battery_Voltage_Max = 0;
uint8_t     SIM_cnt_100ms_1 = 0;
uint8_t     SIM_cnt_100ms_2 = 0;
uint8_t     SIM_cnt_100ms_3 = 0;
uint8_t     SIM_cnt_100ms_4 = 0;
uint8_t     SIM_cnt_100ms_5 = 0;
uint8_t     SIM_cnt_100ms_6 = 0;
uint8_t     SIM_cnt_100ms_7 = 0;
uint8_t     SIM_cnt_100ms_8 = 0;
uint8_t     SIM_cnt_100ms_9 = 0;
uint8_t     Set_max_battery_flag = 0;
uint8_t     SIM_100_tx_cnt = 0;

void SIM_100_Transmit(void)
{
////  if(++SIM_cnt_100ms_1 >= 10)
////  {
////      SIM_cnt_100ms_1 = 0;
////      SIM_100_Tx_step = Isolation_state;
////  }
////  if(++SIM_cnt_100ms_2 >= 11)
////  {
////      SIM_cnt_100ms_2 = 1;
////      SIM_100_Tx_step = Isolation_resistances;
////  }
////  if(++SIM_cnt_100ms_3 >= 12)
////  {
////      SIM_cnt_100ms_3 = 2;
////      SIM_100_Tx_step = Isolation_capacitances;
////  }
////  if(++SIM_cnt_100ms_4 >= 13)
////  {
////      SIM_cnt_100ms_4 = 3;
////      SIM_100_Tx_step = Voltages_Vp_Vn;
////  }
////  if(++SIM_cnt_100ms_5 >= 14)
////  {
////      SIM_cnt_100ms_5 = 4;
////      SIM_100_Tx_step = Battery_voltage_Vb;
////  }
////  if(++SIM_cnt_100ms_7 >= 15)
////  {
////      SIM_cnt_100ms_7 = 5;
////      SIM_100_Tx_step = Error_flags;
////  }
////  if(++SIM_cnt_100ms_8 >= 16)
////  {
////      SIM_cnt_100ms_8 = 6;
////      SIM_100_Tx_step = Temperature;
////  }
////  if(++SIM_cnt_100ms_9 >= 17)
////  {
////      SIM_cnt_100ms_9 = 7;
////      if(Set_max_battery_flag == 1)
////      {
////        SIM_100_Tx_step = Set_max_battery_design_voltage;
////      }
////  }
//  SIM_100_tx_cnt++;
//  if(SIM_100_tx_cnt > 3) SIM_100_tx_cnt = 1;
////  if(SIM_100_tx_cnt == 1)
////  {
////    SIM_100_Tx_step = Isolation_state;
////  }
////  else if(SIM_100_tx_cnt == 2)
////  {
////    SIM_100_Tx_step = Isolation_resistances;
////  }
////  else if(SIM_100_tx_cnt == 3)
////  {
////    SIM_100_Tx_step = Isolation_capacitances;
////  }
////  else if(SIM_100_tx_cnt == 4)
////  {
////    SIM_100_Tx_step = Voltages_Vp_Vn;
////  }
//  if(SIM_100_tx_cnt == 1)
//  {
//    SIM_100_Tx_step = Battery_voltage_Vb;
//  }
////  else if(SIM_100_tx_cnt == 6)
////  {
////    SIM_100_Tx_step = Error_flags;
////  }
////  else if(SIM_100_tx_cnt == 7)
////  {
////    SIM_100_Tx_step = Temperature;
////  }
//  else if(SIM_100_tx_cnt == 2)
//  {
//    if(Set_max_battery_flag == 1)
//    {
//      SIM_100_Tx_step = Set_max_battery_design_voltage;
//    }
//  }
  
  // 2024 02 20 박형은 추가 
  SIM_100_tx_cnt++;
  if(SIM_100_tx_cnt == 1)
  {
    SIM_100_Tx_step = Battery_voltage_Vb;
  }
  
  else if(SIM_100_tx_cnt == 2)
  {
    if(Set_max_battery_flag == 1)
    {
      SIM_100_Tx_step = Set_max_battery_design_voltage;
    }
  }
  
  else if(SIM_100_tx_cnt == 3)
  {
    SIM_100_Tx_step = Isolation_state;
  }  
  
  else if(SIM_100_tx_cnt == 4)
  {
    SIM_100_Tx_step = Isolation_resistances;
  }  
    
  else if(SIM_100_tx_cnt == 5)
  {
    SIM_100_Tx_step = Isolation_capacitances;
  }  
  
  else if(SIM_100_tx_cnt == 6)
  {
    SIM_100_Tx_step = Voltages_Vp_Vn;
  }  

  else if(SIM_100_tx_cnt == 7)
  {
    SIM_100_Tx_step = Error_flags;
  }  

  else if(SIM_100_tx_cnt == 8)
  {
    SIM_100_tx_cnt = 0;
    SIM_100_Tx_step = Temperature;
  }  
  

  switch(SIM_100_Tx_step)
  {
  case Isolation_state:
    TxMessage2.ExtId   = EVSE_ID_SID;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 1;
    TxMessage2.Data[0] = Isolation_state;
    TxMessage2.Data[1] = 0;
    TxMessage2.Data[2] = 0;
    TxMessage2.Data[3] = 0;
    TxMessage2.Data[4] = 0;
    TxMessage2.Data[5] = 0;
    TxMessage2.Data[6] = 0;
    TxMessage2.Data[7] = 0;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  case Isolation_resistances:
    TxMessage2.ExtId   = EVSE_ID_SID;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 1;
    TxMessage2.Data[0] = Isolation_resistances;
    TxMessage2.Data[1] = 0;
    TxMessage2.Data[2] = 0;
    TxMessage2.Data[3] = 0;
    TxMessage2.Data[4] = 0;
    TxMessage2.Data[5] = 0;
    TxMessage2.Data[6] = 0;
    TxMessage2.Data[7] = 0;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  case Isolation_capacitances:
    TxMessage2.ExtId   = EVSE_ID_SID;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 1;
    TxMessage2.Data[0] = Isolation_capacitances;
    TxMessage2.Data[1] = 0;
    TxMessage2.Data[2] = 0;
    TxMessage2.Data[3] = 0;
    TxMessage2.Data[4] = 0;
    TxMessage2.Data[5] = 0;
    TxMessage2.Data[6] = 0;
    TxMessage2.Data[7] = 0;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  case Voltages_Vp_Vn:
    TxMessage2.ExtId   = EVSE_ID_SID;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 1;
    TxMessage2.Data[0] = Voltages_Vp_Vn;
    TxMessage2.Data[1] = 0;
    TxMessage2.Data[2] = 0;
    TxMessage2.Data[3] = 0;
    TxMessage2.Data[4] = 0;
    TxMessage2.Data[5] = 0;
    TxMessage2.Data[6] = 0;
    TxMessage2.Data[7] = 0;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  case Battery_voltage_Vb:
    TxMessage2.ExtId   = EVSE_ID_SID;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 1;
    TxMessage2.Data[0] = Battery_voltage_Vb;
    TxMessage2.Data[1] = 0;
    TxMessage2.Data[2] = 0;
    TxMessage2.Data[3] = 0;
    TxMessage2.Data[4] = 0;
    TxMessage2.Data[5] = 0;
    TxMessage2.Data[6] = 0;
    TxMessage2.Data[7] = 0;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  case Error_flags:
    TxMessage2.ExtId   = EVSE_ID_SID;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 1;
    TxMessage2.Data[0] = Error_flags;
    TxMessage2.Data[1] = 0;
    TxMessage2.Data[2] = 0;
    TxMessage2.Data[3] = 0;
    TxMessage2.Data[4] = 0;
    TxMessage2.Data[5] = 0;
    TxMessage2.Data[6] = 0;
    TxMessage2.Data[7] = 0;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  case Temperature:
    TxMessage2.ExtId   = EVSE_ID_SID;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 1;
    TxMessage2.Data[0] = Temperature;
    TxMessage2.Data[1] = 0;
    TxMessage2.Data[2] = 0;
    TxMessage2.Data[3] = 0;
    TxMessage2.Data[4] = 0;
    TxMessage2.Data[5] = 0;
    TxMessage2.Data[6] = 0;
    TxMessage2.Data[7] = 0;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  case Restart:
    TxMessage2.ExtId   = EVSE_ID_SID;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 5;
    TxMessage2.Data[0] = Restart;
    TxMessage2.Data[1] = 0x01;
    TxMessage2.Data[2] = 0x23;
    TxMessage2.Data[3] = 0x45;
    TxMessage2.Data[4] = 0x67;
    TxMessage2.Data[5] = 0;
    TxMessage2.Data[6] = 0;
    TxMessage2.Data[7] = 0;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  case Set_max_battery_design_voltage:
    TxMessage2.ExtId   = EVSE_ID_SID;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 3;
    TxMessage2.Data[0] = Set_max_battery_design_voltage;
    TxMessage2.Data[1] = (uint8_t)(SIM_Battery_Voltage_Max >> 8); // SIM100_수정
    TxMessage2.Data[2] = (uint8_t)(SIM_Battery_Voltage_Max); // SIM100_수정
    TxMessage2.Data[3] = 0;
    TxMessage2.Data[4] = 0;
    TxMessage2.Data[5] = 0;
    TxMessage2.Data[6] = 0;
    TxMessage2.Data[7] = 0;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  }
}

void SIM_100_Receive(uint16_t SIM_100_Rx_step)
{
  imd_can_communication_rx_fault_count = 0;
  
  switch(SIM_100_Rx_step)
  {
  case Isolation_state:
    SIM_info.SIM_Isolation_State.Status_Bits.all = RxMessage2.Data[1];
    SIM_info.SIM_Isolation_State.Electrical_isolation = (uint16_t)((RxMessage2.Data[2] << 8) | RxMessage2.Data[3]);
    SIM_info.SIM_Isolation_State.Electrical_isolation_uncertainty = RxMessage2.Data[4];
    SIM_info.SIM_Isolation_State.Energy_stored = (uint16_t)((RxMessage2.Data[5] << 8) | RxMessage2.Data[6]);
    SIM_info.SIM_Isolation_State.Energy_stored_uncertainty = RxMessage2.Data[7];
    break;
    
  case Isolation_resistances:
    SIM_info.SIM_Isolation_Resistances.Status_Bits.all = RxMessage2.Data[1];
    SIM_info.SIM_Isolation_Resistances.Rp = (uint16_t)((RxMessage2.Data[2] << 8) | RxMessage2.Data[3]);
    SIM_info.SIM_Isolation_Resistances.Rp_uncertainty = RxMessage2.Data[4];
    SIM_info.SIM_Isolation_Resistances.Rn = (uint16_t)((RxMessage2.Data[5] << 8) | RxMessage2.Data[6]);
    SIM_info.SIM_Isolation_Resistances.Rn_uncertainty = RxMessage2.Data[7];
    break;
    
  case Isolation_capacitances:
    SIM_info.SIM_Isolation_Capacitances.Status_Bits.all = RxMessage2.Data[1];
    SIM_info.SIM_Isolation_Capacitances.Cp = (uint16_t)((RxMessage2.Data[2] << 8) | RxMessage2.Data[3]);
    SIM_info.SIM_Isolation_Capacitances.Cp_uncertainty = RxMessage2.Data[4];
    SIM_info.SIM_Isolation_Capacitances.Cn = (uint16_t)((RxMessage2.Data[5] << 8) | RxMessage2.Data[6]);
    SIM_info.SIM_Isolation_Capacitances.Cn_uncertainty = RxMessage2.Data[7];
    break;
    
  case Voltages_Vp_Vn:
    SIM_info.SIM_Vp_Vn.Status_Bits.all = RxMessage2.Data[1];
    SIM_info.SIM_Vp_Vn.Vp = (uint16_t)((RxMessage2.Data[2] << 8) | RxMessage2.Data[3]);
    SIM_info.SIM_Vp_Vn.Vp_uncertainty = RxMessage2.Data[4];
    SIM_info.SIM_Vp_Vn.Vn = (uint16_t)((RxMessage2.Data[5] << 8) | RxMessage2.Data[6]);
    SIM_info.SIM_Vp_Vn.Vn_uncertainty = RxMessage2.Data[7];
    break;
    
  case Battery_voltage_Vb:
    SIM_info.SIM_Battery_Voltage.Status_Bits.all = RxMessage2.Data[1];
    SIM_info.SIM_Battery_Voltage.Vb = (uint16_t)((RxMessage2.Data[2] << 8) | RxMessage2.Data[3]);
    SIM_info.SIM_Battery_Voltage.Vb_uncertainty = RxMessage2.Data[4];
    SIM_info.SIM_Battery_Voltage.Vb_max = (uint16_t)((RxMessage2.Data[5] << 8) | RxMessage2.Data[6]);
    SIM_info.SIM_Battery_Voltage.Vb_max_uncertainty = RxMessage2.Data[7];
    break;
    
  case Error_flags:
    SIM_info.SIM_Error_Flags.Status_Bits.all = RxMessage2.Data[1];
    SIM_info.SIM_Error_Flags.Error_Flags.all = RxMessage2.Data[2];
    break;
    
  case Temperature:
    SIM_info.SIM_Temperature = RxMessage2.Data[1] << 24;
    SIM_info.SIM_Temperature += RxMessage2.Data[2]<< 16;
    SIM_info.SIM_Temperature += RxMessage2.Data[3]<< 8;
    SIM_info.SIM_Temperature += RxMessage2.Data[4];
    break;
    
  case Set_max_battery_design_voltage:
    break; 
  }
} 