#define __DIGITAL_IO_H

#include <stdbool.h>

typedef enum 
{
	STATE_NONFAULT,             /*!< System is in NON-FAULT state. */
	STATE_FAULT                 /*!< System is in FAULT     state. */
}_eFaultState;


typedef enum
{
	STATE_STANDBY,              /*!< System is in STANDBY state. */
	STATE_READY,                /*!< System is in READY   state. */
	STATE_RUNNING,              /*!< System is in RUNNING state. */
	STATE_FINISH,                /*!< System is in FINISH  state. */
	NORMAL_FINISH,              /* */
	FAULT_FINISH				/* */
}_eSysState;


struct Digital_BITS
{
	uint32_t I_EMG:1;             
	uint32_t I_FUSE_8:1;        
	uint32_t I_FUSE_9:1;        
	uint32_t I_DOOR:1;                      
	uint32_t I_SPD:1;             
	uint32_t I_RY1:1;             
	uint32_t I_RY2:1;                  
	uint32_t I_CABLE_1CH:1;             
	uint32_t I_MC1:1;
    uint32_t I_MC2:1;
	uint32_t I_MCCB1:1;
	uint32_t INPUT_RESERVED:21;
};

typedef union
{
	uint32_t                    all;
	struct      Digital_BITS    bit;        
}DIGITAL_REG;


struct Digital_Out
{
	uint32_t O_RY1:1;         
	uint32_t O_RY2:1;         
	uint32_t O_MC1:1;         
	uint32_t O_MC2:1;         
	uint32_t O_MCCB1_TRIP:1;    
	uint32_t O_TP73:1;    
	uint32_t O_U2_EN:1;  
	uint32_t O_U3_EN:1;  
	uint32_t O_RLED_1CH:1;    
	uint32_t O_GLED_1CH:1;    
	uint32_t O_BLED_1CH:1;    
	uint32_t Out_Reserved:21;       
};


typedef union
{
	uint32_t                    all;
	struct              Digital_Out;        
}DIGITAL_OUT_CMD;





#ifdef __DIGITAL_IO_C


DIGITAL_REG             Digital_input;
DIGITAL_OUT_CMD         Digital_output;


void Digital_Output_Control();
void Digital_Input_Control();
void LED_DISPLAY_and_FAN_Service(void);
void LED_DISPLAY_RGBY_TEST (void);

_eFaultState eFaultState_1CH;
_eSysState eSysState_1CH;



uint8_t blink_flag = 1, blink_count = 0;
int16_t fan_off_count;
char rgby_rotate = 'r';

#else


extern DIGITAL_REG             Digital_input;
extern DIGITAL_OUT_CMD         Digital_output;


extern void Digital_Output_Control();
extern void Digital_Input_Control();
extern void LED_DISPLAY_and_FAN_Service(void);
extern void LED_DISPLAY_RGBY_TEST (void);



extern _eFaultState eFaultState_1CH;
extern _eSysState eSysState_1CH;


extern uint8_t blink_flag;
extern int16_t fan_off_count, blink_count;
#endif