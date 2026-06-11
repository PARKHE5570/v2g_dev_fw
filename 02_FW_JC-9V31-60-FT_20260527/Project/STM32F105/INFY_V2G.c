#define __INFY_V2G_MODULE

#include "stm32f10x.h"
#include "INFY_V2G.h"
#include "main.h"
//#include "CAN_Betw_ST_PU.h"

void Module_Tx_Init()
{
  module_stop_button = 1;
  module_start_button = 0;
  module_rectification = 0;
  module_grid_connected_inverter = 0;
  module_off_grid_inverter = 0;
  Vout_ref_temp_G1 = 0;
  Iout_ref_temp_G1 = 0;
  Module_Dial_Switch();
  discharging_limit_iout = 0;
}

void Module_Dial_Switch()               //모듈 앞 Dial code를 기존의 Group이 아닌 address로 변경하는 명령
{
  TxMessage1.ExtId = 0x02A43FF0;
  TxMessage1.DLC = 8;
  TxMessage1.IDE = CAN_ID_EXT;
  TxMessage1.Data[0] = 0x11;
  TxMessage1.Data[1] = 0x23;
  TxMessage1.Data[2] = 0x00; 
  TxMessage1.Data[3] = 0x00; 
  TxMessage1.Data[4] = 0x00; 
  TxMessage1.Data[5] = 0x00; 
  TxMessage1.Data[6] = 0x00; 
  TxMessage1.Data[7] = 0xA1;
  CAN_Transmit(CAN1, &TxMessage1);
}

void Rx_Module(uint32_t can_id)
{
  uint16_t module_group_num = (can_id & 0xFF);
  uint16_t command_seperation = ((can_id >> 16) & 0x3F);
  
  power_module_Communition_error_counter = 0;
  uint8_t byte_0 = 0;
  
  cnt_module_can_error = 0;
  
  byte_0 = RxMessage1.Data[0];

  if(command_seperation == 0x23)
  {
    switch(byte_0)
    {
    case SYSTEM_BASIC_INFO:
      
      if(RxMessage1.Data[1] == 0x01)
      {
        module_read_total_data.read_system_basic_information.system_dc_voltage = RxMessage1.Data[4] << 24;
        module_read_total_data.read_system_basic_information.system_dc_voltage |= RxMessage1.Data[5] << 16;
        module_read_total_data.read_system_basic_information.system_dc_voltage |= RxMessage1.Data[6] << 8;
        module_read_total_data.read_system_basic_information.system_dc_voltage |= RxMessage1.Data[7];
        
        Vout_Module_G1 = (module_read_total_data.read_system_basic_information.system_dc_voltage/1000);
        Vout_G1 = Vout_Module_G1;
        u16_Vout_Module_G1 = (uint16_t)Vout_Module_G1;
      }
      
      if(RxMessage1.Data[1] == 0x02)
      {
        module_read_total_data.read_system_basic_information.system_dc_total_current = RxMessage1.Data[4] << 24;
        module_read_total_data.read_system_basic_information.system_dc_total_current |= RxMessage1.Data[5] << 16;
        module_read_total_data.read_system_basic_information.system_dc_total_current |= RxMessage1.Data[6] << 8;
        module_read_total_data.read_system_basic_information.system_dc_total_current |= RxMessage1.Data[7];
        
        Iout_Module_G1 = (module_read_total_data.read_system_basic_information.system_dc_total_current/1000);
        u16_Iout_Module_G1 = (uint16_t)Iout_Module_G1;
      }
      
      if(RxMessage1.Data[1] == 0x10)
      {
        module_read_total_data.read_system_basic_information.mdl_numbers = RxMessage1.Data[6] << 8;
        module_read_total_data.read_system_basic_information.mdl_numbers |= RxMessage1.Data[7];
      }
      
      break;
      
    case SINGLE_PM_BASIC_INFO:
      
      if(RxMessage1.Data[1] == 0x01)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_voltage = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_voltage |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_voltage |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_voltage |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x02)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_current = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_current |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_current |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_current |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x03)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.ac_ab_line_voltage = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_basic_information.ac_ab_line_voltage |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_basic_information.ac_ab_line_voltage |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_basic_information.ac_ab_line_voltage |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x04)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.ac_bc_line_voltage = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_basic_information.ac_bc_line_voltage |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_basic_information.ac_bc_line_voltage |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_basic_information.ac_bc_line_voltage |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x05)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.ac_ca_line_voltage = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_basic_information.ac_ca_line_voltage |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_basic_information.ac_ca_line_voltage |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_basic_information.ac_ca_line_voltage |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x06)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_ambient_temperature = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_ambient_temperature |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_ambient_temperature |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_ambient_temperature |= RxMessage1.Data[7];
        
        module_internal_temperature = (module_read_data[1].read_single_pm_basic_information.power_module_ambient_temperature/1000);
        
      }
      
      if(RxMessage1.Data[1] == 0x10)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.module_status_2.all = RxMessage1.Data[5];
        module_read_data[module_group_num].read_single_pm_basic_information.module_status_1.all = RxMessage1.Data[6];
        module_read_data[module_group_num].read_single_pm_basic_information.module_status_0.all = RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x11)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.inverter_status_2.all = RxMessage1.Data[5];
        module_read_data[module_group_num].read_single_pm_basic_information.inverter_status_0.all = RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x20)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.module_group_number = RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_basic_information.module_group_number |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x30)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.dc_max_output_voltage = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_basic_information.dc_max_output_voltage |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_basic_information.dc_max_output_voltage |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_basic_information.dc_max_output_voltage |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x31)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.dc_min_output_voltage = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_basic_information.dc_min_output_voltage |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_basic_information.dc_min_output_voltage |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_basic_information.dc_min_output_voltage |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x32)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.dc_max_output_current = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_basic_information.dc_max_output_current |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_basic_information.dc_max_output_current |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_basic_information.dc_max_output_current |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x33)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.dc_rated_ouput_power = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_basic_information.dc_rated_ouput_power |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_basic_information.dc_rated_ouput_power |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_basic_information.dc_rated_ouput_power |= RxMessage1.Data[7];
      }
      break;
      
    case SINGLE_PM_AC_SIDE_INFO:
      
      if(RxMessage1.Data[1] == 0x01)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_voltage = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_voltage |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_voltage |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_voltage |= RxMessage1.Data[7];        
      }
      
      if(RxMessage1.Data[1] == 0x02)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_voltage = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_voltage |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_voltage |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_voltage |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x03)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_voltage = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_voltage |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_voltage |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_voltage |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x04)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_current = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_current |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_current |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_current |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x05)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_current = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_current |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_current |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_current |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x06)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_current = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_current |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_current |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_current |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x07)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_frequency = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_frequency |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_frequency |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_frequency |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x08)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.total_active_power = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.total_active_power |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.total_active_power |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.total_active_power |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x09)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_power = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_power |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_power |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_power |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x0A)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_power = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_power |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_power |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_power |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x0B)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_power = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_power |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_power |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_power |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x0C)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.total_reactive_power = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.total_reactive_power |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.total_reactive_power |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.total_reactive_power |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x0D)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_reactive_power = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_reactive_power |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_reactive_power |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_reactive_power |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x0E)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_reactive_power = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_reactive_power |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_reactive_power |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_reactive_power |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x0F)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_reactive_power = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_reactive_power |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_reactive_power |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_reactive_power |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x10)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.total_apparent_power = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.total_apparent_power |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.total_apparent_power |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.total_apparent_power |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x11)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_apparent_power = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_apparent_power |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_apparent_power |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_a_phase_apparent_power |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x12)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_apparent_power = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_apparent_power |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_apparent_power |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_b_phase_apparent_power |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x13)
      {
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_apparent_power = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_apparent_power |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_apparent_power |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_ac_side_information.ac_c_phase_apparent_power |= RxMessage1.Data[7];
      }
      break;
      
    case SINGLE_BIDIRECTIONAL_PM_BASIC_INFO:
      
      if(RxMessage1.Data[1] == 0x01)
      {
        module_read_data[module_group_num].read_single_bidirectional_pm_basic_information.pm_dc_high_voltage_side_voltage = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_bidirectional_pm_basic_information.pm_dc_high_voltage_side_voltage |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_bidirectional_pm_basic_information.pm_dc_high_voltage_side_voltage |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_bidirectional_pm_basic_information.pm_dc_high_voltage_side_voltage |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x02)
      {
        module_read_data[module_group_num].read_single_bidirectional_pm_basic_information.pm_dc_high_voltage_side_current = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_bidirectional_pm_basic_information.pm_dc_high_voltage_side_current |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_bidirectional_pm_basic_information.pm_dc_high_voltage_side_current |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_bidirectional_pm_basic_information.pm_dc_high_voltage_side_current |= RxMessage1.Data[7];
      }
      
      break;
    }
  }
  else if(command_seperation == 0x24)
  {
    switch(byte_0)
    {
    case 0x11:

      if(RxMessage1.Data[1] == 0x01)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_voltage = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_voltage |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_voltage |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_voltage |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x02)
      {
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_current = RxMessage1.Data[4] << 24;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_current |= RxMessage1.Data[5] << 16;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_current |= RxMessage1.Data[6] << 8;
        module_read_data[module_group_num].read_single_pm_basic_information.power_module_dc_side_current |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x03)
      {
        module_write_data[module_group_num].moduleGroupNumber = RxMessage1.Data[6] << 8;
        module_write_data[module_group_num].moduleGroupNumber |= RxMessage1.Data[7];
      }
      
      if(RxMessage1.Data[1] == 0x10)
      {
        module_write_data[module_group_num].moduleOnAndOff = RxMessage1.Data[6] << 8;
        module_write_data[module_group_num].moduleOnAndOff |= RxMessage1.Data[7];
      }
      break;     
    }
  }
}

void Read_Module_Data()
{  
  if(++cnt_read_module_50ms >= 64)
  {
    cnt_read_module_50ms = 1;    
  }
    switch(cnt_read_module_50ms)
    {
    case 1:
      
      TxMessage1.ExtId = READ_MODULE_ALL_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SYSTEM_BASIC_INFO;
      TxMessage1.Data[1] = 0x01;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
    case 2:
      
      TxMessage1.ExtId = READ_MODULE_ALL_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SYSTEM_BASIC_INFO;
      TxMessage1.Data[1] = 0x02;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
    case 3:
      
      TxMessage1.ExtId = READ_MODULE_ALL_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SYSTEM_BASIC_INFO;
      TxMessage1.Data[1] = 0x10;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
    case 4:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x01;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 5:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x01;
      TxMessage1.Data[2] = 0x00;
      TxMessage1.Data[3] = 0x00;
      TxMessage1.Data[4] = 0x00;
      TxMessage1.Data[5] = 0x00;
      TxMessage1.Data[6] = 0x00;
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 6:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x01;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 7:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x02;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 8:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x02;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 9:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x02;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
    case 10:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x03;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 11:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x03;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 12:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x03;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 13:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x04;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 14:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x04;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 15:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x04;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 16:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x05;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 17:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x05;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 18:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x05;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 19:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x06;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 20:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x06;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 21:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x06;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 22:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x10;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 23:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x10;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 24:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x10;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 25:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x11;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 26:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x11;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 27:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x11;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 28:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x30;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 29:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x30;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 30:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x30;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 31:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x31;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 32:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x31;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 33:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x31;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 34:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x32;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 35:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x32;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 36:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x32;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 37:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x33;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 38:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x33;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;

      case 39:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
      TxMessage1.Data[1] = 0x33;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 40:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x01;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 41:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x01;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 42:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x01;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 43:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x02;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 44:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x02;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 45:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x02;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 46:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x03;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 47:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x03;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 48:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x03;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 49:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x04;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 50:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x04;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 51:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x04;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 52:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x05;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 53:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x05;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 54:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x05;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 55:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x06;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 56:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x06;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 57:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x06;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 58:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x07;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 59:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x07;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 60:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x07;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 61:
      
      TxMessage1.ExtId = READ_MODULE_1_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x08;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      
      case 62:
      
      TxMessage1.ExtId = READ_MODULE_2_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x08;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
	  
	  case 63:
      
      TxMessage1.ExtId = READ_MODULE_3_DATA;
      TxMessage1.DLC = 8;
      TxMessage1.IDE = CAN_ID_EXT;
      TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
      TxMessage1.Data[1] = 0x08;
      TxMessage1.Data[2] = 0x00; 
      TxMessage1.Data[3] = 0x00; 
      TxMessage1.Data[4] = 0x00; 
      TxMessage1.Data[5] = 0x00; 
      TxMessage1.Data[6] = 0x00; 
      TxMessage1.Data[7] = 0x00;
      CAN_Transmit(CAN1, &TxMessage1);
      
      break;
      

    }
}

void Set_Module_Data()
{
     
    if(++cnt_set_module_50ms >= 9)
    {
      cnt_set_module_50ms = 1;
    }
      switch(cnt_set_module_50ms)
      {
      case 1:     //working mode
        
        if(module_rectification)
        {
          module_grid_connected_inverter = 0;
          module_set_data.set_single_pm_ac_basic_information.working_mode = rectification_mode;
          
          TxMessage1.ExtId = ALL_MODULE_SET_DATA;
          TxMessage1.DLC = 8;
          TxMessage1.IDE = CAN_ID_EXT;
          TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
          TxMessage1.Data[1] = 0x10;
          TxMessage1.Data[2] = 0x00; 
          TxMessage1.Data[3] = 0x00; 
          TxMessage1.Data[4] = 0x00; 
          TxMessage1.Data[5] = 0x00; 
          TxMessage1.Data[6] = 0x00; 
          TxMessage1.Data[7] = module_set_data.set_single_pm_ac_basic_information.working_mode;
          CAN_Transmit(CAN1, &TxMessage1);
        }
        
        else if(module_grid_connected_inverter)
        {
          module_rectification = 0;
          module_set_data.set_single_pm_ac_basic_information.working_mode = grid_connected_inverter_mode;
          
          TxMessage1.ExtId = ALL_MODULE_SET_DATA;
          TxMessage1.DLC = 8;
          TxMessage1.IDE = CAN_ID_EXT;
          TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
          TxMessage1.Data[1] = 0x10;
          TxMessage1.Data[2] = 0x00; 
          TxMessage1.Data[3] = 0x00; 
          TxMessage1.Data[4] = 0x00; 
          TxMessage1.Data[5] = 0x00; 
          TxMessage1.Data[6] = 0x00; 
          TxMessage1.Data[7] = module_set_data.set_single_pm_ac_basic_information.working_mode;
          CAN_Transmit(CAN1, &TxMessage1);
        }
        
        //        else if(module01_off_grid_inverter)
        //        {
        //          module_set_data.set_single_pm_ac_basic_information.working_mode = off_grid_inverter_mode;
        //          
        //          TxMessage1.ExtId = SET_MODULE_1_DATA;
        //          TxMessage1.DLC = 8;
        //          TxMessage1.IDE = CAN_ID_EXT;
        //          TxMessage1.Data[0] = SINGLE_PM_AC_SIDE_INFO;
        //          TxMessage1.Data[1] = 0x10;
        //          TxMessage1.Data[2] = 0x00; 
        //          TxMessage1.Data[3] = 0x00; 
        //          TxMessage1.Data[4] = 0x00; 
        //          TxMessage1.Data[5] = 0x00; 
        //          TxMessage1.Data[6] = 0x00; 
        //          TxMessage1.Data[7] = module_set_data.set_single_pm_ac_basic_information.working_mode;
        //          CAN_Transmit(CAN1, &TxMessage1);
        //        }
        
        break;
        
      case 2:     //DC side output high/low voltage mode
        
        //      if(module01_stop_button == 1 && module01_start_button == 0)
        if(module_on_off_G1 == MODULE_OFF)
        {
          module_set_data.set_single_pm_basic_information.dc_side_output_high_or_low_voltage_mode = module_automatic_control_switching;
          
          TxMessage1.ExtId = ALL_MODULE_SET_DATA;
          TxMessage1.DLC = 8;
          TxMessage1.IDE = CAN_ID_EXT;
          TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
          TxMessage1.Data[1] = 0x26;
          TxMessage1.Data[2] = 0x00; 
          TxMessage1.Data[3] = 0x00; 
          TxMessage1.Data[4] = 0x00;
          TxMessage1.Data[5] = 0x00; 
          TxMessage1.Data[6] = 0x00; 
          TxMessage1.Data[7] = module_set_data.set_single_pm_basic_information.dc_side_output_high_or_low_voltage_mode;
          CAN_Transmit(CAN1, &TxMessage1);
        }
        
        break;
        
      case 3:     //System DC voltage
        
        if(module_rectification)
        {
          if(Vout_ref_temp_G1 > 1000)     Vout_ref_temp_G1 = 1000;
          if(Vout_ref_temp_G1 < 150)      Vout_ref_temp_G1 = 150;
          
          module_set_data.set_system_basic_information.system_dc_voltage = (uint32_t)(Vout_ref_temp_G1 * 1000);
          
          TxMessage1.ExtId = ALL_MODULE_SET_DATA;
          TxMessage1.DLC = 8;
          TxMessage1.IDE = CAN_ID_EXT;
          TxMessage1.Data[0] = SYSTEM_BASIC_INFO;
          TxMessage1.Data[1] = 0x01;
          TxMessage1.Data[2] = 0x00; 
          TxMessage1.Data[3] = 0x00; 
          TxMessage1.Data[4] = module_set_data.set_system_basic_information.system_dc_voltage >> 24; 
          TxMessage1.Data[5] = module_set_data.set_system_basic_information.system_dc_voltage >> 16; 
          TxMessage1.Data[6] = module_set_data.set_system_basic_information.system_dc_voltage >> 8; 
          TxMessage1.Data[7] = module_set_data.set_system_basic_information.system_dc_voltage;
          CAN_Transmit(CAN1, &TxMessage1);
        }
        
        break;
        
      case 4:     //(Charging)System Total DC current
        
#if CURRENT_SELF_DISTRIBUTION_MODE == 0
        if(module_rectification)
        {
          //        if(Iout_ref_temp_G1 > 73.3)     Iout_ref_temp_G1 = 73.3;
          if(Iout_ref_temp_G1 > 200)      Iout_ref_temp_G1 = 200;
          
//          module_set_data.set_system_basic_information.system_total_dc_current = (uint32_t)((Iout_ref_temp_G1/MODULE_COUNT) * 1000);
          module_set_data.set_system_basic_information.system_total_dc_current = (uint32_t)(Iout_ref_temp_G1 * 1000);
          
          TxMessage1.ExtId = ALL_MODULE_SET_DATA;
          TxMessage1.DLC = 8;
          TxMessage1.IDE = CAN_ID_EXT;
          TxMessage1.Data[0] = SYSTEM_BASIC_INFO;
          TxMessage1.Data[1] = 0x02;
          TxMessage1.Data[2] = 0x00; 
          TxMessage1.Data[3] = 0x00; 
          TxMessage1.Data[4] = module_set_data.set_system_basic_information.system_total_dc_current >> 24; 
          TxMessage1.Data[5] = module_set_data.set_system_basic_information.system_total_dc_current >> 16; 
          TxMessage1.Data[6] = module_set_data.set_system_basic_information.system_total_dc_current >> 8; 
          TxMessage1.Data[7] = module_set_data.set_system_basic_information.system_total_dc_current;
          CAN_Transmit(CAN1, &TxMessage1);
        }
        
#elif CURRENT_SELF_DISTRIBUTION_MODE == 1
        if(module_rectification)
        {
          if(Iout_ref_temp_G1 > 200)    Iout_ref_temp_G1 = 200;
          
          if(number_of_using_module != 1 && number_of_using_module != 2)    number_of_using_module = 3;
          
          module_set_data.set_system_basic_information.system_total_dc_current = (uint32_t)((Iout_ref_temp_G1/(float)number_of_using_module) * 1000);
          
          TxMessage1.ExtId = ALL_MODULE_SET_DATA;
          TxMessage1.DLC = 8;
          TxMessage1.IDE = CAN_ID_EXT;
          TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
          TxMessage1.Data[1] = 0x02;
          TxMessage1.Data[2] = 0x00; 
          TxMessage1.Data[3] = 0x00; 
          TxMessage1.Data[4] = module_set_data.set_system_basic_information.system_total_dc_current >> 24; 
          TxMessage1.Data[5] = module_set_data.set_system_basic_information.system_total_dc_current >> 16; 
          TxMessage1.Data[6] = module_set_data.set_system_basic_information.system_total_dc_current >> 8; 
          TxMessage1.Data[7] = module_set_data.set_system_basic_information.system_total_dc_current;
          CAN_Transmit(CAN1, &TxMessage1);
        }
#endif
        
        break;
        
      case 5:     //DC Overvoltage Protection Point -> not used in inverter mode
        
        //      if(module_grid_connected_inverter)
        //      {
        //        module_set_data.set_single_pm_basic_information.dc_overvoltage_protection_point = DC_OVERVOLTAGE_PROTECTION_POINT;
        //        
        //        TxMessage1.ExtId = SET_MODULE_1_DATA;
        //        TxMessage1.DLC = 8;
        //        TxMessage1.IDE = CAN_ID_EXT;
        //        TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
        //        TxMessage1.Data[1] = 0x31;
        //        TxMessage1.Data[2] = 0x00; 
        //        TxMessage1.Data[3] = 0x00; 
        //        TxMessage1.Data[4] = module_set_data.set_single_pm_basic_information.dc_overvoltage_protection_point >> 24; 
        //        TxMessage1.Data[5] = module_set_data.set_single_pm_basic_information.dc_overvoltage_protection_point >> 16; 
        //        TxMessage1.Data[6] = module_set_data.set_single_pm_basic_information.dc_overvoltage_protection_point >> 8; 
        //        TxMessage1.Data[7] = module_set_data.set_single_pm_basic_information.dc_overvoltage_protection_point; 
        //        CAN_Transmit(CAN1, &TxMessage1);
        //      }
        
        break;
        
      case 6:     //DC DisCharge cut-off voltage
        
        if(module_grid_connected_inverter)
        {
		  //20260611 PHE 부하쪽에서 전압설정해보기
          //module_set_data.set_single_pm_basic_information.dc_discharge_cutoff_voltage = (uint32_t)DC_DISCHARGING_CUTOFF_VOLTAGE;

		  if(Vout_ref_temp_G1 > 1000)     Vout_ref_temp_G1 = 1000;
          if(Vout_ref_temp_G1 < 150)      Vout_ref_temp_G1 = 150;
          
          module_set_data.set_single_pm_basic_information.dc_discharge_cutoff_voltage = (uint32_t)(Vout_ref_temp_G1 * 1000);
		  
          TxMessage1.ExtId = ALL_MODULE_SET_DATA;
          TxMessage1.DLC = 8;
          TxMessage1.IDE = CAN_ID_EXT;
          TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
          TxMessage1.Data[1] = 0x32;
          TxMessage1.Data[2] = 0x00; 
          TxMessage1.Data[3] = 0x00; 
          TxMessage1.Data[4] = module_set_data.set_single_pm_basic_information.dc_discharge_cutoff_voltage >> 24; 
          TxMessage1.Data[5] = module_set_data.set_single_pm_basic_information.dc_discharge_cutoff_voltage >> 16; 
          TxMessage1.Data[6] = module_set_data.set_single_pm_basic_information.dc_discharge_cutoff_voltage >> 8; 
          TxMessage1.Data[7] = module_set_data.set_single_pm_basic_information.dc_discharge_cutoff_voltage; 
          CAN_Transmit(CAN1, &TxMessage1);
        }
        
        break;
        
      case 7:     //(Discharging)System Total DC current
        
        if(module_grid_connected_inverter)
        {
		  if(dc_side_current_limit <= 200000) 
		  {
			module_set_data.set_system_basic_information.dc_side_current_limit = dc_side_current_limit;
		  }
		  else
		  {
			module_set_data.set_system_basic_information.dc_side_current_limit = 200000;
		  }
		  
          TxMessage1.ExtId = ALL_MODULE_SET_DATA;
          TxMessage1.DLC = 8;
          TxMessage1.IDE = CAN_ID_EXT;
          TxMessage1.Data[0] = SYSTEM_BASIC_INFO;
          TxMessage1.Data[1] = 0x02;
          TxMessage1.Data[2] = 0x00; 
          TxMessage1.Data[3] = 0x00; 
          TxMessage1.Data[4] = module_set_data.set_system_basic_information.dc_side_current_limit >> 24; 
          TxMessage1.Data[5] = module_set_data.set_system_basic_information.dc_side_current_limit >> 16; 
          TxMessage1.Data[6] = module_set_data.set_system_basic_information.dc_side_current_limit >> 8; 
          TxMessage1.Data[7] = module_set_data.set_system_basic_information.dc_side_current_limit;
          CAN_Transmit(CAN1, &TxMessage1);
        }
        
        break;
        
        
      case 8:
        
        if(module_on_off_G1 == MODULE_OFF)
        {        
          GPIOD->BRR  = GPIO_Pin_2;     //모듈 OFF 시점 확인 TEST용 접점
          
          module_set_data.set_single_pm_basic_information.module_on_off = module_off;
          
          TxMessage1.ExtId = ALL_MODULE_SET_DATA;
          TxMessage1.DLC = 8;
          TxMessage1.IDE = CAN_ID_EXT;
          TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
          TxMessage1.Data[1] = 0x10;
          TxMessage1.Data[2] = 0x00; 
          TxMessage1.Data[3] = 0x00; 
          TxMessage1.Data[4] = 0x00; 
          TxMessage1.Data[5] = 0x00; 
          TxMessage1.Data[6] = 0x00; 
          TxMessage1.Data[7] = module_set_data.set_single_pm_basic_information.module_on_off;
          CAN_Transmit(CAN1, &TxMessage1);
        }
        
        if(module_on_off_G1 == MODULE_ON)
        {
          GPIOD->BSRR  = GPIO_Pin_2;    //모듈 ON 시점 확인 TEST용 접점
          
          module_set_data.set_single_pm_basic_information.module_on_off = module_on;
         
          if(number_of_using_module == 1)
          {            
            TxMessage1.ExtId = FIRST_MODULE_SET_DATA;
            TxMessage1.DLC = 8;
            TxMessage1.IDE = CAN_ID_EXT;
            TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
            TxMessage1.Data[1] = 0x10;
            TxMessage1.Data[2] = 0x00; 
            TxMessage1.Data[3] = 0x00; 
            TxMessage1.Data[4] = 0x00; 
            TxMessage1.Data[5] = 0x00; 
            TxMessage1.Data[6] = 0x00; 
            TxMessage1.Data[7] = module_set_data.set_single_pm_basic_information.module_on_off; 
            CAN_Transmit(CAN1, &TxMessage1);
            
            TxMessage1.ExtId = SECOND_MODULE_SET_DATA;
            TxMessage1.DLC = 8;
            TxMessage1.IDE = CAN_ID_EXT;
            TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
            TxMessage1.Data[1] = 0x10;
            TxMessage1.Data[2] = 0x00; 
            TxMessage1.Data[3] = 0x00; 
            TxMessage1.Data[4] = 0x00; 
            TxMessage1.Data[5] = 0x00; 
            TxMessage1.Data[6] = 0x00; 
            TxMessage1.Data[7] = 0xA1; 
            CAN_Transmit(CAN1, &TxMessage1);
            
            TxMessage1.ExtId = THIRD_MODULE_SET_DATA;
            TxMessage1.DLC = 8;
            TxMessage1.IDE = CAN_ID_EXT;
            TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
            TxMessage1.Data[1] = 0x10;
            TxMessage1.Data[2] = 0x00; 
            TxMessage1.Data[3] = 0x00; 
            TxMessage1.Data[4] = 0x00; 
            TxMessage1.Data[5] = 0x00; 
            TxMessage1.Data[6] = 0x00; 
            TxMessage1.Data[7] = 0xA1; 
            CAN_Transmit(CAN1, &TxMessage1);
            
          }
          
          else if(number_of_using_module == 2)
          {
            TxMessage1.ExtId = FIRST_MODULE_SET_DATA;
            TxMessage1.DLC = 8;
            TxMessage1.IDE = CAN_ID_EXT;
            TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
            TxMessage1.Data[1] = 0x10;
            TxMessage1.Data[2] = 0x00; 
            TxMessage1.Data[3] = 0x00; 
            TxMessage1.Data[4] = 0x00; 
            TxMessage1.Data[5] = 0x00; 
            TxMessage1.Data[6] = 0x00; 
            TxMessage1.Data[7] = module_set_data.set_single_pm_basic_information.module_on_off; 
            CAN_Transmit(CAN1, &TxMessage1);
            
            TxMessage1.ExtId = SECOND_MODULE_SET_DATA;
            TxMessage1.DLC = 8;
            TxMessage1.IDE = CAN_ID_EXT;
            TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
            TxMessage1.Data[1] = 0x10;
            TxMessage1.Data[2] = 0x00; 
            TxMessage1.Data[3] = 0x00; 
            TxMessage1.Data[4] = 0x00; 
            TxMessage1.Data[5] = 0x00; 
            TxMessage1.Data[6] = 0x00; 
            TxMessage1.Data[7] = module_set_data.set_single_pm_basic_information.module_on_off; 
            CAN_Transmit(CAN1, &TxMessage1);
            
            TxMessage1.ExtId = THIRD_MODULE_SET_DATA;
            TxMessage1.DLC = 8;
            TxMessage1.IDE = CAN_ID_EXT;
            TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
            TxMessage1.Data[1] = 0x10;
            TxMessage1.Data[2] = 0x00; 
            TxMessage1.Data[3] = 0x00; 
            TxMessage1.Data[4] = 0x00; 
            TxMessage1.Data[5] = 0x00; 
            TxMessage1.Data[6] = 0x00; 
            TxMessage1.Data[7] = 0xA1; 
            CAN_Transmit(CAN1, &TxMessage1);
          }
          
          else
          {
            TxMessage1.ExtId = ALL_MODULE_SET_DATA;
            TxMessage1.DLC = 8;
            TxMessage1.IDE = CAN_ID_EXT;
            TxMessage1.Data[0] = SINGLE_PM_BASIC_INFO;
            TxMessage1.Data[1] = 0x10;
            TxMessage1.Data[2] = 0x00; 
            TxMessage1.Data[3] = 0x00; 
            TxMessage1.Data[4] = 0x00; 
            TxMessage1.Data[5] = 0x00; 
            TxMessage1.Data[6] = 0x00; 
            TxMessage1.Data[7] = module_set_data.set_single_pm_basic_information.module_on_off; 
            CAN_Transmit(CAN1, &TxMessage1);
          }
        }
        break;
      }
}

void MODULE_INFO_TRANS(void)
{
  module_info[0].ufVout.fValue         = module_read_data[1].read_single_pm_basic_information.power_module_dc_side_voltage;
  module_info[0].ufIout.fValue         = module_read_data[1].read_single_pm_basic_information.power_module_dc_side_current;
  module_info[0].Temperature.fValue    = module_read_data[1].read_single_pm_basic_information.power_module_ambient_temperature;

}