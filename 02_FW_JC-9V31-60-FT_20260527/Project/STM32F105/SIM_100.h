#include "stdint.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define  EVSE_ID_SID                    0x0A100101
#define  SID_ID_EVSE                    0x0A100100

#define  Part_name_0                    0x01
#define  Part_name_1                    0x02
#define  Part_name_2                    0x03
#define  Part_name_3                    0x04
#define  Version_0                      0x05
#define  Version_1                      0x06
#define  Version_2                      0x07
#define  Serial_number_0                0x08
#define  Serial_number_1                0x09
#define  Serial_number_2                0x0A
#define  Serial_number_3                0x0B

#define  Turn_excitation_pulse_off      0x62
#define  Restart                        0xC1
#define  Set_max_battery_design_voltage 0xF0

#define  Temperature                    0x80
#define  Isolation_state                0xE0
#define  Isolation_resistances          0xE1
#define  Isolation_capacitances         0xE2
#define  Voltages_Vp_Vn                 0xE3
#define  Battery_voltage_Vb             0xE4
#define  Error_flags                    0xE5

struct status_bits
{
   uint8_t IS1_IS0:2;       
   uint8_t LV:1;     
   uint8_t HV:1;     
   uint8_t Reserved:1;
   uint8_t HU:1;
   uint8_t NE:1;
   uint8_t HE:1;
};

typedef union
{
    uint8_t                    all;
    struct              status_bits;        
}_Status_Bits;

typedef struct
{
  _Status_Bits  Status_Bits;
  uint16_t      Electrical_isolation; // Big Endian
  uint8_t       Electrical_isolation_uncertainty; // Big Endian
  uint16_t      Energy_stored; // Big Endian
  uint8_t       Energy_stored_uncertainty; // Big Endian
}_SIM_Isolation_State;

typedef struct
{
  _Status_Bits  Status_Bits;
  uint16_t      Rp;
  uint8_t       Rp_uncertainty;
  uint16_t      Rn;
  uint8_t       Rn_uncertainty;
}_SIM_Isolation_Resistances;

typedef struct
{
  _Status_Bits  Status_Bits;
  uint16_t      Cp;
  uint8_t       Cp_uncertainty;
  uint16_t      Cn;
  uint8_t       Cn_uncertainty;
}_SIM_Isolation_Capacitances;

typedef struct
{
  _Status_Bits  Status_Bits;
  int      Vp;
  short       Vp_uncertainty;
  int      Vn;
  short       Vn_uncertainty;
}_SIM_Vp_Vn;

typedef struct
{
  _Status_Bits  Status_Bits;
  uint16_t      Vb;
  uint8_t       Vb_uncertainty;
  uint16_t      Vb_max;
  uint8_t       Vb_max_uncertainty;
}_SIM_Battery_Voltage;

struct error_flags
{
   uint8_t Reserved:2;       
   uint8_t Err_Vpwr:1;     
   uint8_t Err_Vexi:1;     
   uint8_t Err_VxR:1;
   uint8_t Err_CH:1;
   uint8_t Err_Vx1:1;
   uint8_t Err_Vx2:1;
};

typedef union
{
    uint8_t                    all;
    struct              error_flags;        
}_Error_Flags;

typedef struct
{
  _Status_Bits  Status_Bits;
  _Error_Flags  Error_Flags;
}_SIM_Error_Flags;

typedef struct
{
  _SIM_Isolation_State            SIM_Isolation_State;
  _SIM_Isolation_Resistances      SIM_Isolation_Resistances;
  _SIM_Isolation_Capacitances     SIM_Isolation_Capacitances;
  _SIM_Vp_Vn                      SIM_Vp_Vn;
  _SIM_Battery_Voltage            SIM_Battery_Voltage;
  _SIM_Error_Flags                SIM_Error_Flags;
  long        SIM_Temperature;
  
}_SIM_info;

#ifdef _SIM_100_C

_SIM_info       SIM_info;

void SIM_100_Receive(uint16_t SIM_100_Rx_step);
void SIM_100_Transmit(void);

#else

extern _SIM_info       SIM_info;

extern void SIM_100_Receive(uint16_t SIM_100_Rx_step);
extern void SIM_100_Transmit(void);

#endif