
#define __IO_H


#include <stdbool.h>







#ifdef __IO_C

void Count_section_100ms(void);
void Digital_Output_Control(void);
void Digital_Input_Control(void);

void LED_DISPLAY_Service(void);
void Out_Pow_Cal(void);
void GBT_connector_lock_control(void);

uint8_t RY_welding_check(uint32_t RY_in, uint32_t RY_out, uint16_t* chk_cnt);
uint16_t RY_open_check(uint32_t RY_in, uint32_t RY_out, uint16_t* chk_cnt);
uint8_t DM_CS_u8(uint8_t *Data, int n);
uint8_t gbt_connector_lock_flag = 0;
uint16_t check_lock_delay_timer;
uint16_t check_unlock_delay_timer;
uint8_t connector_lock_fault = 0;
uint8_t connector_unlock_fault = 0, unlocking_fault_cnt = 0;
uint8_t Aux_Relay_delay_timer = 0;


struct Digital_BITS
{
   uint32_t I_EMG:1;            // 0      BIT0
   uint32_t I_MC1:1;            // 1      BIT1
   uint32_t I_MC3:1;            // 2      BIT2
   uint32_t I_RY1:1;            // 3      BIT3
   uint32_t I_RY2:1;            // 4      BIT4
   uint32_t I_RY3:1;            // 5      BIT5
   uint32_t I_RY4:1;            // 6      BIT6
   uint32_t I_DOOR1:1;          // 7      BIT7
   uint32_t I_DOOR2:1;          // 8      BIT8
   uint32_t I_DOOR3:1;          // 9      BIT9
   uint32_t I_ELCB_TRIP:1;      // 10     BIT10
   uint32_t I_RCD_TRIP:1;       // 11     BIT11
   uint32_t I_DCGF:1;           // 12     BIT12
   uint32_t I_Sol:1;            // 13     BIT13
   uint32_t I_PILOT:1;          // 14     BIT14
   uint32_t I_Ignition:1;       // 15     BIT15
   uint32_t I_LIMIT1:1;         // 16     BIT16
   uint32_t I_LIMIT2:1;         // 17     BIT17
   uint32_t I_LIMIT3:1;         // 18     BIT18
   uint32_t I_Limit_Door:1;     // 20     BIT20
   uint32_t I_WELDING:1;        // 21     BIT21
   uint32_t I_ELCB:1;           // 22     BIT21
   uint32_t I_A2:1;             // 23     BIT21
   uint32_t I_A1:1;             // 24     BIT21
   uint32_t None:7;             //     BIT25~31
};


struct Digital_Out
{
   uint32_t O_RY1_Chademo_P:1;  // 0      BIT0
   uint32_t O_RY2_Chademo_N:1;  // 1      BIT1
   uint32_t O_RY3_Combo_P:1;    // 2      BIT2
   uint32_t O_RY4_Combo_N:1;    // 3      BIT3
   uint32_t O_RY5_discharge:1;  // 4      BIT4
   uint32_t O_DOOR1_Chademo:1;  // 5      BIT5
   uint32_t O_DOOR2_Combo:1;    // 6      BIT6
   uint32_t O_DOOR3_AC:1;       // 7      BIT7
   uint32_t O_MC1_Main:1;       // 8      BIT8
   uint32_t O_MC2_AC:1;         // 9      BIT9
   uint32_t O_MC3_precharge:1;  // 10     BIT10
   uint32_t O_FAN:1;            // 11     BIT11 //
   uint32_t O_WAKE_RY1:1;       // 12     BIT12
   uint32_t O_WAKE_G_RY2:1;     // 13     BIT13
   uint32_t O_SOL:1;            // 14     BIT14
   uint32_t O_PWM1_EN:1;        // 15     BIT15
   uint32_t O_PWM2_EN:1;        // 16     BIT16
   uint32_t O_SP1:1;            // 17     BIT17
   uint32_t O_BLED:1;           // 18     BIT18
   uint32_t O_GLED:1;           // 19     BIT19
   uint32_t O_RLED:1;           // 20     BIT20
   uint32_t O_ELCB_TRIP:1;      // 21     BIT21
   uint32_t O_WDD_B_R:1;        // 22     BIT22
   uint32_t None:9;             // 23~31    
   //uint32_t O_UIRESET1:1;        // 22     BIT22
   //uint32_t O_UIRESET2:1;        // 23     BIT23
   //uint32_t O_CP_RY1_Combo:1;    // 15     BIT15
   //uint32_t O_CP_RY2_AC:1;       // 16     BIT16
};

typedef union
{
    uint32_t                    all;
    struct      Digital_BITS    bit;        
}DIGITAL_REG;

typedef union
{
    uint32_t                    all;
    struct              Digital_Out;        
}DIGITAL_OUT_CMD;


DIGITAL_REG             Digital_input,Digital_input_buffer1,Digital_input_buffer2,Digital_input_buffer3,Digital_input_buffer4;
DIGITAL_OUT_CMD         Digital_output;

uint16_t lock_test_tmr, unlock_test_tmr;

uint16_t cable_lock_sequence, cable_unlock_sequence;

#else

extern uint16_t cable_lock_sequence, cable_unlock_sequence;

extern uint16_t lock_test_tmr, unlock_test_tmr;

extern void Digital_Input_Control(void);
extern void Count_section_100ms(void);
extern void Digital_Output_Control(void);
extern void GBT_connector_lock_control(void);
extern uint8_t gbt_connector_lock_flag;
extern uint16_t check_lock_delay_timer;
extern uint16_t check_unlock_delay_timer;
extern uint8_t connector_lock_fault;
extern uint8_t connector_unlock_fault, unlocking_fault_cnt;
extern uint8_t Aux_Relay_delay_timer;
extern void LED_DISPLAY_Service(void);

struct Digital_BITS
{
   uint32_t I_EMG:1;            // 0      BIT0
   uint32_t I_MC1:1;            // 1      BIT1
   uint32_t I_MC3:1;            // 2      BIT2
   uint32_t I_RY1:1;            // 3      BIT3
   uint32_t I_RY2:1;            // 4      BIT4
   uint32_t I_RY3:1;            // 5      BIT5
   uint32_t I_RY4:1;            // 6      BIT6
   uint32_t I_DOOR1:1;          // 7      BIT7
   uint32_t I_DOOR2:1;          // 8      BIT8
   uint32_t I_DOOR3:1;          // 9      BIT9
   uint32_t I_ELCB_TRIP:1;      // 10     BIT10
   uint32_t I_RCD_TRIP:1;       // 11     BIT11
   uint32_t I_DCGF:1;           // 12     BIT12
   uint32_t I_Sol:1;            // 13     BIT13
   uint32_t I_PILOT:1;          // 14     BIT14
   uint32_t I_Ignition:1;       // 15     BIT15
   uint32_t I_LIMIT1:1;         // 16     BIT16
   uint32_t I_LIMIT2:1;         // 17     BIT17
   uint32_t I_LIMIT3:1;         // 18     BIT18
   uint32_t I_Limit_Door:1;     // 20     BIT20
   uint32_t I_WELDING:1;        // 21     BIT21
   uint32_t I_ELCB:1;           // 22     BIT21
   uint32_t I_A2:1;             // 23     BIT21
   uint32_t I_A1:1;             // 24     BIT21
   uint32_t None:7;             //     BIT25~31
};

struct Digital_Out
{
   uint32_t O_RY1_Chademo_P:1;  // 0      BIT0
   uint32_t O_RY2_Chademo_N:1;  // 1      BIT1
   uint32_t O_RY3_Combo_P:1;    // 2      BIT2
   uint32_t O_RY4_Combo_N:1;    // 3      BIT3
   uint32_t O_RY5_discharge:1;  // 4      BIT4
   uint32_t O_DOOR1_Chademo:1;  // 5      BIT5
   uint32_t O_DOOR2_Combo:1;    // 6      BIT6
   uint32_t O_DOOR3_AC:1;       // 7      BIT7
   uint32_t O_MC1_Main:1;       // 8      BIT8
   uint32_t O_MC2_AC:1;         // 9      BIT9
   uint32_t O_MC3_precharge:1;  // 10     BIT10
   uint32_t O_FAN:1;            // 11     BIT11 //
   uint32_t O_WAKE_RY1:1;       // 12     BIT12
   uint32_t O_WAKE_G_RY2:1;     // 13     BIT13
   uint32_t O_SOL:1;            // 14     BIT14
   uint32_t O_PWM1_EN:1;        // 15     BIT15
   uint32_t O_PWM2_EN:1;        // 16     BIT16
   uint32_t O_SP1:1;            // 17     BIT17
   uint32_t O_BLED:1;           // 18     BIT18
   uint32_t O_GLED:1;           // 19     BIT19
   uint32_t O_RLED:1;           // 20     BIT20
   uint32_t O_ELCB_TRIP:1;      // 21     BIT21
   uint32_t O_WDD_B_R:1;        // 22     BIT22
   uint32_t None:9;             // 23~31    
   //uint32_t O_UIRESET1:1;        // 22     BIT22
   //uint32_t O_UIRESET2:1;        // 23     BIT23
   //uint32_t O_CP_RY1_Combo:1;    // 15     BIT15
   //uint32_t O_CP_RY2_AC:1;       // 16     BIT16
};


typedef union
{
    uint32_t                    all;
    struct      Digital_BITS    bit;        
}DIGITAL_REG;

typedef union
{
    uint32_t                    all;
    struct              Digital_Out;        
}DIGITAL_OUT_CMD;


extern DIGITAL_REG             Digital_input,Digital_input_buffer1,Digital_input_buffer2,Digital_input_buffer3,Digital_input_buffer4;
extern DIGITAL_OUT_CMD         Digital_output;


extern uint8_t RY_welding_check(uint32_t RY_in, uint32_t RY_out, uint16_t* chk_cnt);
extern uint16_t RY_open_check(uint32_t RY_in, uint32_t RY_out, uint16_t* chk_cnt);
extern uint8_t DM_CS_u8(uint8_t *Data, int n);


#endif