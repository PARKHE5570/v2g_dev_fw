

#define __IO_C

#include "stm32f10x.h"
#include "main.h"
#include "io.h"
#include "UART.h"
#include "sequence.h"

extern DIGITAL_REG             Digital_input,Digital_input_buffer1,Digital_input_buffer2,Digital_input_buffer3,Digital_input_buffer4;
extern DIGITAL_OUT_CMD         Digital_output;


void CABLE_LOCK (void);
void CABLE_UNLOCK (void);
void CABLE_NORMAL (void);



  /*******************************************************************************
* Function Name : void Digital_Input_Control()
* Description   : Digital_Input_Control
* Parameters    : None
* Return        : None
*******************************************************************************/
void Digital_Input_Control(void)
{
    Digital_input_buffer3.all = Digital_input_buffer2.all;
    Digital_input_buffer2.all = Digital_input_buffer1.all;

    Digital_input_buffer1.bit.I_EMG             = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_9);
    Digital_input_buffer1.bit.I_MC1             = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4);
    Digital_input_buffer1.bit.I_MC3             = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2);
    Digital_input_buffer1.bit.I_RY1             = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11);
    Digital_input_buffer1.bit.I_RY2             = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12); 
    Digital_input_buffer1.bit.I_RY3             = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13);   
    Digital_input_buffer1.bit.I_RY4             = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14);
    Digital_input_buffer1.bit.I_DOOR1           = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
    Digital_input_buffer1.bit.I_DOOR2           = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
    Digital_input_buffer1.bit.I_DOOR3           = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8);
    Digital_input_buffer1.bit.I_ELCB_TRIP       = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6);
    Digital_input_buffer1.bit.I_RCD_TRIP        = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);
    Digital_input_buffer1.bit.I_DCGF            = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3);
    Digital_input_buffer1.bit.I_Sol             = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7);
    Digital_input_buffer1.bit.I_PILOT           = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8);
    Digital_input_buffer1.bit.I_Ignition        = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9);
    Digital_input_buffer1.bit.I_LIMIT1          = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5);
    Digital_input_buffer1.bit.I_LIMIT2          = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6);
    Digital_input_buffer1.bit.I_LIMIT3          = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3);
    Digital_input_buffer1.bit.I_Limit_Door      = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1);
    Digital_input_buffer1.bit.I_WELDING         = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7);
    Digital_input_buffer1.bit.I_ELCB            = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0);
    Digital_input_buffer1.bit.I_A2              = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0);
    Digital_input_buffer1.bit.I_A1              = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1);


//    charger_mode = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2); // charger_mode 1:트랜스, 0:노멀
    
    if( (Digital_input_buffer1.bit.I_Sol == Digital_input_buffer2.bit.I_Sol) && (Digital_input_buffer2.bit.I_Sol == Digital_input_buffer3.bit.I_Sol) )
         Digital_input_buffer4.bit.I_Sol  = Digital_input_buffer3.bit.I_Sol;
    if( (Digital_input_buffer1.bit.I_PILOT == Digital_input_buffer2.bit.I_PILOT) && (Digital_input_buffer2.bit.I_PILOT == Digital_input_buffer3.bit.I_PILOT) )
         Digital_input_buffer4.bit.I_PILOT  = Digital_input_buffer3.bit.I_PILOT;
    if( (Digital_input_buffer1.bit.I_Ignition == Digital_input_buffer2.bit.I_Ignition) && (Digital_input_buffer2.bit.I_Ignition == Digital_input_buffer3.bit.I_Ignition) )
         Digital_input_buffer4.bit.I_Ignition  = Digital_input_buffer3.bit.I_Ignition;
    if( (Digital_input_buffer1.bit.I_RY1 == Digital_input_buffer2.bit.I_RY1) && (Digital_input_buffer2.bit.I_RY1 == Digital_input_buffer3.bit.I_RY1) )
         Digital_input_buffer4.bit.I_RY1  = Digital_input_buffer3.bit.I_RY1;
    if( (Digital_input_buffer1.bit.I_RY2 == Digital_input_buffer2.bit.I_RY2) && (Digital_input_buffer2.bit.I_RY2 == Digital_input_buffer3.bit.I_RY2) )
         Digital_input_buffer4.bit.I_RY2  = Digital_input_buffer3.bit.I_RY2;
    if( (Digital_input_buffer1.bit.I_RY3 == Digital_input_buffer2.bit.I_RY3) && (Digital_input_buffer2.bit.I_RY3 == Digital_input_buffer3.bit.I_RY3) )
         Digital_input_buffer4.bit.I_RY3  = Digital_input_buffer3.bit.I_RY3;
    if( (Digital_input_buffer1.bit.I_RY4 == Digital_input_buffer2.bit.I_RY4) && (Digital_input_buffer2.bit.I_RY4 == Digital_input_buffer3.bit.I_RY4) )
         Digital_input_buffer4.bit.I_RY4  = Digital_input_buffer3.bit.I_RY4;
    if( (Digital_input_buffer1.bit.I_DOOR1 == Digital_input_buffer2.bit.I_DOOR1) && (Digital_input_buffer2.bit.I_DOOR1 == Digital_input_buffer3.bit.I_DOOR1) )
         Digital_input_buffer4.bit.I_DOOR1 =  Digital_input_buffer3.bit.I_DOOR1;
    if( (Digital_input_buffer1.bit.I_DOOR2 == Digital_input_buffer2.bit.I_DOOR2) && (Digital_input_buffer2.bit.I_DOOR2 == Digital_input_buffer3.bit.I_DOOR2) )
         Digital_input_buffer4.bit.I_DOOR2 =  Digital_input_buffer3.bit.I_DOOR2;
    if( (Digital_input_buffer1.bit.I_DOOR3 == Digital_input_buffer2.bit.I_DOOR3) && (Digital_input_buffer2.bit.I_DOOR3 == Digital_input_buffer3.bit.I_DOOR3) )
         Digital_input_buffer4.bit.I_DOOR3 =  Digital_input_buffer3.bit.I_DOOR3;
    if( (Digital_input_buffer1.bit.I_EMG == Digital_input_buffer2.bit.I_EMG) && (Digital_input_buffer2.bit.I_EMG == Digital_input_buffer3.bit.I_EMG) )
         Digital_input_buffer4.bit.I_EMG  = Digital_input_buffer3.bit.I_EMG;
    if( (Digital_input_buffer1.bit.I_Limit_Door == Digital_input_buffer2.bit.I_Limit_Door) && (Digital_input_buffer2.bit.I_Limit_Door == Digital_input_buffer3.bit.I_Limit_Door) )
         Digital_input_buffer4.bit.I_Limit_Door  = Digital_input_buffer3.bit.I_Limit_Door;
    if( (Digital_input_buffer1.bit.I_MC3 == Digital_input_buffer2.bit.I_MC3) && (Digital_input_buffer2.bit.I_MC3 == Digital_input_buffer3.bit.I_MC3) )
         Digital_input_buffer4.bit.I_MC3  = Digital_input_buffer3.bit.I_MC3;
    if( (Digital_input_buffer1.bit.I_DCGF == Digital_input_buffer2.bit.I_DCGF) && (Digital_input_buffer2.bit.I_DCGF == Digital_input_buffer3.bit.I_DCGF) )
         Digital_input_buffer4.bit.I_DCGF =  Digital_input_buffer3.bit.I_DCGF;
    if( (Digital_input_buffer1.bit.I_MC1 == Digital_input_buffer2.bit.I_MC1) && (Digital_input_buffer2.bit.I_MC1 == Digital_input_buffer3.bit.I_MC1) )
         Digital_input_buffer4.bit.I_MC1  = Digital_input_buffer3.bit.I_MC1;
    if( (Digital_input_buffer1.bit.I_LIMIT1 == Digital_input_buffer2.bit.I_LIMIT1) && (Digital_input_buffer2.bit.I_LIMIT1 == Digital_input_buffer3.bit.I_LIMIT1) )
         Digital_input_buffer4.bit.I_LIMIT1  = Digital_input_buffer3.bit.I_LIMIT1;
    if( (Digital_input_buffer1.bit.I_LIMIT2 == Digital_input_buffer2.bit.I_LIMIT2) && (Digital_input_buffer2.bit.I_LIMIT2 == Digital_input_buffer3.bit.I_LIMIT2) )
         Digital_input_buffer4.bit.I_LIMIT2  = Digital_input_buffer3.bit.I_LIMIT2;
    if( (Digital_input_buffer1.bit.I_LIMIT3 == Digital_input_buffer2.bit.I_LIMIT3) && (Digital_input_buffer2.bit.I_LIMIT3 == Digital_input_buffer3.bit.I_LIMIT3) )
         Digital_input_buffer4.bit.I_LIMIT3  = Digital_input_buffer3.bit.I_LIMIT3;
    if( (Digital_input_buffer1.bit.I_WELDING == Digital_input_buffer2.bit.I_WELDING) && (Digital_input_buffer2.bit.I_WELDING == Digital_input_buffer3.bit.I_WELDING) )
         Digital_input_buffer4.bit.I_WELDING  = Digital_input_buffer3.bit.I_WELDING;
    if( (Digital_input_buffer1.bit.I_RCD_TRIP == Digital_input_buffer2.bit.I_RCD_TRIP) && (Digital_input_buffer2.bit.I_RCD_TRIP == Digital_input_buffer3.bit.I_RCD_TRIP) )
         Digital_input_buffer4.bit.I_RCD_TRIP  = Digital_input_buffer3.bit.I_RCD_TRIP;
    if( (Digital_input_buffer1.bit.I_ELCB_TRIP == Digital_input_buffer2.bit.I_ELCB_TRIP) && (Digital_input_buffer2.bit.I_ELCB_TRIP == Digital_input_buffer3.bit.I_ELCB_TRIP) )
         Digital_input_buffer4.bit.I_ELCB_TRIP  = Digital_input_buffer3.bit.I_ELCB_TRIP;
    if( (Digital_input_buffer1.bit.I_ELCB == Digital_input_buffer2.bit.I_ELCB) && (Digital_input_buffer2.bit.I_ELCB == Digital_input_buffer3.bit.I_ELCB) )
         Digital_input_buffer4.bit.I_ELCB  = Digital_input_buffer3.bit.I_ELCB;
    if( (Digital_input_buffer1.bit.I_A2 == Digital_input_buffer2.bit.I_A2) && (Digital_input_buffer2.bit.I_A2 == Digital_input_buffer3.bit.I_A2) )
         Digital_input_buffer4.bit.I_A2  = Digital_input_buffer3.bit.I_A2;
    if( (Digital_input_buffer1.bit.I_A1 == Digital_input_buffer2.bit.I_A1) && (Digital_input_buffer2.bit.I_A1 == Digital_input_buffer3.bit.I_A1) )
         Digital_input_buffer4.bit.I_A1  = Digital_input_buffer3.bit.I_A1;
    

    if(!Digital_input_buffer4.bit.I_Sol)        Digital_input.bit.I_Sol = 1;
    else                                        Digital_input.bit.I_Sol = 0;
    if(!Digital_input_buffer4.bit.I_PILOT)      Digital_input.bit.I_PILOT = 1;
    else                                        Digital_input.bit.I_PILOT = 0;
    if(!Digital_input_buffer4.bit.I_Ignition)   Digital_input.bit.I_Ignition = 1;
    else                                        Digital_input.bit.I_Ignition = 0;
    if(Digital_input_buffer4.bit.I_RY1)         Digital_input.bit.I_RY1 = 1;
    else                                        Digital_input.bit.I_RY1 = 0;
    if(Digital_input_buffer4.bit.I_RY2)         Digital_input.bit.I_RY2 = 1;
    else                                        Digital_input.bit.I_RY2 = 0;
    if(Digital_input_buffer4.bit.I_RY3)         Digital_input.bit.I_RY3 = 1;
    else                                        Digital_input.bit.I_RY3 = 0;
    if(Digital_input_buffer4.bit.I_RY4)         Digital_input.bit.I_RY4 = 1; 
    else                                        Digital_input.bit.I_RY4 = 0;
    if(Digital_input_buffer4.bit.I_DOOR1)       Digital_input.bit.I_DOOR1 = 1;
    else                                        Digital_input.bit.I_DOOR1 = 0;
    if(Digital_input_buffer4.bit.I_DOOR2)       Digital_input.bit.I_DOOR2 = 1;
    else                                        Digital_input.bit.I_DOOR2 = 0;
    if(Digital_input_buffer4.bit.I_DOOR3)       Digital_input.bit.I_DOOR3 = 1;
    else                                        Digital_input.bit.I_DOOR3 = 0;
    if(!Digital_input_buffer4.bit.I_EMG)         Digital_input.bit.I_EMG = 1;
    else                                        Digital_input.bit.I_EMG = 0;
    if(Digital_input_buffer4.bit.I_Limit_Door)  Digital_input.bit.I_Limit_Door = 1;
    else                                        Digital_input.bit.I_Limit_Door = 0;
    if(Digital_input_buffer4.bit.I_MC3)         Digital_input.bit.I_MC3 = 1;
    else                                        Digital_input.bit.I_MC3 = 0;
    if(Digital_input_buffer4.bit.I_DCGF)        Digital_input.bit.I_DCGF = 1; 
    else                                        Digital_input.bit.I_DCGF = 0;
    if(Digital_input_buffer4.bit.I_MC1)         Digital_input.bit.I_MC1 = 1;
    else                                        Digital_input.bit.I_MC1 = 0;
    if(Digital_input_buffer4.bit.I_LIMIT1)      Digital_input.bit.I_LIMIT1 = 1;
    else                                        Digital_input.bit.I_LIMIT1 = 0;
    if(Digital_input_buffer4.bit.I_LIMIT2)      Digital_input.bit.I_LIMIT2 = 1;
    else                                        Digital_input.bit.I_LIMIT2 = 0;
    if(Digital_input_buffer4.bit.I_LIMIT3)      Digital_input.bit.I_LIMIT3 = 1;
    else                                        Digital_input.bit.I_LIMIT3 = 0;
    if(!Digital_input_buffer4.bit.I_WELDING)    Digital_input.bit.I_WELDING = 1;
    else                                        Digital_input.bit.I_WELDING = 0;
    if(!Digital_input_buffer4.bit.I_RCD_TRIP)   Digital_input.bit.I_RCD_TRIP = 1;
    else                                        Digital_input.bit.I_RCD_TRIP = 0;
    if(!Digital_input_buffer4.bit.I_ELCB_TRIP)   Digital_input.bit.I_ELCB_TRIP = 1; 
    else                                        Digital_input.bit.I_ELCB_TRIP = 0;
    if(Digital_input_buffer4.bit.I_ELCB)        Digital_input.bit.I_ELCB = 1; 
    else                                        Digital_input.bit.I_ELCB = 0;
    if(!Digital_input_buffer4.bit.I_A2)         Digital_input.bit.I_A2 = 1; 
    else                                        Digital_input.bit.I_A2 = 0;
    if(!Digital_input_buffer4.bit.I_A1)         Digital_input.bit.I_A1 = 1; 
    else                                        Digital_input.bit.I_A1 = 0;
    

    
}       


/*******************************************************************************
* Function Name : void Digital_Output_Control()
* Description   : Digital_Output_Control
* Parameters    : None
* Return        : None
*******************************************************************************/
void Digital_Output_Control(void)
{
  
  if(p_6V_LED == 1)                     GPIOC->BSRR = GPIO_Pin_13;      // 
  else                                  GPIOC->BRR  = GPIO_Pin_13;
   
  if(p_9V_LED == 1)                     GPIOC->BSRR = GPIO_Pin_14;      // 
  else                                  GPIOC->BRR  = GPIO_Pin_14;
  
  if(p_12V_LED == 1)                    GPIOC->BSRR = GPIO_Pin_15;      // 
  else                                  GPIOC->BRR  = GPIO_Pin_15;
    
  //if(Fault_LED == 1)                    GPIOE->BSRR = GPIO_Pin_4;       // 
  //else                                  GPIOE->BRR  = GPIO_Pin_4;
  
  if(!Digital_output.O_WDD_B_R)         GPIOB->BSRR = GPIO_Pin_7;     //
  else                                  GPIOB->BRR  = GPIO_Pin_7;
  
  if(Digital_output.O_PWM1_EN)          GPIOE->BSRR = GPIO_Pin_5;      
  else                                  GPIOE->BRR  = GPIO_Pin_5;
  
  if(Digital_output.O_PWM2_EN)          GPIOE->BSRR = GPIO_Pin_6;      
  else                                  GPIOE->BRR  = GPIO_Pin_6;

  if(Digital_output.O_DOOR3_AC)         GPIOC->BSRR = GPIO_Pin_5;    
  else                                  GPIOC->BRR  = GPIO_Pin_5;
  
  if(Digital_output.O_DOOR2_Combo)      GPIOC->BSRR = GPIO_Pin_4;    
  else                                  GPIOC->BRR  = GPIO_Pin_4;
  
  if(Digital_output.O_DOOR1_Chademo)    GPIOA->BSRR = GPIO_Pin_7;    
  else                                  GPIOA->BRR  = GPIO_Pin_7;
  
  if(Digital_output.O_RY5_discharge)    GPIOA->BSRR = GPIO_Pin_6;     
  else                                  GPIOA->BRR  = GPIO_Pin_6;
  
  if(Digital_output.O_RY4_Combo_N)      GPIOB->BSRR = GPIO_Pin_0;     
  else                                  GPIOB->BRR  = GPIO_Pin_0;
  
  if(Digital_output.O_RY3_Combo_P)      GPIOB->BSRR = GPIO_Pin_1;      
  else                                  GPIOB->BRR  = GPIO_Pin_1;
  
  if(Digital_output.O_RY2_Chademo_N)    GPIOB->BSRR = GPIO_Pin_2;     // +24V용 릴레이
  else                                  GPIOB->BRR  = GPIO_Pin_2;
  
  if(Digital_output.O_RY1_Chademo_P)    GPIOE->BSRR = GPIO_Pin_10;    // 방전저항
  else                                  GPIOE->BRR  = GPIO_Pin_10;
  
  if(Digital_output.O_RLED)             GPIOD->BSRR = GPIO_Pin_10;     
  else                                  GPIOD->BRR  = GPIO_Pin_10; 
  
  if(Digital_output.O_GLED)             GPIOD->BSRR = GPIO_Pin_11;      
  else                                  GPIOD->BRR  = GPIO_Pin_11; 
  
  if(Digital_output.O_BLED)             GPIOD->BSRR = GPIO_Pin_12;     
  else                                  GPIOD->BRR  = GPIO_Pin_12; 
  
  if(Digital_output.O_ELCB_TRIP)        GPIOD->BSRR = GPIO_Pin_13;  
  else                                  GPIOD->BRR  = GPIO_Pin_13;

  if(Digital_output.O_WAKE_RY1)         GPIOD->BSRR = GPIO_Pin_14;    
  else                                  GPIOD->BRR  = GPIO_Pin_14;
  
  if(Digital_output.O_WAKE_G_RY2)       GPIOD->BSRR = GPIO_Pin_15;  
  else                                  GPIOD->BRR  = GPIO_Pin_15;
  
  if(Digital_output.O_SOL)              GPIOC->BSRR = GPIO_Pin_6;    
  else                                  GPIOC->BRR  = GPIO_Pin_6;
  
  if(Digital_output.O_MC1_Main)         GPIOC->BSRR = GPIO_Pin_7;  
  else                                  GPIOC->BRR  = GPIO_Pin_7;
  
  if(Digital_output.O_MC2_AC)           GPIOC->BSRR = GPIO_Pin_8;   
  else                                  GPIOC->BRR  = GPIO_Pin_8;
  
  if(Digital_output.O_MC3_precharge)    GPIOC->BSRR = GPIO_Pin_9;   
  else                                  GPIOC->BRR  = GPIO_Pin_9;
  
  if(Digital_output.O_FAN)              GPIOA->BSRR = GPIO_Pin_8;      
  else                                  GPIOA->BRR  = GPIO_Pin_8;
}


void GBT_connector_lock_control(void)
{

   switch (cable_lock_sequence) // 1ms 마다 폴링
   {
     case 0:
       if(gbt_connector_lock_flag == LOCK)
          {
           CABLE_LOCK();           //1
           cable_lock_timer = 0;
           cable_lock_sequence++;
          }
       break;
       
     case 1:
         if(cable_lock_timer >= 1000)
         {
           CABLE_NORMAL();
           cable_lock_timer = 0;
           cable_lock_sequence++;
         }
       break;
       
     case 2:
         if(cable_lock_timer >= 1000)
         {
           CABLE_LOCK();           //2
           cable_lock_timer = 0;
           cable_lock_sequence++;
         }
     break;
     
     case 3:
         if(cable_lock_timer >= 1000)
         {
           CABLE_NORMAL();
           cable_lock_timer = 0;
           cable_lock_sequence++;
         }
       break;
       
     case 4:
         if(cable_lock_timer >= 1000)
         {
           CABLE_LOCK();           //3
           cable_lock_timer = 0;
           cable_lock_sequence++;
         }
     break;
     
     case 5:
         if(cable_lock_timer >= 1000)
         {
           CABLE_NORMAL();
           cable_lock_timer = 0;
           cable_lock_sequence = 0;
           if(Digital_input.bit.I_LIMIT1 == 0) usRegInputBuf_2[6] |= 0x0020; //충전 커플러 잠김상태 불량(Lock 폴트), 충전중에도 계속 감지해야??
           gbt_connector_lock_flag = NORMAL;
         }
       break;
   }
   
   switch (cable_unlock_sequence) // 1ms 마다 폴링
   {
     case 0:
       if(gbt_connector_lock_flag == UNLOCK)
          {
           CABLE_UNLOCK();           //1
           cable_unlock_timer = 0;
           cable_unlock_sequence++;
          }
       break;
       
     case 1:
         if(cable_unlock_timer >= 1000)
         {
           CABLE_NORMAL();
           cable_unlock_timer = 0;
           cable_unlock_sequence++;
         }
       break;
       
     case 2:
         if(cable_unlock_timer >= 1000)
         {
           CABLE_UNLOCK();           //2
           cable_unlock_timer = 0;
           cable_unlock_sequence++;
         }
     break;
     
     case 3:
         if(cable_unlock_timer >= 1000)
         {
           CABLE_NORMAL();
           cable_unlock_timer = 0;
           cable_unlock_sequence++;
         }
       break;
       
     case 4:
         if(cable_unlock_timer >= 1000)
         {
           CABLE_UNLOCK();           //3
           cable_unlock_timer = 0;
           cable_unlock_sequence++;
         }
     break;
     
     case 5:
         if(cable_unlock_timer >= 1000)
         {
           CABLE_NORMAL();
           cable_unlock_timer = 0;
           cable_unlock_sequence = 0;
           if(Digital_input.bit.I_LIMIT1 == 1) usRegInputBuf_2[6] |= 0x0008; //충전 커플러 풀림상태 불량(Unlock 폴트)
           gbt_connector_lock_flag = NORMAL;
         }
       break;
   }

   
}

/*
충전중 녹색
대기 노란색
충전정지인데 케이블 꽂혀있으면 빨간색
경고는 빨간색 점멸

*/
void LED_DISPLAY_Service(void)      // 상태등 표시
{
    if (eFaultState == STATE_FAULT)
    {
        if(blink)
        {
                                       // 1번핀 12V, CN38
            Digital_output.O_RLED = 1; //RED
            Digital_output.O_GLED = 0; //AMBER
            Digital_output.O_BLED = 0; //GREEN
//          Digital_output.O_DOOR1_Chademo = 1; //BUZZER, CN 44의 2번 핀
        }
        else
        {
            Digital_output.O_RLED = 0;
            Digital_output.O_GLED = 0; //AMBER
            Digital_output.O_BLED = 0;
//          Digital_output.O_DOOR1_Chademo = 0; //BUZZER, CN 44의 2번 핀
        }
    }
    else if (eSysState == STATE_RUNNING) // 충전중 , 녹색
    {
        Digital_output.O_RLED = 0; //RED
        Digital_output.O_GLED = 0; //AMBER
        Digital_output.O_BLED = 1; //GREEN
//      Digital_output.O_DOOR1_Chademo = 0;
    }
    else if ((eSysState == STATE_READY || eSysState == STATE_FINISH) && (cpline != DC_12V_MODE)) // 케이블이 연결되어 있을 경우 빨강
    {  // 9V, 9V PWM, 6V PWM
        Digital_output.O_RLED = 1; //RED
        Digital_output.O_GLED = 0; //AMBER
        Digital_output.O_BLED = 0; //GREEN
//      Digital_output.O_DOOR1_Chademo = 0;
    }
    else if ((eSysState == STATE_READY || eSysState == STATE_FINISH) && (cpline == DC_12V_MODE))  // 케이블이 분리되었을 경우 노랑
    {
       //12V
        Digital_output.O_RLED = 0; //RED
        Digital_output.O_GLED = 1; //AMBER
        Digital_output.O_BLED = 0; //GREEN
//      Digital_output.O_DOOR1_Chademo = 0;
    }
    else
    {
        Digital_output.O_RLED = 1; //RED
        Digital_output.O_GLED = 1; //AMBER
        Digital_output.O_BLED = 1; //GREEN
    }
    
    
    
//    else if (eSysState == STATE_RUNNING || Channel2_RUN == 1) // 2019.04.17 Channel2_RUN 조건 추가 - 2채널이지만 LED가 채널1에서만 제어하므로
//    {
//        Digital_output.O_RLED = 1;
//        Digital_output.O_GLED = 0;
//        Digital_output.O_BLED = 0;
//    }
//    else if (eSysState == STATE_FINISH)
//    {
//        Digital_output.O_RLED = 0;
//        Digital_output.O_GLED = 1;
//        Digital_output.O_BLED = 0;
//    }
//    else if ((eSysState == STATE_STANDBY) || (eSysState == STATE_READY))
//    {
//        Digital_output.O_RLED = 0;
//        Digital_output.O_GLED = 0;
//        Digital_output.O_BLED = 1;
//    }
//    else
//    {
//        Digital_output.O_RLED = 1;
//        Digital_output.O_GLED = 1;
//        Digital_output.O_BLED = 1;
//    }
    
    
}

void Count_section_100ms(void)
{
    if (fault_timer < 65535)                fault_timer++;
    
    if (Green_count < 255)                  Green_count++;
    
    if (procnt < 255)                       procnt++;
    
    if (Communication_Err_cnt < 255)        Communication_Err_cnt++;
    
    if (count_mc2_ac_off < 255)             count_mc2_ac_off++;
    
    if (count_rcd_check < 255)              count_rcd_check++;
    
    if (counter_cp_relay_off < 255)         counter_cp_relay_off++;
    
    if (counter_pwm_9V_mode_holding < 400)  counter_pwm_9V_mode_holding++;
    
    if (count_welding_check < 255)          count_welding_check++;
    
    if (count_mc3_check < 255)              count_mc3_check++;    
    
    if (screen_error_count < 255)           screen_error_count++;
}

uint8_t DM_CS_u8(uint8_t *Data, int n)
{
  uint8_t CS_temp = 0;
 
  for ( uint8_t i=1; i < n; i++)
  {
       CS_temp += Data[i];
  }
  return CS_temp;
}

uint16_t RY_open_check(uint32_t RY_in, uint32_t RY_out, uint16_t* chk_cnt)
{    
    (*chk_cnt)++;
    if (!RY_in)   
    {
        if (RY_out)
        {
            if (*chk_cnt >= 1500)	
            {
                *chk_cnt = 1500;
                return 1;
            }
        }
        else    *chk_cnt = 0;
    }
    else	*chk_cnt = 0;  

    return 0;    
}

uint8_t RY_welding_check(uint32_t RY_in, uint32_t RY_out, uint16_t* chk_cnt)
{    
    (*chk_cnt)++;
    if (RY_in)   
    {
        if (!RY_out)
        {
            if (*chk_cnt >= 1500)	
            {
                *chk_cnt = 1500;
                return 1;
            }
        }
        else    *chk_cnt = 0;
    }
    else	*chk_cnt = 0;  

    return 0;    
}

/*
void Out_Pow_Cal()  
{
  if((usRegInputBuf_2[0] & 0x0010) == 0x0010)             // 충전중 상태 
  {
    if(Iout > 2)
    {
//      if(pm_2_Cur_ctr == 1)
//      {
////           u_DM_W_cal_Value.fValue += (((Vout * (Iout + uf_oth_CH_Iout_state_1.fValue)) / 3600));
//        u_DM_W_cal_Value.fValue = (((Vout * (Iout + uf_oth_CH_Iout_state_1.fValue)) / 10));
//      }        
//      else      u_DM_W_cal_Value.fValue = (((Vout * Iout) / 10));      
////        u_DM_W_cal_Value.fValue += (((Vout * Iout) / 3600));
////        u_DM_W_cal_Value.fValue ++;
      
        u_DM_W_cal_Value.fValue = (((Vout * Iout) / 10));
    }
    
  }
  else  u_DM_W_cal_Value.fValue = 0;
}
*/

void CABLE_LOCK (void)
{
   Digital_output.O_DOOR1_Chademo = 1; // ENABLE
   Digital_output.O_DOOR2_Combo   = 1;
   Digital_output.O_DOOR3_AC      = 0;  
}

void CABLE_UNLOCK (void)
{
   Digital_output.O_DOOR1_Chademo = 1; // ENABLE
   Digital_output.O_DOOR2_Combo   = 0;
   Digital_output.O_DOOR3_AC      = 1;  
}

void CABLE_NORMAL (void)
{
   Digital_output.O_DOOR1_Chademo = 0;
   Digital_output.O_DOOR2_Combo   = 0;
   Digital_output.O_DOOR3_AC      = 0;  
}


