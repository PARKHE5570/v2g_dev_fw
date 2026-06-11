// Protocol_Joas_20211126_Ver_3_0.xlsx
#define __UART_H
/* Compiler-independent, fixed size types */


#define MAX_RBUF            (10)

/*****************AC/DC 계량기 데이터 수신 설정*****************/
//#define ACPM_DCPM_READ                //Acrel, Eastron -> DC 계량기 패리티 none
#define ONLY_ACPM_READ                //Eastron
//#define ONLY_DCPM_READ                //Acrel -> DC 계량기 패리티 even
//#define DISABLE_PM_DATA
/*********************************************************/

typedef union {
	float fValue;
	unsigned long ulValue;
}UNIONFLOAT_PM;

typedef enum
{
  acrel_from_0 = 0,
  acrel_from_2000 = 1,
  acrel_from_2140 = 2,
  eastron_read_input_registers_1 = 3,
  eastron_read_input_registers_2 = 4,
  eastron_read_input_registers_3 = 5,
  eastron_read_input_registers_4 = 6,
  eastron_read_input_registers_5 = 7
}_power_meter_tx_data;

struct __Alarm_status
{
  uint16_t DO1_Over_voltage:1;
  uint16_t DO1_Under_voltage:1;
  uint16_t DO1_Over_current:1;
  uint16_t DO1_Under_current:1;
  uint16_t DO1_Over_power:1;
  uint16_t DO1_Under_power:1;
  uint16_t DO1_No1_switch_input:1;
  uint16_t DO1_No2_switch_input:1;
  uint16_t DO2_Over_voltage:1;
  uint16_t DO2_Under_voltage:1;
  uint16_t DO2_Over_current:1;
  uint16_t DO2_Under_current:1;
  uint16_t DO2_Over_power:1;
  uint16_t DO2_Under_power:1;
  uint16_t DO2_No1_switch_input:1;
  uint16_t DO2_No2_switch_input:1;
};

typedef union
{
  uint16_t                      all;
  struct        __Alarm_status  bit;
}_Alarm_status;

typedef struct
{
  uint16_t DC_voltage_value;
  uint16_t Decimal_Point_Voltage;
  int16_t DC_current_value;                     //Displays a negative value when in inverter mode.
  uint16_t Decimal_Point_Current;
  uint16_t Broken_wire_detection_indication; // 1: Broken 0: Not broken
  uint16_t Temperature_acrel;
  uint16_t Reserved6;
  uint16_t Reserved7;
  int16_t Power_value;                          //Displays a negative value when in inverter mode.
  uint16_t Decimal_Point_Power;
  uint16_t Reserved10;
  uint16_t Reserved11;
  uint32_t Total_positive_active_energy;
  uint32_t Total_reverse_active_energy;
  uint16_t Voltage_transformation_ratio;
  uint16_t Primary_rated_current;
  uint16_t Switch_input_and_output_status;
  _Alarm_status Alarm_status;
}_Acrel_Rx_data;

typedef struct
{
  UNIONFLOAT_PM    phase_1_line_to_neutral_volts;       //30001
  UNIONFLOAT_PM    phase_2_line_to_neutral_volts;       //30003
  UNIONFLOAT_PM    phase_3_line_to_neutral_volts;       //30005
  UNIONFLOAT_PM    phase_1_current;                     //30007
  UNIONFLOAT_PM    phase_2_current;                     //30009
  UNIONFLOAT_PM    phase_3_current;                     //30011
  UNIONFLOAT_PM    phase_1_active_power;                //30013
  UNIONFLOAT_PM    phase_2_active_power;                //30015
  UNIONFLOAT_PM    phase_3_active_power;                //30017
  UNIONFLOAT_PM    phase_1_apparent_power;              //30019
  UNIONFLOAT_PM    phase_2_apparent_power;              //30021
  UNIONFLOAT_PM    phase_3_apparent_power;              //30023
  UNIONFLOAT_PM    phase_1_reactive_power;              //30025
  UNIONFLOAT_PM    phase_2_reactive_power;              //30027
  UNIONFLOAT_PM    phase_3_reactive_power;              //30029
  UNIONFLOAT_PM    phase_1_power_factor;                //30031
  UNIONFLOAT_PM    phase_2_power_factor;                //30033
  UNIONFLOAT_PM    phase_3_power_factor;                //30035
  UNIONFLOAT_PM    phase_1_phase_angle;                 //30037
  UNIONFLOAT_PM    phase_2_phase_angle;                 //30039
  UNIONFLOAT_PM    phase_3_phase_angle;                 //30041
  UNIONFLOAT_PM    average_line_to_neutral_volts;       //30043
  UNIONFLOAT_PM    average_line_current;                //30047
  UNIONFLOAT_PM    sum_of_line_currents;                //30049
}_Eastron_Rx_data_1;

typedef struct
{
  UNIONFLOAT_PM         total_system_power;
  UNIONFLOAT_PM         total_system_volt_amps;
  UNIONFLOAT_PM         total_system_VAr;
  UNIONFLOAT_PM         total_system_power_factor;
  UNIONFLOAT_PM         total_system_phase_angle;
  UNIONFLOAT_PM         frequency_of_supply_voltages;
  UNIONFLOAT_PM         total_import_kWh;
  UNIONFLOAT_PM         total_export_kWh;
  UNIONFLOAT_PM         total_import_kVARh;
  UNIONFLOAT_PM         total_export_kVARh;
  UNIONFLOAT_PM         total_VAh;
  UNIONFLOAT_PM         Ah;  
}_Eastron_Rx_data_2;

typedef struct
{
  UNIONFLOAT_PM         line1_to_line2_volts;
  UNIONFLOAT_PM         line2_to_line3_volts;
  UNIONFLOAT_PM         line3_to_line1_volts;
  UNIONFLOAT_PM         average_line_to_line_volts;
}_Eastron_Rx_data_3;

typedef struct
{
  UNIONFLOAT_PM         phase_1_L_N_volts_thd;
  UNIONFLOAT_PM         phase_2_L_N_volts_thd;
  UNIONFLOAT_PM         phase_3_L_N_volts_thd;
  UNIONFLOAT_PM         phase_1_current_thd;
  UNIONFLOAT_PM         phase_2_current_thd;
  UNIONFLOAT_PM         phase_3_current_thd;
  UNIONFLOAT_PM         average_line_to_neutral_volts_thd;
  UNIONFLOAT_PM         average_line_current_thd;
  UNIONFLOAT_PM         total_system_power_factor;
}_Eastron_Rx_data_4;

typedef struct
{
  UNIONFLOAT_PM         line1_to_line2_volts_thd;
  UNIONFLOAT_PM         line2_to_line3_volts_thd;
  UNIONFLOAT_PM         line3_to_line1_volts_thd;
  UNIONFLOAT_PM         average_line_to_line_volts_thd;
  UNIONFLOAT_PM         total_kWh;
  UNIONFLOAT_PM         total_kVARh;
  UNIONFLOAT_PM         L1_import_kWh;
  UNIONFLOAT_PM         L2_import_kWh;
  UNIONFLOAT_PM         L3_import_kWh;
  UNIONFLOAT_PM         L1_export_kWh;
  UNIONFLOAT_PM         L2_export_kWh;
  UNIONFLOAT_PM         L3_export_kWh;
  UNIONFLOAT_PM         L1_total_kWh;
  UNIONFLOAT_PM         L2_total_kWh;
  UNIONFLOAT_PM         L3_total_kWh;
  UNIONFLOAT_PM         L1_import_kVARh;
  UNIONFLOAT_PM         L2_import_kVARh;
  UNIONFLOAT_PM         L3_import_kVARh;
  UNIONFLOAT_PM         L1_export_kVARh;
  UNIONFLOAT_PM         L2_export_kVARh;
  UNIONFLOAT_PM         L3_export_kVARh;
  UNIONFLOAT_PM         L1_total_kVARh;
  UNIONFLOAT_PM         L2_total_kVARh;
  UNIONFLOAT_PM         L3_total_kVARh;
}_Eastron_Rx_data_5;



#ifdef __UART_C

uint8_t tUart1_TX_DATA[MAX_RBUF];
uint8_t rUART1_RECEIVED_DATA[MAX_RBUF];
uint8_t rUART1_RECEIVED_DATA2[MAX_RBUF];
uint32_t DCMeter_Acc_power1 = 0, DCMeter_Pow11 = 0, uart3_rx_isr_cnt = 0;

_Alarm_status           Alarm_status;
_Acrel_Rx_data          Acrel_Rx_ch1_data;
_Eastron_Rx_data_1      Eastron_Rx_data_1;
_Eastron_Rx_data_2      Eastron_Rx_data_2;
_Eastron_Rx_data_3      Eastron_Rx_data_3;
_Eastron_Rx_data_4      Eastron_Rx_data_4;
_Eastron_Rx_data_5      Eastron_Rx_data_5;

_power_meter_tx_data    power_meter_tx_data;

uint8_t Request_word_data_length_read_input_register_1;
uint8_t Request_word_data_length_read_input_register_2;
uint8_t Request_word_data_length_read_input_register_3;
uint8_t Request_word_data_length_read_input_register_4;
uint8_t Request_word_data_length_read_input_register_5;

uint8_t Request_word_data_length_from_0 = 0;

uint8_t rxok_DM = 0;
uint8_t cRxd_DM = 0, DM_cRxdMax = 0;
uint8_t Eastron_AC_Power_Meter_Tx_read_input_registers_1[8] = {0,};
uint8_t Eastron_AC_Power_Meter_Tx_read_input_registers_2[8] = {0,};
uint8_t Eastron_AC_Power_Meter_Tx_read_input_registers_3[8] = {0,};
uint8_t Eastron_AC_Power_Meter_Tx_read_input_registers_4[8] = {0,};
uint8_t Eastron_AC_Power_Meter_Tx_read_input_registers_5[8] = {0,};

uint8_t Acrel_DC_Power_Meter_ch1_Tx_from_0[8] = {0,};

uint8_t Eastron_AC_Power_Meter_Rx_read_input_registers_1[110] = {0,};
uint8_t Eastron_AC_Power_Meter_Rx_read_input_registers_2[70] = {0,};
uint8_t Eastron_AC_Power_Meter_Rx_read_input_registers_3[20] = {0,};
uint8_t Eastron_AC_Power_Meter_Rx_read_input_registers_4[50] = {0,};
uint8_t Eastron_AC_Power_Meter_Rx_read_input_registers_5[110] = {0,};
uint8_t Acrel_DC_Power_Meter_ch1_Rx_from_0[100] = {0,};

uint16_t Acrel_ch1_Communication_Err_cnt = 0;

void make_Txd_Acrel_data_from_0();
void Rxd_Acrel_ch1_data();

void Rxd_Eastron_data_1();
void Rxd_Eastron_data_2();
void Rxd_Eastron_data_3();
void Rxd_Eastron_data_4();
void Rxd_Eastron_data_5();

void make_read_input_eastron_data_001_049();
void make_read_input_eastron_data_053_083();
void make_read_input_eastron_data_201_207();
void make_read_input_eastron_data_235_255();
void make_read_input_eastron_data_335_381();

#else 

extern uint8_t  tUart1_TX_DATA[MAX_RBUF];
extern uint8_t  rUART1_RECEIVED_DATA[MAX_RBUF];
extern uint8_t  rUART1_RECEIVED_DATA2[MAX_RBUF];
extern uint32_t DCMeter_Acc_power1, DCMeter_Pow11, uart3_rx_isr_cnt;

extern _Alarm_status            Alarm_status;
extern _Acrel_Rx_data           Acrel_Rx_ch1_data;
extern _Eastron_Rx_data_1       Eastron_Rx_data_1;
extern _Eastron_Rx_data_2       Eastron_Rx_data_2;
extern _Eastron_Rx_data_3       Eastron_Rx_data_3;
extern _Eastron_Rx_data_4       Eastron_Rx_data_4;
extern _Eastron_Rx_data_5       Eastron_Rx_data_5;

extern _power_meter_tx_data     power_meter_tx_data;

extern uint8_t Request_word_data_length_read_input_register_1;
extern uint8_t Request_word_data_length_read_input_register_2;
extern uint8_t Request_word_data_length_read_input_register_3;
extern uint8_t Request_word_data_length_read_input_register_4;
extern uint8_t Request_word_data_length_read_input_register_5;

extern uint8_t Request_word_data_length_from_0;

extern uint8_t rxok_DM;
extern uint8_t cRxd_DM, DM_cRxdMax;
extern uint8_t Eastron_AC_Power_Meter_Tx_read_input_registers_1[8];
extern uint8_t Eastron_AC_Power_Meter_Tx_read_input_registers_2[8];
extern uint8_t Eastron_AC_Power_Meter_Tx_read_input_registers_3[8];
extern uint8_t Eastron_AC_Power_Meter_Tx_read_input_registers_4[8];
extern uint8_t Eastron_AC_Power_Meter_Tx_read_input_registers_5[8];

extern uint8_t Acrel_DC_Power_Meter_ch1_Tx_from_0[8];

extern uint8_t Eastron_AC_Power_Meter_Rx_read_input_registers_1[110];
extern uint8_t Eastron_AC_Power_Meter_Rx_read_input_registers_2[70];
extern uint8_t Eastron_AC_Power_Meter_Rx_read_input_registers_3[20];
extern uint8_t Eastron_AC_Power_Meter_Rx_read_input_registers_4[50];
extern uint8_t Eastron_AC_Power_Meter_Rx_read_input_registers_5[110];
extern uint8_t Acrel_DC_Power_Meter_ch1_Rx_from_0[100];

extern uint16_t Acrel_ch1_Communication_Err_cnt;

extern void make_Txd_Acrel_data_from_0();
extern void Rxd_Acrel_ch1_data();

extern void Rxd_Eastron_data_1();
extern void Rxd_Eastron_data_2();
extern void Rxd_Eastron_data_3();
extern void Rxd_Eastron_data_4();
extern void Rxd_Eastron_data_5();

extern void make_read_input_eastron_data_001_049();
extern void make_read_input_eastron_data_053_083();
extern void make_read_input_eastron_data_201_207();
extern void make_read_input_eastron_data_235_255();
extern void make_read_input_eastron_data_335_381();

#endif

