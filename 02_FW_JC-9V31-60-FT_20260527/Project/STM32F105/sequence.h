
#define __SEQUENCE_H

#include "stdint.h"
#include "stm32f10x.h"
//#include "ADC.h"
//#include "HMI_modbus.h"

#define ISOLATION_SET_VOLTAGE  (450)
#define ISOLATION_SET_CURRENT  (5)
#define DEFAULT_SET_VOLTAGE    (200)
#define DEFAULT_SET_CURRENT    (1)
#define MAX_OUTPUT_VOLTAGE	   (1000)
#define MAX_OUTPUT_CURRENT     (200)





typedef enum
{
   _both_standby_mode = 0,
   _other_ch_100kW_mode,
   _100kW_mode,
   _50kW_mode
}_charge_mode_capacity;

typedef enum
{
   default_ = 5,
   _100kW_to_50kW,
   _50kW_to_100kW
}_mode_change;



#ifdef __SEQUENCE_C

void Sequence_control_1CH(void);
void Combo_Load_Test_1CH(void);
void PLC_Reboot_1CH(void);
void Power_Control_Value_1CH(void);
void V2G_inverter_mode_test(); //240813

uint8_t restart_flag_1CH = 0;



uint8_t plc_reset_timer_1CH;
uint8_t plc_reset_step_1CH;
uint16_t ev_ry_fault_timer;

uint16_t   timer_step3_1CH=0;

uint8_t charging_restart_counter = 0; 
uint8_t user_stop = 0;
uint16_t final_set_max_current_1CH;
uint16_t mode_change_timer;
_mode_change mode_change = default_;
_charge_mode_capacity charge_mode_capacity;



#else

extern void Sequence_control_1CH(void);
extern void Combo_Load_Test_1CH(void);
extern void PLC_Reboot_1CH(void);
extern void V2G_inverter_mode_test(); //240813

extern uint8_t restart_flag_1CH;

extern uint8_t plc_reset_timer_1CH;
extern uint8_t plc_reset_step_1CH;
extern uint16_t ev_ry_fault_timer;
extern uint16_t mode_change_timer;
extern _mode_change mode_change;
extern _charge_mode_capacity charge_mode_capacity;


#endif


