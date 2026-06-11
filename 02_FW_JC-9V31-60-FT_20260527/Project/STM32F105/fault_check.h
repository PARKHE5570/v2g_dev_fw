
#define __FAULT_CHECK_H



#include "stdint.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



#define COMMUNICATION_FAULT_CHECK_SET_TIME (100)

typedef enum
{
	_rx_fault = 0,
	_rx_non_fault,
}_communication_rx_fault;


#ifdef __FAULT_CHECK_C

void Fault_Check_1CH(void);
uint8_t RY_welding_check(uint32_t RY_in, uint32_t RY_out, uint16_t* chk_cnt);
uint16_t RY_open_check(uint32_t RY_in, uint32_t RY_out, uint16_t* chk_cnt);



uint16_t Leakage_time = 0;

void communication_fault_check(uint16_t set_time);

uint8_t                 hmi_485_communication_rx_fault_count, imd_can_communication_rx_fault_count, ac_meter_communication_rx_fault_count[10], dc_meter_communication_rx_fault_count;
_communication_rx_fault hmi_485_communication_rx_fault = _rx_non_fault, imd_can_communication_rx_fault = _rx_non_fault, dc_meter_communication_rx_fault = _rx_non_fault, ac_meter_communication_rx_fault = _rx_non_fault;


uint8_t                 plc_can_communication_rx_fault_count_1CH;
_communication_rx_fault plc_can_communication_rx_fault_1CH = _rx_non_fault;

_communication_rx_fault module1_can_communication_rx_fault = _rx_non_fault;
_communication_rx_fault module2_can_communication_rx_fault = _rx_non_fault;
_communication_rx_fault module3_can_communication_rx_fault = _rx_non_fault;
_communication_rx_fault module4_can_communication_rx_fault = _rx_non_fault;
_communication_rx_fault module5_can_communication_rx_fault = _rx_non_fault;
_communication_rx_fault module6_can_communication_rx_fault = _rx_non_fault;
_communication_rx_fault module7_can_communication_rx_fault = _rx_non_fault;
_communication_rx_fault module8_can_communication_rx_fault = _rx_non_fault;

uint8_t                 module1_can_communication_rx_fault_count;
uint8_t                 module2_can_communication_rx_fault_count;
uint8_t                 module3_can_communication_rx_fault_count;
uint8_t                 module4_can_communication_rx_fault_count;
uint8_t                 module5_can_communication_rx_fault_count;
uint8_t                 module6_can_communication_rx_fault_count;
uint8_t                 module7_can_communication_rx_fault_count;
uint8_t                 module8_can_communication_rx_fault_count;

void cp_fault_save (void);

#else

extern void Fault_Check_1CH(void);

extern uint8_t RY_welding_check(uint32_t RY_in, uint32_t RY_out, uint16_t* chk_cnt);
extern uint16_t RY_open_check(uint32_t RY_in, uint32_t RY_out, uint16_t* chk_cnt);

extern void communication_fault_check(uint16_t set_time);

extern uint8_t                 hmi_485_communication_rx_fault_count, imd_can_communication_rx_fault_count, ac_meter_communication_rx_fault_count[10], dc_meter_communication_rx_fault_count;
extern _communication_rx_fault hmi_485_communication_rx_fault, imd_can_communication_rx_fault, dc_meter_communication_rx_fault, ac_meter_communication_rx_fault;

extern uint8_t                 plc_can_communication_rx_fault_count_1CH;
extern _communication_rx_fault plc_can_communication_rx_fault_1CH;


extern _communication_rx_fault module1_can_communication_rx_fault;
extern _communication_rx_fault module2_can_communication_rx_fault;
extern _communication_rx_fault module3_can_communication_rx_fault;
extern _communication_rx_fault module4_can_communication_rx_fault;
extern _communication_rx_fault module5_can_communication_rx_fault;
extern _communication_rx_fault module6_can_communication_rx_fault;
extern _communication_rx_fault module7_can_communication_rx_fault;
extern _communication_rx_fault module8_can_communication_rx_fault;

extern uint8_t                 module1_can_communication_rx_fault_count;
extern uint8_t                 module2_can_communication_rx_fault_count;
extern uint8_t                 module3_can_communication_rx_fault_count;
extern uint8_t                 module4_can_communication_rx_fault_count;
extern uint8_t                 module5_can_communication_rx_fault_count;
extern uint8_t                 module6_can_communication_rx_fault_count;
extern uint8_t                 module7_can_communication_rx_fault_count;
extern uint8_t                 module8_can_communication_rx_fault_count;

extern void cp_fault_save (void);

#endif

