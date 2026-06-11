
#define __UART_C

#include "stdint.h"
#include "stm32f10x.h"
#include "ctype.h"
#include "stm32f10x_it.h"

#include "UART.h"
#include "main.h"
#include "FirmwareUpdate.h"
#include "digital_io.h"
#include "PLC_3_0.h"
#include "sequence.h"
#include "HMI_modbus.h"


/*
The Actual Value =
DATA×10(소수점-3)
예시) 전압 관련
전압 Data : 5005
소수점 Data : 2
실제값 = 500.0V
: 5000 x 10(2-3)

5000e-3는 5000 * 10의 -3승과 같다.
DC_voltage_value * e-(Decimal_Point_Voltage-3)
*/


void Rxd_Acrel_ch1_data(void)
{
  uint8_t  crc_buf_L, crc_buf_H;
  
  crc_buf_L = make8(CRC16 (Acrel_DC_Power_Meter_ch1_Rx_from_0, DM_cRxdMax-2), 0); // 33개의 데이터 crc16 low 를 구하기 위함
  if (Acrel_DC_Power_Meter_ch1_Rx_from_0[DM_cRxdMax-2] == crc_buf_L)
  {
    crc_buf_H = make8(CRC16 (Acrel_DC_Power_Meter_ch1_Rx_from_0, DM_cRxdMax-2), 1); // 33개의 데이터 crc16 high 를 구하기 위함
    if (Acrel_DC_Power_Meter_ch1_Rx_from_0[DM_cRxdMax-1] == crc_buf_H)	// data 정상
    {
      Acrel_ch1_Communication_Err_cnt = 0;
      
      Acrel_Rx_ch1_data.DC_voltage_value                        = ((uint16_t)Acrel_DC_Power_Meter_ch1_Rx_from_0[3]<< 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[4];
      Acrel_Rx_ch1_data.Decimal_Point_Voltage                   = ((uint16_t)Acrel_DC_Power_Meter_ch1_Rx_from_0[5]<< 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[6];
      Acrel_Rx_ch1_data.DC_current_value                        = ((uint16_t)Acrel_DC_Power_Meter_ch1_Rx_from_0[7]<< 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[8];
      Acrel_Rx_ch1_data.Decimal_Point_Current                   = ((uint16_t)Acrel_DC_Power_Meter_ch1_Rx_from_0[9]<< 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[10];
      Acrel_Rx_ch1_data.Broken_wire_detection_indication        = ((uint16_t)Acrel_DC_Power_Meter_ch1_Rx_from_0[11]<< 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[12]; // 1: Broken 0: Not broken
      Acrel_Rx_ch1_data.Temperature_acrel                       = ((uint16_t)Acrel_DC_Power_Meter_ch1_Rx_from_0[13]<< 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[14];  // * 0.1해서 읽어야함
      Acrel_Rx_ch1_data.Reserved6                               = ((uint16_t)Acrel_DC_Power_Meter_ch1_Rx_from_0[15]<< 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[16];
      Acrel_Rx_ch1_data.Reserved7                               = ((uint16_t)Acrel_DC_Power_Meter_ch1_Rx_from_0[17]<< 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[18];
      Acrel_Rx_ch1_data.Power_value                             = ((uint16_t)Acrel_DC_Power_Meter_ch1_Rx_from_0[19]<< 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[20];
      Acrel_Rx_ch1_data.Decimal_Point_Power                     = ((uint16_t)Acrel_DC_Power_Meter_ch1_Rx_from_0[21]<< 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[22];
      Acrel_Rx_ch1_data.Reserved10                              = ((uint16_t)Acrel_DC_Power_Meter_ch1_Rx_from_0[23]<< 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[24];
      Acrel_Rx_ch1_data.Reserved11                              = ((uint16_t)Acrel_DC_Power_Meter_ch1_Rx_from_0[25]<< 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[26];
      
      Acrel_Rx_ch1_data.Total_positive_active_energy            = Acrel_DC_Power_Meter_ch1_Rx_from_0[27]<< 24; // W/h, 나누기 1000해서 읽어야 kW/h가 된다.
      Acrel_Rx_ch1_data.Total_positive_active_energy            += Acrel_DC_Power_Meter_ch1_Rx_from_0[28]<< 16;
      Acrel_Rx_ch1_data.Total_positive_active_energy            += Acrel_DC_Power_Meter_ch1_Rx_from_0[29]<< 8;
      Acrel_Rx_ch1_data.Total_positive_active_energy            += Acrel_DC_Power_Meter_ch1_Rx_from_0[30];
      
      DCMeter_Pow11 = (uint32_t)(Acrel_Rx_ch1_data.Total_positive_active_energy * 0.1); // 소수점 2번째 자리까지만 사용하도록 한전 프로토콜 정의되어 있음.
      
      Acrel_Rx_ch1_data.Total_reverse_active_energy             = Acrel_DC_Power_Meter_ch1_Rx_from_0[31]<< 24;
      Acrel_Rx_ch1_data.Total_reverse_active_energy             += Acrel_DC_Power_Meter_ch1_Rx_from_0[32]<< 16;
      Acrel_Rx_ch1_data.Total_reverse_active_energy             += Acrel_DC_Power_Meter_ch1_Rx_from_0[33]<< 8;
      Acrel_Rx_ch1_data.Total_reverse_active_energy             += Acrel_DC_Power_Meter_ch1_Rx_from_0[34];
      
      Acrel_Rx_ch1_data.Voltage_transformation_ratio            = ((Acrel_DC_Power_Meter_ch1_Rx_from_0[35] << 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[36]);
      Acrel_Rx_ch1_data.Primary_rated_current                   = ((Acrel_DC_Power_Meter_ch1_Rx_from_0[37] << 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[38]);
      Acrel_Rx_ch1_data.Switch_input_and_output_status          = ((Acrel_DC_Power_Meter_ch1_Rx_from_0[39] << 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[40]);
      Acrel_Rx_ch1_data.Alarm_status.all                        = ((Acrel_DC_Power_Meter_ch1_Rx_from_0[41] << 8) + Acrel_DC_Power_Meter_ch1_Rx_from_0[42]);
    }
  }
}

void Rxd_Eastron_data_1()
{
  Eastron_Rx_data_1.phase_1_line_to_neutral_volts.ulValue       = Eastron_AC_Power_Meter_Rx_read_input_registers_1[3] << 24;
  Eastron_Rx_data_1.phase_1_line_to_neutral_volts.ulValue       |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[4] << 16;
  Eastron_Rx_data_1.phase_1_line_to_neutral_volts.ulValue       |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[5] << 8;
  Eastron_Rx_data_1.phase_1_line_to_neutral_volts.ulValue       |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[6];
  
  Eastron_Rx_data_1.phase_2_line_to_neutral_volts.ulValue       = Eastron_AC_Power_Meter_Rx_read_input_registers_1[7] << 24;
  Eastron_Rx_data_1.phase_2_line_to_neutral_volts.ulValue       |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[8] << 16;
  Eastron_Rx_data_1.phase_2_line_to_neutral_volts.ulValue       |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[9] << 8;
  Eastron_Rx_data_1.phase_2_line_to_neutral_volts.ulValue       |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[10];
  
  Eastron_Rx_data_1.phase_3_line_to_neutral_volts.ulValue       = Eastron_AC_Power_Meter_Rx_read_input_registers_1[11] << 24;
  Eastron_Rx_data_1.phase_3_line_to_neutral_volts.ulValue       |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[12] << 16;
  Eastron_Rx_data_1.phase_3_line_to_neutral_volts.ulValue       |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[13] << 8;
  Eastron_Rx_data_1.phase_3_line_to_neutral_volts.ulValue       |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[14];
  
  Eastron_Rx_data_1.phase_1_current.ulValue                     = Eastron_AC_Power_Meter_Rx_read_input_registers_1[15] << 24;
  Eastron_Rx_data_1.phase_1_current.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[16] << 16;
  Eastron_Rx_data_1.phase_1_current.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[17] << 8;
  Eastron_Rx_data_1.phase_1_current.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[18];
  
  Eastron_Rx_data_1.phase_2_current.ulValue                     = Eastron_AC_Power_Meter_Rx_read_input_registers_1[19] << 24;
  Eastron_Rx_data_1.phase_2_current.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[20] << 16;
  Eastron_Rx_data_1.phase_2_current.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[21] << 8;
  Eastron_Rx_data_1.phase_2_current.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[22];
  
  Eastron_Rx_data_1.phase_3_current.ulValue                     = Eastron_AC_Power_Meter_Rx_read_input_registers_1[23] << 24;
  Eastron_Rx_data_1.phase_3_current.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[24] << 16;
  Eastron_Rx_data_1.phase_3_current.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[25] << 8;
  Eastron_Rx_data_1.phase_3_current.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[26];
  
  Eastron_Rx_data_1.phase_1_active_power.ulValue                = Eastron_AC_Power_Meter_Rx_read_input_registers_1[27] << 24;
  Eastron_Rx_data_1.phase_1_active_power.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[28] << 16;
  Eastron_Rx_data_1.phase_1_active_power.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[29] << 8;
  Eastron_Rx_data_1.phase_1_active_power.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[30];
  
  Eastron_Rx_data_1.phase_2_active_power.ulValue                = Eastron_AC_Power_Meter_Rx_read_input_registers_1[31] << 24;
  Eastron_Rx_data_1.phase_2_active_power.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[32] << 16;
  Eastron_Rx_data_1.phase_2_active_power.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[33] << 8;
  Eastron_Rx_data_1.phase_2_active_power.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[34];
  
  Eastron_Rx_data_1.phase_3_active_power.ulValue                = Eastron_AC_Power_Meter_Rx_read_input_registers_1[35] << 24;
  Eastron_Rx_data_1.phase_3_active_power.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[36] << 16;
  Eastron_Rx_data_1.phase_3_active_power.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[37] << 8;
  Eastron_Rx_data_1.phase_3_active_power.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[38];
  
  Eastron_Rx_data_1.phase_1_apparent_power.ulValue              = Eastron_AC_Power_Meter_Rx_read_input_registers_1[39] << 24;
  Eastron_Rx_data_1.phase_1_apparent_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[40] << 16;
  Eastron_Rx_data_1.phase_1_apparent_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[41] << 8;
  Eastron_Rx_data_1.phase_1_apparent_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[42];
  
  Eastron_Rx_data_1.phase_2_apparent_power.ulValue              = Eastron_AC_Power_Meter_Rx_read_input_registers_1[43] << 24;
  Eastron_Rx_data_1.phase_2_apparent_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[44] << 16;
  Eastron_Rx_data_1.phase_2_apparent_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[45] << 8;
  Eastron_Rx_data_1.phase_2_apparent_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[46];
  
  Eastron_Rx_data_1.phase_3_apparent_power.ulValue              = Eastron_AC_Power_Meter_Rx_read_input_registers_1[47] << 24;
  Eastron_Rx_data_1.phase_3_apparent_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[48] << 16;
  Eastron_Rx_data_1.phase_3_apparent_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[49] << 8;
  Eastron_Rx_data_1.phase_3_apparent_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[50];
  
  Eastron_Rx_data_1.phase_1_reactive_power.ulValue              = Eastron_AC_Power_Meter_Rx_read_input_registers_1[51] << 24;
  Eastron_Rx_data_1.phase_1_reactive_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[52] << 16;
  Eastron_Rx_data_1.phase_1_reactive_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[53] << 8;
  Eastron_Rx_data_1.phase_1_reactive_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[54];
  
  Eastron_Rx_data_1.phase_2_reactive_power.ulValue              = Eastron_AC_Power_Meter_Rx_read_input_registers_1[55] << 24;
  Eastron_Rx_data_1.phase_2_reactive_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[56] << 16;
  Eastron_Rx_data_1.phase_2_reactive_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[57] << 8;
  Eastron_Rx_data_1.phase_2_reactive_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[58];
  
  Eastron_Rx_data_1.phase_3_reactive_power.ulValue              = Eastron_AC_Power_Meter_Rx_read_input_registers_1[59] << 24;
  Eastron_Rx_data_1.phase_3_reactive_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[60] << 16;
  Eastron_Rx_data_1.phase_3_reactive_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[61] << 8;
  Eastron_Rx_data_1.phase_3_reactive_power.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[62];
  
  Eastron_Rx_data_1.phase_1_power_factor.ulValue                = Eastron_AC_Power_Meter_Rx_read_input_registers_1[63] << 24;
  Eastron_Rx_data_1.phase_1_power_factor.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[64] << 16;
  Eastron_Rx_data_1.phase_1_power_factor.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[65] << 8;
  Eastron_Rx_data_1.phase_1_power_factor.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[66];
  
  Eastron_Rx_data_1.phase_2_power_factor.ulValue                = Eastron_AC_Power_Meter_Rx_read_input_registers_1[67] << 24;
  Eastron_Rx_data_1.phase_2_power_factor.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[68] << 16;
  Eastron_Rx_data_1.phase_2_power_factor.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[69] << 8;
  Eastron_Rx_data_1.phase_2_power_factor.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[70];
  
  Eastron_Rx_data_1.phase_3_power_factor.ulValue                = Eastron_AC_Power_Meter_Rx_read_input_registers_1[71] << 24;
  Eastron_Rx_data_1.phase_3_power_factor.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[72] << 16;
  Eastron_Rx_data_1.phase_3_power_factor.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[73] << 8;
  Eastron_Rx_data_1.phase_3_power_factor.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[74];
  
  Eastron_Rx_data_1.phase_1_phase_angle.ulValue                 = Eastron_AC_Power_Meter_Rx_read_input_registers_1[75] << 24;
  Eastron_Rx_data_1.phase_1_phase_angle.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[76] << 16;
  Eastron_Rx_data_1.phase_1_phase_angle.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[77] << 8;
  Eastron_Rx_data_1.phase_1_phase_angle.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[78];
  
  Eastron_Rx_data_1.phase_2_phase_angle.ulValue                 = Eastron_AC_Power_Meter_Rx_read_input_registers_1[79] << 24;
  Eastron_Rx_data_1.phase_2_phase_angle.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[80] << 16;
  Eastron_Rx_data_1.phase_2_phase_angle.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[81] << 8;
  Eastron_Rx_data_1.phase_2_phase_angle.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[82];
  
  Eastron_Rx_data_1.phase_3_phase_angle.ulValue                 = Eastron_AC_Power_Meter_Rx_read_input_registers_1[83] << 24;
  Eastron_Rx_data_1.phase_3_phase_angle.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[84] << 16;
  Eastron_Rx_data_1.phase_3_phase_angle.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[85] << 8;
  Eastron_Rx_data_1.phase_3_phase_angle.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[86];
  
  Eastron_Rx_data_1.average_line_to_neutral_volts.ulValue       = Eastron_AC_Power_Meter_Rx_read_input_registers_1[87] << 24;
  Eastron_Rx_data_1.average_line_to_neutral_volts.ulValue       |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[88] << 16;
  Eastron_Rx_data_1.average_line_to_neutral_volts.ulValue       |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[89] << 8;
  Eastron_Rx_data_1.average_line_to_neutral_volts.ulValue       |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[90];
  
  Eastron_Rx_data_1.average_line_current.ulValue                = Eastron_AC_Power_Meter_Rx_read_input_registers_1[95] << 24;
  Eastron_Rx_data_1.average_line_current.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[96] << 16;
  Eastron_Rx_data_1.average_line_current.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[97] << 8;
  Eastron_Rx_data_1.average_line_current.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[98];
  
  Eastron_Rx_data_1.sum_of_line_currents.ulValue                = Eastron_AC_Power_Meter_Rx_read_input_registers_1[99] << 24;
  Eastron_Rx_data_1.sum_of_line_currents.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[100] << 16;
  Eastron_Rx_data_1.sum_of_line_currents.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[101] << 8;
  Eastron_Rx_data_1.sum_of_line_currents.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_1[102];
  
}

void Rxd_Eastron_data_2(){
  
  Eastron_Rx_data_2.total_system_power.ulValue                  = Eastron_AC_Power_Meter_Rx_read_input_registers_2[3] << 24;
  Eastron_Rx_data_2.total_system_power.ulValue                  |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[4] << 16;
  Eastron_Rx_data_2.total_system_power.ulValue                  |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[5] << 8;
  Eastron_Rx_data_2.total_system_power.ulValue                  |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[6];
  
  Eastron_Rx_data_2.total_system_volt_amps.ulValue              = Eastron_AC_Power_Meter_Rx_read_input_registers_2[11] << 24;
  Eastron_Rx_data_2.total_system_volt_amps.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[12] << 16;
  Eastron_Rx_data_2.total_system_volt_amps.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[13] << 8;
  Eastron_Rx_data_2.total_system_volt_amps.ulValue              |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[14];
  
  Eastron_Rx_data_2.total_system_VAr.ulValue                    = Eastron_AC_Power_Meter_Rx_read_input_registers_2[19] << 24;
  Eastron_Rx_data_2.total_system_VAr.ulValue                    |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[20] << 16;
  Eastron_Rx_data_2.total_system_VAr.ulValue                    |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[21] << 8;
  Eastron_Rx_data_2.total_system_VAr.ulValue                    |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[22];
  
  Eastron_Rx_data_2.total_system_power_factor.ulValue           = Eastron_AC_Power_Meter_Rx_read_input_registers_2[23] << 24;
  Eastron_Rx_data_2.total_system_power_factor.ulValue           |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[24] << 16;
  Eastron_Rx_data_2.total_system_power_factor.ulValue           |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[25] << 8;
  Eastron_Rx_data_2.total_system_power_factor.ulValue           |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[26];
  
  Eastron_Rx_data_2.total_system_phase_angle.ulValue            = Eastron_AC_Power_Meter_Rx_read_input_registers_2[31] << 24;
  Eastron_Rx_data_2.total_system_phase_angle.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[32] << 16;
  Eastron_Rx_data_2.total_system_phase_angle.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[33] << 8;
  Eastron_Rx_data_2.total_system_phase_angle.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[34];
  
  Eastron_Rx_data_2.frequency_of_supply_voltages.ulValue        = Eastron_AC_Power_Meter_Rx_read_input_registers_2[39] << 24;
  Eastron_Rx_data_2.frequency_of_supply_voltages.ulValue        |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[40] << 16;
  Eastron_Rx_data_2.frequency_of_supply_voltages.ulValue        |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[41] << 8;
  Eastron_Rx_data_2.frequency_of_supply_voltages.ulValue        |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[42];
  
  Eastron_Rx_data_2.total_import_kWh.ulValue                    = Eastron_AC_Power_Meter_Rx_read_input_registers_2[43] << 24;
  Eastron_Rx_data_2.total_import_kWh.ulValue                    |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[44] << 16;
  Eastron_Rx_data_2.total_import_kWh.ulValue                    |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[45] << 8;
  Eastron_Rx_data_2.total_import_kWh.ulValue                    |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[46];
  
  Eastron_Rx_data_2.total_export_kWh.ulValue                    = Eastron_AC_Power_Meter_Rx_read_input_registers_2[47] << 24;
  Eastron_Rx_data_2.total_export_kWh.ulValue                    |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[48] << 16;
  Eastron_Rx_data_2.total_export_kWh.ulValue                    |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[49] << 8;
  Eastron_Rx_data_2.total_export_kWh.ulValue                    |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[50];
  
  Eastron_Rx_data_2.total_import_kVARh.ulValue                  = Eastron_AC_Power_Meter_Rx_read_input_registers_2[51] << 24;
  Eastron_Rx_data_2.total_import_kVARh.ulValue                  |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[52] << 16;
  Eastron_Rx_data_2.total_import_kVARh.ulValue                  |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[53] << 8;
  Eastron_Rx_data_2.total_import_kVARh.ulValue                  |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[54];
  
  Eastron_Rx_data_2.total_export_kVARh.ulValue                  = Eastron_AC_Power_Meter_Rx_read_input_registers_2[55] << 24;
  Eastron_Rx_data_2.total_export_kVARh.ulValue                  |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[56] << 16;
  Eastron_Rx_data_2.total_export_kVARh.ulValue                  |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[57] << 8;
  Eastron_Rx_data_2.total_export_kVARh.ulValue                  |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[58];
  
  Eastron_Rx_data_2.total_VAh.ulValue                           = Eastron_AC_Power_Meter_Rx_read_input_registers_2[59] << 24;
  Eastron_Rx_data_2.total_VAh.ulValue                           |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[60] << 16;
  Eastron_Rx_data_2.total_VAh.ulValue                           |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[61] << 8;
  Eastron_Rx_data_2.total_VAh.ulValue                           |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[62];
  
  Eastron_Rx_data_2.Ah.ulValue                                  = Eastron_AC_Power_Meter_Rx_read_input_registers_2[63] << 24;
  Eastron_Rx_data_2.Ah.ulValue                                  |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[64] << 16;
  Eastron_Rx_data_2.Ah.ulValue                                  |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[65] << 8;
  Eastron_Rx_data_2.Ah.ulValue                                  |= Eastron_AC_Power_Meter_Rx_read_input_registers_2[66];
}

void Rxd_Eastron_data_3(){
  
  Eastron_Rx_data_3.line1_to_line2_volts.ulValue                = Eastron_AC_Power_Meter_Rx_read_input_registers_3[3] << 24;
  Eastron_Rx_data_3.line1_to_line2_volts.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_3[4] << 16;
  Eastron_Rx_data_3.line1_to_line2_volts.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_3[5] << 8;
  Eastron_Rx_data_3.line1_to_line2_volts.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_3[6];
  
  Eastron_Rx_data_3.line2_to_line3_volts.ulValue                = Eastron_AC_Power_Meter_Rx_read_input_registers_3[7] << 24;
  Eastron_Rx_data_3.line2_to_line3_volts.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_3[8] << 16;
  Eastron_Rx_data_3.line2_to_line3_volts.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_3[9] << 8;
  Eastron_Rx_data_3.line2_to_line3_volts.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_3[10];
  
  Eastron_Rx_data_3.line3_to_line1_volts.ulValue                = Eastron_AC_Power_Meter_Rx_read_input_registers_3[11] << 24;
  Eastron_Rx_data_3.line3_to_line1_volts.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_3[12] << 16;
  Eastron_Rx_data_3.line3_to_line1_volts.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_3[13] << 8;
  Eastron_Rx_data_3.line3_to_line1_volts.ulValue                |= Eastron_AC_Power_Meter_Rx_read_input_registers_3[14];
  
  Eastron_Rx_data_3.average_line_to_line_volts.ulValue          = Eastron_AC_Power_Meter_Rx_read_input_registers_3[15] << 24;
  Eastron_Rx_data_3.average_line_to_line_volts.ulValue          |= Eastron_AC_Power_Meter_Rx_read_input_registers_3[16] << 16;
  Eastron_Rx_data_3.average_line_to_line_volts.ulValue          |= Eastron_AC_Power_Meter_Rx_read_input_registers_3[17] << 8;
  Eastron_Rx_data_3.average_line_to_line_volts.ulValue          |= Eastron_AC_Power_Meter_Rx_read_input_registers_3[18];
}

void Rxd_Eastron_data_4(){
  
  Eastron_Rx_data_4.phase_1_L_N_volts_thd.ulValue               = Eastron_AC_Power_Meter_Rx_read_input_registers_4[3] << 24;
  Eastron_Rx_data_4.phase_1_L_N_volts_thd.ulValue               |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[4] << 16;
  Eastron_Rx_data_4.phase_1_L_N_volts_thd.ulValue               |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[5] << 8;
  Eastron_Rx_data_4.phase_1_L_N_volts_thd.ulValue               |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[6];
  
  Eastron_Rx_data_4.phase_2_L_N_volts_thd.ulValue               = Eastron_AC_Power_Meter_Rx_read_input_registers_4[7] << 24;
  Eastron_Rx_data_4.phase_2_L_N_volts_thd.ulValue               |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[8] << 16;
  Eastron_Rx_data_4.phase_2_L_N_volts_thd.ulValue               |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[9] << 8;
  Eastron_Rx_data_4.phase_2_L_N_volts_thd.ulValue               |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[10];
  
  Eastron_Rx_data_4.phase_3_L_N_volts_thd.ulValue               = Eastron_AC_Power_Meter_Rx_read_input_registers_4[11] << 24;
  Eastron_Rx_data_4.phase_3_L_N_volts_thd.ulValue               |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[12] << 16;
  Eastron_Rx_data_4.phase_3_L_N_volts_thd.ulValue               |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[13] << 8;
  Eastron_Rx_data_4.phase_3_L_N_volts_thd.ulValue               |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[14];
  
  Eastron_Rx_data_4.phase_1_current_thd.ulValue                 = Eastron_AC_Power_Meter_Rx_read_input_registers_4[15] << 24;
  Eastron_Rx_data_4.phase_1_current_thd.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[16] << 16;
  Eastron_Rx_data_4.phase_1_current_thd.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[17] << 8;
  Eastron_Rx_data_4.phase_1_current_thd.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[18];
  
  Eastron_Rx_data_4.phase_2_current_thd.ulValue                 = Eastron_AC_Power_Meter_Rx_read_input_registers_4[19] << 24;
  Eastron_Rx_data_4.phase_2_current_thd.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[20] << 16;
  Eastron_Rx_data_4.phase_2_current_thd.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[21] << 8;
  Eastron_Rx_data_4.phase_2_current_thd.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[22];
  
  Eastron_Rx_data_4.phase_3_current_thd.ulValue                 = Eastron_AC_Power_Meter_Rx_read_input_registers_4[23] << 24;
  Eastron_Rx_data_4.phase_3_current_thd.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[24] << 16;
  Eastron_Rx_data_4.phase_3_current_thd.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[25] << 8;
  Eastron_Rx_data_4.phase_3_current_thd.ulValue                 |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[26];
  
  Eastron_Rx_data_4.average_line_to_neutral_volts_thd.ulValue   = Eastron_AC_Power_Meter_Rx_read_input_registers_4[31] << 24;
  Eastron_Rx_data_4.average_line_to_neutral_volts_thd.ulValue   |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[32] << 16;
  Eastron_Rx_data_4.average_line_to_neutral_volts_thd.ulValue   |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[33] << 8;
  Eastron_Rx_data_4.average_line_to_neutral_volts_thd.ulValue   |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[34];
  
  Eastron_Rx_data_4.average_line_current_thd.ulValue            = Eastron_AC_Power_Meter_Rx_read_input_registers_4[35] << 24;
  Eastron_Rx_data_4.average_line_current_thd.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[36] << 16;
  Eastron_Rx_data_4.average_line_current_thd.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[37] << 8;
  Eastron_Rx_data_4.average_line_current_thd.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[38];
  
  Eastron_Rx_data_4.total_system_power_factor.ulValue           = Eastron_AC_Power_Meter_Rx_read_input_registers_4[43] << 24;
  Eastron_Rx_data_4.total_system_power_factor.ulValue           |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[44] << 16;
  Eastron_Rx_data_4.total_system_power_factor.ulValue           |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[45] << 8;
  Eastron_Rx_data_4.total_system_power_factor.ulValue           |= Eastron_AC_Power_Meter_Rx_read_input_registers_4[46];
}

void Rxd_Eastron_data_5(){
  
  Eastron_Rx_data_5.line1_to_line2_volts_thd.ulValue            = Eastron_AC_Power_Meter_Rx_read_input_registers_5[3] << 24;
  Eastron_Rx_data_5.line1_to_line2_volts_thd.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[4] << 16;
  Eastron_Rx_data_5.line1_to_line2_volts_thd.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[5] << 8;
  Eastron_Rx_data_5.line1_to_line2_volts_thd.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[6];
  
  Eastron_Rx_data_5.line2_to_line3_volts_thd.ulValue            = Eastron_AC_Power_Meter_Rx_read_input_registers_5[7] << 24;
  Eastron_Rx_data_5.line2_to_line3_volts_thd.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[8] << 16;
  Eastron_Rx_data_5.line2_to_line3_volts_thd.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[9] << 8;
  Eastron_Rx_data_5.line2_to_line3_volts_thd.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[10];
  
  Eastron_Rx_data_5.line3_to_line1_volts_thd.ulValue            = Eastron_AC_Power_Meter_Rx_read_input_registers_5[11] << 24;
  Eastron_Rx_data_5.line3_to_line1_volts_thd.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[12] << 16;
  Eastron_Rx_data_5.line3_to_line1_volts_thd.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[13] << 8;
  Eastron_Rx_data_5.line3_to_line1_volts_thd.ulValue            |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[14];
  
  Eastron_Rx_data_5.average_line_to_line_volts_thd.ulValue      = Eastron_AC_Power_Meter_Rx_read_input_registers_5[15] << 24;
  Eastron_Rx_data_5.average_line_to_line_volts_thd.ulValue      |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[16] << 16;
  Eastron_Rx_data_5.average_line_to_line_volts_thd.ulValue      |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[17] << 8;
  Eastron_Rx_data_5.average_line_to_line_volts_thd.ulValue      |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[18];
  
  Eastron_Rx_data_5.total_kWh.ulValue                           = Eastron_AC_Power_Meter_Rx_read_input_registers_5[19] << 24;
  Eastron_Rx_data_5.total_kWh.ulValue                           |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[20] << 16;
  Eastron_Rx_data_5.total_kWh.ulValue                           |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[21] << 8;
  Eastron_Rx_data_5.total_kWh.ulValue                           |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[22];
  
  Eastron_Rx_data_5.total_kVARh.ulValue                         = Eastron_AC_Power_Meter_Rx_read_input_registers_5[23] << 24;
  Eastron_Rx_data_5.total_kVARh.ulValue                         |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[24] << 16;
  Eastron_Rx_data_5.total_kVARh.ulValue                         |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[25] << 8;
  Eastron_Rx_data_5.total_kVARh.ulValue                         |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[26];
  
  Eastron_Rx_data_5.L1_import_kWh.ulValue                       = Eastron_AC_Power_Meter_Rx_read_input_registers_5[27] << 24;
  Eastron_Rx_data_5.L1_import_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[28] << 16;
  Eastron_Rx_data_5.L1_import_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[29] << 8;
  Eastron_Rx_data_5.L1_import_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[30];
  
  Eastron_Rx_data_5.L2_import_kWh.ulValue                       = Eastron_AC_Power_Meter_Rx_read_input_registers_5[31] << 24;
  Eastron_Rx_data_5.L2_import_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[32] << 16;
  Eastron_Rx_data_5.L2_import_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[33] << 8;
  Eastron_Rx_data_5.L2_import_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[34];
  
  Eastron_Rx_data_5.L3_import_kWh.ulValue                       = Eastron_AC_Power_Meter_Rx_read_input_registers_5[35] << 24;
  Eastron_Rx_data_5.L3_import_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[36] << 16;
  Eastron_Rx_data_5.L3_import_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[37] << 8;
  Eastron_Rx_data_5.L3_import_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[38];
  
  Eastron_Rx_data_5.L1_export_kWh.ulValue                       = Eastron_AC_Power_Meter_Rx_read_input_registers_5[39] << 24;
  Eastron_Rx_data_5.L1_export_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[40] << 16;
  Eastron_Rx_data_5.L1_export_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[41] << 8;
  Eastron_Rx_data_5.L1_export_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[42];
  
  Eastron_Rx_data_5.L2_export_kWh.ulValue                       = Eastron_AC_Power_Meter_Rx_read_input_registers_5[43] << 24;
  Eastron_Rx_data_5.L2_export_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[44] << 16;
  Eastron_Rx_data_5.L2_export_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[45] << 8;
  Eastron_Rx_data_5.L2_export_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[46];
  
  Eastron_Rx_data_5.L3_export_kWh.ulValue                       = Eastron_AC_Power_Meter_Rx_read_input_registers_5[47] << 24;
  Eastron_Rx_data_5.L3_export_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[48] << 16;
  Eastron_Rx_data_5.L3_export_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[49] << 8;
  Eastron_Rx_data_5.L3_export_kWh.ulValue                       |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[50];
  
  Eastron_Rx_data_5.L1_total_kWh.ulValue                        = Eastron_AC_Power_Meter_Rx_read_input_registers_5[51] << 24;
  Eastron_Rx_data_5.L1_total_kWh.ulValue                        |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[52] << 16;
  Eastron_Rx_data_5.L1_total_kWh.ulValue                        |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[53] << 8;
  Eastron_Rx_data_5.L1_total_kWh.ulValue                        |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[54];
  
  Eastron_Rx_data_5.L2_total_kWh.ulValue                        = Eastron_AC_Power_Meter_Rx_read_input_registers_5[55] << 24;
  Eastron_Rx_data_5.L2_total_kWh.ulValue                        |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[56] << 16;
  Eastron_Rx_data_5.L2_total_kWh.ulValue                        |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[57] << 8;
  Eastron_Rx_data_5.L2_total_kWh.ulValue                        |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[58];
  
  Eastron_Rx_data_5.L3_total_kWh.ulValue                        = Eastron_AC_Power_Meter_Rx_read_input_registers_5[59] << 24;
  Eastron_Rx_data_5.L3_total_kWh.ulValue                        |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[60] << 16;
  Eastron_Rx_data_5.L3_total_kWh.ulValue                        |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[61] << 8;
  Eastron_Rx_data_5.L3_total_kWh.ulValue                        |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[62];
  
  Eastron_Rx_data_5.L1_import_kVARh.ulValue                     = Eastron_AC_Power_Meter_Rx_read_input_registers_5[63] << 24;
  Eastron_Rx_data_5.L1_import_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[64] << 16;
  Eastron_Rx_data_5.L1_import_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[65] << 8;
  Eastron_Rx_data_5.L1_import_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[66];
  
  Eastron_Rx_data_5.L2_import_kVARh.ulValue                     = Eastron_AC_Power_Meter_Rx_read_input_registers_5[67] << 24;
  Eastron_Rx_data_5.L2_import_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[68] << 16;
  Eastron_Rx_data_5.L2_import_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[69] << 8;
  Eastron_Rx_data_5.L2_import_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[70];
  
  Eastron_Rx_data_5.L3_import_kVARh.ulValue                     = Eastron_AC_Power_Meter_Rx_read_input_registers_5[71] << 24;
  Eastron_Rx_data_5.L3_import_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[72] << 16;
  Eastron_Rx_data_5.L3_import_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[73] << 8;
  Eastron_Rx_data_5.L3_import_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[74];
  
  Eastron_Rx_data_5.L1_export_kVARh.ulValue                     = Eastron_AC_Power_Meter_Rx_read_input_registers_5[75] << 24;
  Eastron_Rx_data_5.L1_export_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[76] << 16;
  Eastron_Rx_data_5.L1_export_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[77] << 8;
  Eastron_Rx_data_5.L1_export_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[78];
  
  Eastron_Rx_data_5.L2_export_kVARh.ulValue                     = Eastron_AC_Power_Meter_Rx_read_input_registers_5[79] << 24;
  Eastron_Rx_data_5.L2_export_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[80] << 16;
  Eastron_Rx_data_5.L2_export_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[81] << 8;
  Eastron_Rx_data_5.L2_export_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[82];
  
  Eastron_Rx_data_5.L3_export_kVARh.ulValue                     = Eastron_AC_Power_Meter_Rx_read_input_registers_5[83] << 24;
  Eastron_Rx_data_5.L3_export_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[84] << 16;
  Eastron_Rx_data_5.L3_export_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[85] << 8;
  Eastron_Rx_data_5.L3_export_kVARh.ulValue                     |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[86];
  
  Eastron_Rx_data_5.L1_total_kVARh.ulValue                      = Eastron_AC_Power_Meter_Rx_read_input_registers_5[87] << 24;
  Eastron_Rx_data_5.L1_total_kVARh.ulValue                      |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[88] << 16;
  Eastron_Rx_data_5.L1_total_kVARh.ulValue                      |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[89] << 8;
  Eastron_Rx_data_5.L1_total_kVARh.ulValue                      |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[90];
  
  Eastron_Rx_data_5.L2_total_kVARh.ulValue                      = Eastron_AC_Power_Meter_Rx_read_input_registers_5[91] << 24;
  Eastron_Rx_data_5.L2_total_kVARh.ulValue                      |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[92] << 16;
  Eastron_Rx_data_5.L2_total_kVARh.ulValue                      |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[93] << 8;
  Eastron_Rx_data_5.L2_total_kVARh.ulValue                      |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[94];
  
  Eastron_Rx_data_5.L3_total_kVARh.ulValue                      = Eastron_AC_Power_Meter_Rx_read_input_registers_5[95] << 24;
  Eastron_Rx_data_5.L3_total_kVARh.ulValue                      |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[96] << 16;
  Eastron_Rx_data_5.L3_total_kVARh.ulValue                      |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[97] << 8;
  Eastron_Rx_data_5.L3_total_kVARh.ulValue                      |= Eastron_AC_Power_Meter_Rx_read_input_registers_5[98];
}


void make_Txd_Acrel_data_from_0(void)
{
  uint16_t crc16;
  
  Request_word_data_length_from_0 = 20;
  
  Acrel_DC_Power_Meter_ch1_Tx_from_0[0] = 0x01; //Address
  Acrel_DC_Power_Meter_ch1_Tx_from_0[1] = 0x03; //Function code
  Acrel_DC_Power_Meter_ch1_Tx_from_0[2] = 0x00; //Data Start High
  Acrel_DC_Power_Meter_ch1_Tx_from_0[3] = 0x00; //Data Start Low
  Acrel_DC_Power_Meter_ch1_Tx_from_0[4] = 0x00; //Number of data read High
  Acrel_DC_Power_Meter_ch1_Tx_from_0[5] = Request_word_data_length_from_0; //Number of data read Low
  
  crc16  = CRC16 (Acrel_DC_Power_Meter_ch1_Tx_from_0, 6);
  Acrel_DC_Power_Meter_ch1_Tx_from_0[6] = crc16;     // CRC16 L
  Acrel_DC_Power_Meter_ch1_Tx_from_0[7] = crc16>> 8; // CRC16 H
  
  DM_cRxdMax = 3 + (Request_word_data_length_from_0 * 2) + 2;
}

void make_read_input_eastron_data_001_049(void)
{
  uint16_t crc16;
  
  Request_word_data_length_read_input_register_1 = 50;
  
  Eastron_AC_Power_Meter_Tx_read_input_registers_1[0] = 0x02; //Address
  Eastron_AC_Power_Meter_Tx_read_input_registers_1[1] = 0x04; //Function code
  Eastron_AC_Power_Meter_Tx_read_input_registers_1[2] = 0x00; //Data Start High
  Eastron_AC_Power_Meter_Tx_read_input_registers_1[3] = 0x00; //Data Start Low
  Eastron_AC_Power_Meter_Tx_read_input_registers_1[4] = 0x00; //Number of data read High
  Eastron_AC_Power_Meter_Tx_read_input_registers_1[5] = Request_word_data_length_read_input_register_1; //Number of data read Low
  
  crc16  = CRC16 (Eastron_AC_Power_Meter_Tx_read_input_registers_1, 6);
  Eastron_AC_Power_Meter_Tx_read_input_registers_1[6] = crc16;     // CRC16 L
  Eastron_AC_Power_Meter_Tx_read_input_registers_1[7] = crc16>> 8; // CRC16 H
  
  DM_cRxdMax = 3 + (Request_word_data_length_read_input_register_1 * 2) + 2;
}

void make_read_input_eastron_data_053_083(void)
{
  uint16_t crc16;
  
  Request_word_data_length_read_input_register_2 = 32;
  
  Eastron_AC_Power_Meter_Tx_read_input_registers_2[0] = 0x02; //Address
  Eastron_AC_Power_Meter_Tx_read_input_registers_2[1] = 0x04; //Function code
  Eastron_AC_Power_Meter_Tx_read_input_registers_2[2] = 0x00; //Data Start High
  Eastron_AC_Power_Meter_Tx_read_input_registers_2[3] = 0x34; //Data Start Low
  Eastron_AC_Power_Meter_Tx_read_input_registers_2[4] = 0x00; //Number of data read High
  Eastron_AC_Power_Meter_Tx_read_input_registers_2[5] = Request_word_data_length_read_input_register_2; //Number of data read Low
  
  crc16  = CRC16 (Eastron_AC_Power_Meter_Tx_read_input_registers_2, 6);
  Eastron_AC_Power_Meter_Tx_read_input_registers_2[6] = crc16;     // CRC16 L
  Eastron_AC_Power_Meter_Tx_read_input_registers_2[7] = crc16>> 8; // CRC16 H
  
  DM_cRxdMax = 3 + (Request_word_data_length_read_input_register_2 * 2) + 2;
}

void make_read_input_eastron_data_201_207(void)
{
  uint16_t crc16;
  
  Request_word_data_length_read_input_register_3 = 8;
  
  Eastron_AC_Power_Meter_Tx_read_input_registers_3[0] = 0x02; //Address
  Eastron_AC_Power_Meter_Tx_read_input_registers_3[1] = 0x04; //Function code
  Eastron_AC_Power_Meter_Tx_read_input_registers_3[2] = 0x00; //Data Start High
  Eastron_AC_Power_Meter_Tx_read_input_registers_3[3] = 0xC8; //Data Start Low
  Eastron_AC_Power_Meter_Tx_read_input_registers_3[4] = 0x00; //Number of data read High
  Eastron_AC_Power_Meter_Tx_read_input_registers_3[5] = Request_word_data_length_read_input_register_3; //Number of data read Low
  
  crc16  = CRC16 (Eastron_AC_Power_Meter_Tx_read_input_registers_3, 6);
  Eastron_AC_Power_Meter_Tx_read_input_registers_3[6] = crc16;     // CRC16 L
  Eastron_AC_Power_Meter_Tx_read_input_registers_3[7] = crc16>> 8; // CRC16 H
  
  DM_cRxdMax = 3 + (Request_word_data_length_read_input_register_3 * 2) + 2;
}

void make_read_input_eastron_data_235_255(void)
{
  uint16_t crc16;
  
  Request_word_data_length_read_input_register_4 = 22;
  
  Eastron_AC_Power_Meter_Tx_read_input_registers_4[0] = 0x02; //Address
  Eastron_AC_Power_Meter_Tx_read_input_registers_4[1] = 0x04; //Function code
  Eastron_AC_Power_Meter_Tx_read_input_registers_4[2] = 0x00; //Data Start High
  Eastron_AC_Power_Meter_Tx_read_input_registers_4[3] = 0xEA; //Data Start Low
  Eastron_AC_Power_Meter_Tx_read_input_registers_4[4] = 0x00; //Number of data read High
  Eastron_AC_Power_Meter_Tx_read_input_registers_4[5] = Request_word_data_length_read_input_register_4; //Number of data read Low
  
  crc16  = CRC16 (Eastron_AC_Power_Meter_Tx_read_input_registers_4, 6);
  Eastron_AC_Power_Meter_Tx_read_input_registers_4[6] = crc16;     // CRC16 L
  Eastron_AC_Power_Meter_Tx_read_input_registers_4[7] = crc16>> 8; // CRC16 H
  
  DM_cRxdMax = 3 + (Request_word_data_length_read_input_register_4 * 2) + 2;
}

void make_read_input_eastron_data_335_381(void)
{
  uint16_t crc16;
  
  Request_word_data_length_read_input_register_5 = 48;
  
  Eastron_AC_Power_Meter_Tx_read_input_registers_5[0] = 0x02; //Address
  Eastron_AC_Power_Meter_Tx_read_input_registers_5[1] = 0x04; //Function code
  Eastron_AC_Power_Meter_Tx_read_input_registers_5[2] = 0x01; //Data Start High
  Eastron_AC_Power_Meter_Tx_read_input_registers_5[3] = 0x4E; //Data Start Low
  Eastron_AC_Power_Meter_Tx_read_input_registers_5[4] = 0x00; //Number of data read High
  Eastron_AC_Power_Meter_Tx_read_input_registers_5[5] = Request_word_data_length_read_input_register_5; //Number of data read Low
  
  crc16  = CRC16 (Eastron_AC_Power_Meter_Tx_read_input_registers_5, 6);
  Eastron_AC_Power_Meter_Tx_read_input_registers_5[6] = crc16;     // CRC16 L
  Eastron_AC_Power_Meter_Tx_read_input_registers_5[7] = crc16>> 8; // CRC16 H
  
  DM_cRxdMax = 3 + (Request_word_data_length_read_input_register_5 * 2) + 2;
}
