
#define __DIGITAL_IO_C   //  I/O (LED, RELAY, DOOR, EMG, etc)

#include "stm32f10x.h"
#include "main.h"
#include "digital_io.h"
#include "sequence.h"
#include "PLC_3_0.h"
#include "HMI_modbus.h"
#include "pt1000.h"
#include "fault_check.h"
#include "INFY_V2G.h"
#include "Board_Pin_Config.h"


/*******************************************************************************
* Function Name : void Digital_Input_Control()
* Description   : Digital_Input_Control
* Parameters    : None
* Return        : None
*******************************************************************************/

void Digital_Input_Control()
{    
  if(GPIO_ReadInputDataBit(PIN_I_EMG_GPIO_PORT, PIN_I_EMG_GPIO_PIN))        			Digital_input.bit.I_EMG = 1;
  else                                                                                          Digital_input.bit.I_EMG = 0;
  if(GPIO_ReadInputDataBit(PIN_I_DOOR_GPIO_PORT, PIN_I_DOOR_GPIO_PIN))      			Digital_input.bit.I_DOOR = 1;
  else                                                                                          Digital_input.bit.I_DOOR = 0;
  if(GPIO_ReadInputDataBit(PIN_I_SPD_GPIO_PORT, PIN_I_SPD_GPIO_PIN))                            Digital_input.bit.I_SPD = 1;
  else                                                					        Digital_input.bit.I_SPD = 0;
  if(GPIO_ReadInputDataBit(PIN_I_RY1_GPIO_PORT, PIN_I_RY1_GPIO_PIN))        			Digital_input.bit.I_RY1 = 1;
  else                                                						Digital_input.bit.I_RY1 = 0;
  if(GPIO_ReadInputDataBit(PIN_I_RY2_GPIO_PORT, PIN_I_RY2_GPIO_PIN))        			Digital_input.bit.I_RY2 = 1;
  else                                                						Digital_input.bit.I_RY2 = 0;
  if(GPIO_ReadInputDataBit(PIN_I_MCCB1_GPIO_PORT, PIN_I_MCCB1_GPIO_PIN))       			Digital_input.bit.I_MCCB1 = 1; 
  else                                                						Digital_input.bit.I_MCCB1 = 0;  
  if(GPIO_ReadInputDataBit(PIN_I_CABLE_1CH_GPIO_PORT, PIN_I_CABLE_1CH_GPIO_PIN))       	        Digital_input.bit.I_CABLE_1CH = 1;
  else                                                						Digital_input.bit.I_CABLE_1CH = 0; 
  if(GPIO_ReadInputDataBit(PIN_I_FUSE8_GPIO_PORT, PIN_I_FUSE8_GPIO_PIN))      			Digital_input.bit.I_FUSE_8 = 1;
  else                                                						Digital_input.bit.I_FUSE_8 = 0;
  if(GPIO_ReadInputDataBit(PIN_I_FUSE9_GPIO_PORT, PIN_I_FUSE9_GPIO_PIN))       			Digital_input.bit.I_FUSE_9 = 1;
  else                                                						Digital_input.bit.I_FUSE_9 = 0;
  if(GPIO_ReadInputDataBit(PIN_I_MC1_GPIO_PORT, PIN_I_MC1_GPIO_PIN))       			Digital_input.bit.I_MC1 = 1; 
  else                                                						Digital_input.bit.I_MC1 = 0;
  if(GPIO_ReadInputDataBit(PIN_I_MC2_GPIO_PORT, PIN_I_MC2_GPIO_PIN))       			Digital_input.bit.I_MC2 = 1; 
  else                                                						Digital_input.bit.I_MC2 = 0;
}


/*******************************************************************************
* Function Name : void Digital_Output_Control()
* Description   : Digital_Output_Control
* Parameters    : None
* Return        : None
*******************************************************************************/
void Digital_Output_Control()
{
  if(Digital_output.O_RY1)              PIN_O_RY1_GPIO_PORT->BSRR = PIN_O_RY1_GPIO_PIN;
  else                                  PIN_O_RY1_GPIO_PORT->BRR  = PIN_O_RY1_GPIO_PIN; 
  
  if(Digital_output.O_RY2)              PIN_O_RY2_GPIO_PORT->BSRR = PIN_O_RY2_GPIO_PIN;
  else                                  PIN_O_RY2_GPIO_PORT->BRR  = PIN_O_RY2_GPIO_PIN; 
  
  if(Digital_output.O_MC1)              PIN_O_MC1_GPIO_PORT->BSRR = PIN_O_MC1_GPIO_PIN;
  else                                  PIN_O_MC1_GPIO_PORT->BRR  = PIN_O_MC1_GPIO_PIN; 
  
  if(Digital_output.O_MC2)              PIN_O_MC2_GPIO_PORT->BSRR = PIN_O_MC2_GPIO_PIN;
  else                                  PIN_O_MC2_GPIO_PORT->BRR  = PIN_O_MC2_GPIO_PIN;   

  if(Digital_output.O_MCCB1_TRIP)       PIN_O_MCCB1_TRIP_GPIO_PORT->BSRR = PIN_O_MCCB1_TRIP_GPIO_PIN;
  else                                  PIN_O_MCCB1_TRIP_GPIO_PORT->BRR  = PIN_O_MCCB1_TRIP_GPIO_PIN;   
  
  if(Digital_output.O_RLED_1CH)         PIN_O_RLED_1CH_GPIO_PORT->BSRR = PIN_O_RLED_1CH_GPIO_PIN;
  else                                  PIN_O_RLED_1CH_GPIO_PORT->BRR  = PIN_O_RLED_1CH_GPIO_PIN;
  
  if(Digital_output.O_GLED_1CH)         PIN_O_GLED_1CH_GPIO_PORT->BSRR = PIN_O_GLED_1CH_GPIO_PIN;
  else                                  PIN_O_GLED_1CH_GPIO_PORT->BRR  = PIN_O_GLED_1CH_GPIO_PIN;
  
  if(Digital_output.O_BLED_1CH)         PIN_O_BLED_1CH_GPIO_PORT->BSRR = PIN_O_BLED_1CH_GPIO_PIN;
  else                                  PIN_O_BLED_1CH_GPIO_PORT->BRR  = PIN_O_BLED_1CH_GPIO_PIN;
  
  if(Digital_output.O_TP73)         	PIN_O_TP73_GPIO_PORT->BSRR = PIN_O_TP73_GPIO_PIN;
  else                                  PIN_O_TP73_GPIO_PORT->BRR  = PIN_O_TP73_GPIO_PIN;
  
  if(Digital_output.O_U2_EN)         	PIN_O_U2_EN_GPIO_PORT->BSRR = PIN_O_U2_EN_GPIO_PIN;
  else                                  PIN_O_U2_EN_GPIO_PORT->BRR  = PIN_O_U2_EN_GPIO_PIN;
  
  if(Digital_output.O_U3_EN)         	PIN_O_U3_EN_GPIO_PORT->BSRR = PIN_O_U3_EN_GPIO_PIN;
  else                                  PIN_O_U3_EN_GPIO_PORT->BRR  = PIN_O_U3_EN_GPIO_PIN;
  
  if(Digital_output.O_BLED_1CH)         PIN_O_BLED_1CH_GPIO_PORT->BSRR = PIN_O_BLED_1CH_GPIO_PIN;
  else                                  PIN_O_BLED_1CH_GPIO_PORT->BRR  = PIN_O_BLED_1CH_GPIO_PIN;  
}   

void LED_DISPLAY_and_FAN_Service(void)
{
  if(blink_count++ >= 5) {blink_count = 0; blink_flag ^= 1;}
  
  if (Channel1_RUN == 1) {
	eSysState_1CH = STATE_RUNNING;
	fan_off_count = 3000;
	Digital_output.O_MC2 = 1;
  }
  
  if(Channel1_RUN == 0)
  {
    if(fan_off_count-- <= 0) 
    {
      fan_off_count = 0;
	  Digital_output.O_MC2 = 0;
    }
  }
  
  if(eFaultState_1CH == STATE_FAULT)
  {
    if(blink_flag)
    {
      Digital_output.O_RLED_1CH = 1;
      Digital_output.O_GLED_1CH = 1;
      Digital_output.O_BLED_1CH = 0;
    }
    else
    {
      Digital_output.O_RLED_1CH = 0;
      Digital_output.O_GLED_1CH = 0;
      Digital_output.O_BLED_1CH = 0;
    }
  }
  else
  {
    if (eSysState_1CH == STATE_RUNNING)
    {
      Digital_output.O_RLED_1CH = 1;
      Digital_output.O_GLED_1CH = 0;
      Digital_output.O_BLED_1CH = 0;
    }
    else if (eSysState_1CH == STATE_FINISH)
    {
      Digital_output.O_RLED_1CH = 0;
      Digital_output.O_GLED_1CH = 1;
      Digital_output.O_BLED_1CH = 0;
    }
    else if (eSysState_1CH == STATE_STANDBY)
    {
      Digital_output.O_RLED_1CH = 0;
      Digital_output.O_GLED_1CH = 0;
      Digital_output.O_BLED_1CH = 1;
    }
    else
    {
      Digital_output.O_RLED_1CH = 1;
      Digital_output.O_GLED_1CH = 1;
      Digital_output.O_BLED_1CH = 1;
    }
  }
}

void LED_DISPLAY_RGBY_TEST (void)
{
  switch(rgby_rotate)
  {
  case 'r' :
  
    Digital_output.O_RLED_1CH = 1;
    Digital_output.O_GLED_1CH = 0;
    Digital_output.O_BLED_1CH = 0;
    rgby_rotate = 'g';
    break;
	
  case 'g' :
  
    Digital_output.O_RLED_1CH = 0;
    Digital_output.O_GLED_1CH = 1;
    Digital_output.O_BLED_1CH = 0;
    
    rgby_rotate = 'b';
    break;
	
  case 'b' :
  
    Digital_output.O_RLED_1CH = 0;
    Digital_output.O_GLED_1CH = 0;
    Digital_output.O_BLED_1CH = 1;

    rgby_rotate = 'y';
    break;
	
  case 'y' :

    Digital_output.O_RLED_1CH = 1;
    Digital_output.O_GLED_1CH = 1;
    Digital_output.O_BLED_1CH = 0;
    
    rgby_rotate = 'r';
    break;			
  }
}
