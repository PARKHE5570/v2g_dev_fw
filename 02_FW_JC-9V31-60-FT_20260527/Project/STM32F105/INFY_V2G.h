#define CHARGER_OUTPUT_MAX_POWER (set_total_power_fix * 1000)
#define POWER_EFFICIENCY         (0.93)

#define READING_THE_PM_INFORMATION              0x23
#define WRITING_THE_PM_SETTING                  0x24

#define SYSTEM_BASIC_INFO                       0x10
#define SINGLE_PM_BASIC_INFO                    0x11
#define SINGLE_PM_AC_SIDE_INFO                  0x21
#define SINGLE_BIDIRECTIONAL_PM_BASIC_INFO      0x41

#define READ_MODULE_1_DATA              0x02A301F0
#define READ_MODULE_2_DATA              0x02A302F0
#define READ_MODULE_3_DATA              0x02A303F0
#define READ_MODULE_4_DATA              0x02A304F0

#define READ_MODULE_ALL_DATA            0x02A33FF0

#define RESPONSE_MODULE_1_DATA          0x02E3F001
#define RESPONSE_MODULE_2_DATA          0x02E3F002
#define RESPONSE_MODULE_3_DATA          0x02E3F003
#define RESPONSE_MODULE_4_DATA          0x02E3F004

#define SET_MODULE_1_DATA               0x02E401F0
#define SET_MODULE_2_DATA               0x02E402F0
#define SET_MODULE_3_DATA               0x02E403F0
#define SET_MODULE_4_DATA               0x02E404F0

#define ALL_MODULE_SET_DATA             0x02A43FF0
#define FIRST_MODULE_SET_DATA           0x02A401F0
#define SECOND_MODULE_SET_DATA          0x02A402F0
#define THIRD_MODULE_SET_DATA           0x02A403F0

#define MODULE_ON                       (1)
#define MODULE_OFF                      (0)

#define DC_DISCHARGING_CUTOFF_VOLTAGE   200000          //200V                  Prohibit 150V setting, unstable because the module voltage minimum is 150V
//#define DC_SIDE_CURRENT_LIMIT           20000           //20A                   current-limit in discharging mode
//#define DC_OVERVOLTAGE_PROTECTION_POINT 1000000         //1000V                Not used in inverter mode

typedef union {
	float fValue;
	unsigned long ulValue;
}UNIONFLOAT;

typedef struct
{
	UNIONFLOAT         ufVout;
	UNIONFLOAT         ufIout;
	UNIONFLOAT         Temperature;
}__chg_module_state;

typedef enum
{
  normal = 0x00,
  command_invalid = 0x02,
  data_invalid = 0x03,
  in_start_proessing = 0x07
}_error_code;

/******************************************************/
struct __module_status_0
{
  uint8_t       output_short_current:1;
  uint8_t       reserved_01:1;
  uint8_t       reserved_02:1;
  uint8_t       reserved_03:1;
  uint8_t       mdl_sleeping:1;
  uint8_t       mdl_discharge_abnormal:1;
  uint8_t       reserved_06:1;
  uint8_t       reserved_07:1;  
};

typedef union
{
  uint8_t                               all;
  struct        __module_status_0       bit;
}_module_status_0;

/******************************************************/
struct __module_status_1
{
  uint8_t       mdl_dc_side_off_status:1;
  uint8_t       mdl_fault_alarm:1;
  uint8_t       mdl_protection_alarm:1;
  uint8_t       fan_fault_alarm:1;
  uint8_t       over_temperature:1;
  uint8_t       output_over_voltage_alarm:1;
  uint8_t       walk_in_enable:1;
  uint8_t       mdl_communication_interrupt_alarm:1;
};

typedef union
{
  uint8_t                               all;
  struct        __module_status_1       bit;
}_module_status_1;

/******************************************************/
struct  __module_status_2
{
  uint8_t       mdl_power_limit_status:1;
  uint8_t       mdl_id_repeat_alarm:1;
  uint8_t       mdl_load_sharing_alarm:1;
  uint8_t       three_phase_input_phase_lost_alarm:1;
  uint8_t       three_phase_input_unbalance_alarm:1;
  uint8_t       input_low_voltage_alarm:1;
  uint8_t       input_over_voltage_protection:1;
  uint8_t       mdl_pfc_side_off:1;
};

typedef union
{
  uint8_t                               all;
  struct        __module_status_2       bit;
}_module_status_2;

/******************************************************/
typedef enum
{
  rectifier_mode = 0,
  invert_mode = 1
}_mode;

struct __inverter_status_0
{
  _mode         mode:1;
  uint8_t       uvvo_in_grid_inverter:1;
  uint8_t       reserved_02:1;
  uint8_t       reserved_03:1;
  uint8_t       reserved_04:1;
  uint8_t       reserved_05:1;
  uint8_t       island:1;
  uint8_t       reserved_06:1;
};

typedef union
{
  uint8_t                               all;
  struct        __inverter_status_0     bit;
}_inverter_status_0;

/******************************************************/
typedef enum
{
  grid_inv = 0,
  off_grid_inverter = 1
}_grid;

struct __inverter_status_2
{
  _grid         grid:1;
  uint8_t       off_grid_ac_overload:1;
  uint8_t       reserved_02:1;
  uint8_t       reserved_03:1;
  uint8_t       estop:1;
  uint8_t       reserved_05:1;
  uint8_t       reserved_06:1;
  uint8_t       reserved_07:1;
};

typedef union
{
  uint8_t                               all;
  struct        __inverter_status_2     bit;
}_inverter_status_2;

/******************************************************/
typedef struct
{
  uint32_t                      system_dc_voltage;                      //mV
  uint32_t                      system_dc_total_current;                //mA
  uint16_t                      mdl_numbers;
}_read_system_basic_information;

typedef struct
{
  uint32_t                      power_module_dc_side_voltage;           //mV
  uint32_t                      power_module_dc_side_current;           //mA
  uint32_t                      ac_ab_line_voltage;                     //mV
  uint32_t                      ac_bc_line_voltage;                     //mV
  uint32_t                      ac_ca_line_voltage;                     //mV
  uint32_t                      power_module_ambient_temperature;       //m��
  _module_status_0              module_status_0;
  _module_status_1              module_status_1;
  _module_status_2              module_status_2;
  _inverter_status_0            inverter_status_0;
  _inverter_status_2            inverter_status_2;
  uint16_t                      module_group_number;
  uint32_t                      dc_max_output_voltage;                  //mV
  uint32_t                      dc_min_output_voltage;                  //mV
  uint32_t                      dc_max_output_current;                  //mA
  uint32_t                      dc_rated_ouput_power;                   //mW
}_read_single_pm_basic_information;

typedef struct
{
  uint32_t      ac_a_phase_voltage;                                     //mV
  uint32_t      ac_b_phase_voltage;                                     //mV
  uint32_t      ac_c_phase_voltage;                                     //mV
  uint32_t      ac_a_phase_current;                                     //mA
  uint32_t      ac_b_phase_current;                                     //mA
  uint32_t      ac_c_phase_current;                                     //mA
  uint32_t      ac_frequency;                                           //mHz
  uint32_t      total_active_power;                                     //mW
  uint32_t      ac_a_phase_power;                                       //mW
  uint32_t      ac_b_phase_power;                                       //mW
  uint32_t      ac_c_phase_power;                                       //mW
  uint32_t      total_reactive_power;                                   //mVA
  uint32_t      ac_a_phase_reactive_power;                              //mVA
  uint32_t      ac_b_phase_reactive_power;                              //mVA
  uint32_t      ac_c_phase_reactive_power;                              //mVA
  uint32_t      total_apparent_power;                                   //mVA
  uint32_t      ac_a_phase_apparent_power;                              //mVA
  uint32_t      ac_b_phase_apparent_power;                              //mVA
  uint32_t      ac_c_phase_apparent_power;                              //mVA
}_read_single_pm_ac_side_information;

typedef struct
{
  uint32_t                      pm_dc_high_voltage_side_voltage;
  uint32_t                      pm_dc_high_voltage_side_current;
}_read_single_bidirectional_pm_basic_information;

/******************************************************/
typedef struct
{
  _read_system_basic_information                        read_system_basic_information;
  _read_single_pm_basic_information                     read_single_pm_basic_information;
  _read_single_pm_ac_side_information                   read_single_pm_ac_side_information;
  _read_single_bidirectional_pm_basic_information       read_single_bidirectional_pm_basic_information;
  
}_module_read_data;

/******************************************************/
/******************************************************/

typedef struct
{
  uint32_t      system_dc_voltage;
  uint32_t      system_total_dc_current;
  
  uint32_t      dc_side_current_limit;                  //인버터 모드인데 위의 system_total_dc_current와 같이써서 헷갈려서 나눔
}_set_system_basic_information;

/******************************************************/

typedef enum
{
  module_on = 0xA0,
  module_off = 0xA1
}_module_on_off;

typedef enum
{
  green_led_blink_normal = 0xA0,
  green_led_blink = 0xA1
}_green_led_blink;

typedef enum
{
  not_sleep = 0xA0,
  sleep = 0xA1
}_sleep;

typedef enum
{
  forbit_walkin = 0xA0,
  enable_walkin = 0xA1
}_walkin_enable;

typedef enum
{
  low_voltage_mode = 0xA0,
  high_voltage_mode = 0xA1,
  module_automatic_control_switching = 0xA2     //default
}_dc_side_output_high_or_low_voltage_mode;

typedef enum
{
  disable_automatic_voltage_stabilization_mode = 0xA0,
  automatic_voltage_stabilization_mode = 0xA1
}_automatic_voltage_stabilization_mode;

typedef struct
{
  uint32_t                                      power_module_dc_side_voltage;
  uint32_t                                      power_moduel_dc_side_current;
  _module_on_off                                module_on_off;
  _green_led_blink                              green_led_blink;
  _sleep                                        sleep;
  _walkin_enable                                walkin_enable;
  _dc_side_output_high_or_low_voltage_mode      dc_side_output_high_or_low_voltage_mode;
  uint32_t                                      dc_discharge_cutoff_voltage;
  uint32_t                                      dc_overvoltage_protection_point;
  _automatic_voltage_stabilization_mode         automatic_voltage_stabilization_mode;
}_set_single_pm_basic_information;

/******************************************************/

typedef enum
{
  offgrid_voltage_set_220v = 0x00035B60
}_offgrid_voltage_set;

typedef enum
{
  offgrid_frequency_set_60hz = 0x0000EA60
}_offgrid_frequency_set;

typedef enum
{
  invalid = 0x00,
  rectification_mode = 0xA0,
  grid_connected_inverter_mode = 0xA1,
  off_grid_inverter_mode = 0xA2
}_working_mode;

typedef enum
{
  reactive_power_output_is_not_set = 0xA0,
  reactive_power_with_pf_set = 0xA1,
  reactive_power_with_reactive_power = 0xA2,
  automatically_adjust_reactive_power_vde4105 = 0xA4,
  automatically_adjust_reactive_power_ul1741 = 0xA5,
  automatically_adjust_reactive_power_ieee1547 = 0xA6  
}_reactive_power_setting_type;

typedef enum
{
    disable_energy_storage_mode = 0xA0,
    enable_energy_storage_mode = 0xA1
}_energy_storage_mode;

typedef enum
{
  dc_regulation_mode = 0xA0,
  ac_regulation_mode = 0xA1
}_power_regulation_mode;

typedef enum
{
  disable_low_voltage_ride_through = 0xA0,
  enable_low_voltage_ride_through = 0xA1
}_low_voltage_ride_through;

typedef enum
{
  disable_high_voltage_ride_through = 0xA0,
  enable_high_voltage_ride_through = 0xA1
}_high_voltage_ride_through;

typedef struct
{
  uint32_t                              power_factor_adjust;
  _offgrid_voltage_set                  offgrid_voltage_set;
  _offgrid_frequency_set                offgrid_frequency_set;
  uint32_t                              set_reactive_power_value;               //mVar
  _working_mode                         working_mode;
  _reactive_power_setting_type          reactive_power_setting_type;
  _energy_storage_mode                  energy_storage_mode;
  uint32_t                              level_1_over_voltage_protection_value;
  uint32_t                              level_1_over_voltage_protection_time;
  uint32_t                              level_2_over_voltage_protection_value;
  uint32_t                              level_2_over_voltage_protection_time;
  uint32_t                              level_1_under_voltage_protection_value;
  uint32_t                              level_1_under_voltage_protection_time;
  uint32_t                              level_2_under_voltage_protection_value;
  uint32_t                              level_2_under_voltage_protection_time;
  uint32_t                              level_1_over_frequence_protection_value;
  uint32_t                              level_1_over_frequence_protection_time;
  uint32_t                              level_2_over_frequence_protection_value;
  uint32_t                              level_2_over_frequence_protection_time;
  uint32_t                              level_1_under_frequence_protection_value;
  uint32_t                              level_1_under_frequence_protection_time;
  uint32_t                              level_2_under_frequence_protection_value;
  uint32_t                              level_2_under_frequence_protection_time;
  _power_regulation_mode                power_regulation_mode;
  uint32_t                              ac_active_power_setting;
  uint32_t                              ac_active_power_fall_gradient;
  uint32_t                              ac_active_power_rise_gradient;
  _low_voltage_ride_through             low_voltage_ride_through;
  _high_voltage_ride_through            high_voltage_ride_through;
}_set_single_pm_ac_basic_information;


/******************************************************/

typedef struct
{
  uint32_t      power_module_dc_high_voltage_side_voltage;
  uint32_t      power_module_dc_high_voltage_side_current;
}_set_single_bidirectional_pm_basic_information;

typedef struct
{
  _set_system_basic_information                         set_system_basic_information;
  _set_single_pm_basic_information                      set_single_pm_basic_information;
  _set_single_pm_ac_basic_information                   set_single_pm_ac_basic_information;
  _set_single_bidirectional_pm_basic_information        set_single_bidirectional_pm_basic_information;
}_module_set_data;

typedef struct
{
  uint32_t      moduleGroupNumber;
  uint32_t      moduleOnAndOff;
}_module_write_data;

#ifdef __INFY_V2G_MODULE

CanRxMsg        RxMessage1;
CanTxMsg        TxMessage1;

UNIONFLOAT ufRefACV, ufRefACI, ufRefACW, ufRefDCV, ufRefDCI, ufRefDCW;
UNIONFLOAT ufRefV_G1, ufRefI_G1,ufRefV_G2, ufRefI_G2, ufFbVout_G1, ufFbIout_G1, ufFbVout_G2, ufFbIout_G2;

int cnt_set_module_50ms, cnt_read_module_50ms;
int cnt_module_can_error;

float discharging_limit_iout;
float Vout_ref_temp_G1 = 0, Vout_ref_temp_G2 = 0;
float Iout_ref_temp_G1 = 0, Iout_ref_temp_G2 = 0;

uint8_t module_on_off, module_on_off_G1, module_on_off_G2;
uint8_t module_can_communication_start, module_can_communication_start_G1, module_can_communication_start_G2;
uint8_t power_module_Communition_error_counter = 0;
uint8_t module_group, module_id;

uint16_t u16_Vout_Module_G1, u16_Vout_Module_G2, u16_Vout_Module;
uint16_t u16_Iout_Module_G1, u16_Iout_Module_G2, u16_Iout_Module;

uint32_t Vout_Module_G1, Iout_Module_G1, Vout_Module_G2, Iout_Module_G2, Vout_Module, Iout_Module;
uint32_t module_internal_temperature;

char Module_Init_flag;
char module_start_button;
char module_stop_button;
char module_rectification, module_grid_connected_inverter, module_off_grid_inverter;
char number_of_using_module = 3;



uint32_t dc_side_current_limit;
uint32_t dc_side_cutoff_voltage;

_module_read_data               module_read_data[4], module_read_total_data;
_module_set_data                module_set_data;
_module_write_data              module_write_data[4];

__chg_module_state      module_info[1];
UNIONFLOAT module_info_ac_line_voltage_avg;
UNIONFLOAT module_info_ac_phase_current_avg;
UNIONFLOAT module_info_total_active_power_avg;

void Module_Tx_Init();
void Module_Dial_Switch();
void Rx_Module(uint32_t can_id);
void Set_Module_Data();
void Read_Module_Data();
void MODULE_INFO_TRANS();

#else

extern CanRxMsg        RxMessage1;
extern CanTxMsg        TxMessage1;

extern UNIONFLOAT ufRefACV, ufRefACI, ufRefACW, ufRefDCV, ufRefDCI, ufRefDCW;
extern UNIONFLOAT ufRefV_G1, ufRefI_G1,ufRefV_G2, ufRefI_G2, ufFbVout_G1, ufFbIout_G1, ufFbVout_G2, ufFbIout_G2;

extern int cnt_set_module_50ms, cnt_read_module_50ms;
extern int cnt_module_can_error;

extern float discharging_limit_iout;
extern float Vout_ref_temp_G1, Vout_ref_temp_G2;
extern float Iout_ref_temp_G1, Iout_ref_temp_G2;

extern uint8_t module_on_off, module_on_off_G1, module_on_off_G2;
extern uint8_t module_can_communication_start, module_can_communication_start_G1, module_can_communication_start_G2;
extern uint8_t power_module_Communition_error_counter;
extern uint8_t module_group, module_id;

extern uint16_t u16_Vout_Module_G1, u16_Vout_Module_G2, u16_Vout_Module;
extern uint16_t u16_Iout_Module_G1, u16_Iout_Module_G2, u16_Iout_Module;

extern uint32_t Vout_Module_G1, Iout_Module_G1, Vout_Module_G2, Iout_Module_G2, Vout_Module, Iout_Module;
extern uint32_t module_internal_temperature;

extern char Module_Init_flag;
extern char module_start_button;
extern char module_stop_button;
extern char module_rectification, module_grid_connected_inverter, module_off_grid_inverter;
extern char number_of_using_module;

extern uint32_t dc_side_current_limit;
extern uint32_t dc_side_cutoff_voltage;

extern _module_read_data        module_read_data[4], module_read_total_data;
extern _module_set_data         module_set_data;
extern _module_write_data       module_write_data[4];

extern __chg_module_state      module_info[1];
//20260611 float value 
extern UNIONFLOAT module_info_ac_line_voltage_avg;
extern UNIONFLOAT module_info_ac_phase_current_avg;
extern UNIONFLOAT module_info_total_active_power_avg;

extern void Module_Tx_Init();
extern void Module_Dial_Switch();
extern void Rx_Module(uint32_t can_id);
extern void Set_Module_Data();
extern void Read_Module_Data();
extern void MODULE_INFO_TRANS();

#endif
