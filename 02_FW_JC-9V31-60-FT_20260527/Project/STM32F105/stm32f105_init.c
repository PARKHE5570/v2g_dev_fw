
#define _STM32F105_INIT_C


#include "stm32f10x.h"
#include "main.h"
#include "stm32f105_init.h"
#include "INFY_V2G.h"
#include "PLC_3_0.h"
#include "HMI_modbus.h"

void System_Init(void)
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO  |
                         RCC_APB2Periph_GPIOA |
                         RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 |
                         RCC_APB2Periph_ADC1 |
                         RCC_APB2Periph_USART1, ENABLE);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 |
                         RCC_APB1Periph_USART3 |
                         RCC_APB1Periph_CAN1   |
                         RCC_APB1Periph_CAN2, ENABLE);
  
  DMA1_Config();
  TIM1_Configuration();
  ADC1_Configuration();
  
  GPIO_Config();
  UART_Init(1,460800);  // UART Monitoring Terminal RS232
  UART_Init(2,38400);   // HMI RS485
//  UART_Init(3,9600);    // Not used
  CAN_Config();
  
  Init_RxMes(&RxMessage1, CAN_ID_EXT); // power module
  Init_TxMes(&TxMessage1, CAN_ID_EXT);
  
  Init_RxMes(&RxMessage2, CAN_ID_EXT); // plc modem
  Init_TxMes(&TxMessage2, CAN_ID_EXT);
  
}

void DMA1_Config()
{
  /*********************** DMA_ADC ***********************/
    DMA_InitTypeDef     DMA_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;

    DMA_DeInit(DMA1_Channel1);                  //DMA1 Channel1 Reset
    DMA_StructInit(&DMA_InitStructure);         //DMA_Struct_Initialize
    
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    
    DMA_InitStructure.DMA_BufferSize = 9;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);        //DMA1 Channel1 Set

    ADC_DMACmd(ADC1, ENABLE);                           //Allow ADC1 to transmit a DMA request

    DMA1_Channel1->CNDTR = 9;                           //Same with DMA_BufferSize setting.Just reconfirm.
    DMA1_Channel1->CMAR = (uint32_t)&ADCConvertedValue; //Smae with DMA_MemoryBaseAddr setting. Just reconfirm.

    DMA_Cmd(DMA1_Channel1, ENABLE);                     //DMA1 Channel1 Start

    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);     //TC(Transfer Complete) -> After transmission is complete, an interrupt occurs
    
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /**********************************************************/
    
    /*********************** USART2_ADC ***********************/
    
    DMA_DeInit(DMA1_Channel7);
    DMA_StructInit(&DMA_InitStructure);
    
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)bTxSio_2; 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    
    DMA_InitStructure.DMA_BufferSize = cTxdMax_2;  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);
    
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);

    DMA1_Channel7->CNDTR = cTxdMax_2;
    DMA1_Channel7->CMAR = (uint32_t)bTxSio_2;

    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /**********************************************************/
}

void TIM1_Configuration(void)
{
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
    NVIC_InitTypeDef            NVIC_InitStructure;
      
    TIM_TimeBaseStructure.TIM_Prescaler         = 1;                    // (Prescaler+1=2), [72MHz / {2 * (35999+1)}] = **** 1kHz (1ms) ****
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;   // Counter direction : 0 to Period
    TIM_TimeBaseStructure.TIM_Period            = 35999;
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    NVIC_InitStructure.NVIC_IRQChannel                          = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority               = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                       = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

void ADC1_Configuration()
{
    ADC_InitTypeDef     ADC_InitStructure;

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 9;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    // GPIOA
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0,  1, ADC_SampleTime_28Cycles5);  //[0] PA0     //AD_PLC_CONN
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1,  2, ADC_SampleTime_28Cycles5);  //[1] PA1     //CABLE_AD3
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4,  3, ADC_SampleTime_28Cycles5);  //[2] PA4     //CABLE_AD4
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5,  4, ADC_SampleTime_28Cycles5);  //[3] PA5     //CABLE_AD1
    
    // GPIOC
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 5, ADC_SampleTime_28Cycles5);  //[4] PC0     //AD_CP1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 6, ADC_SampleTime_28Cycles5);  //[5] PC1     //AD_PLC_PT1000
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 7, ADC_SampleTime_28Cycles5);  //[6] PC2     //AD_DCPT
    ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 8, ADC_SampleTime_28Cycles5);  //[7] PC3     //CABLE_AD5
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 9, ADC_SampleTime_28Cycles5);  //[8] PC4     //CABLE_AD2
    
    ADC_Cmd(ADC1, ENABLE);                      //ADC1 ON
  
    ADC_ResetCalibration(ADC1);                 //Calibration Reset
    while(ADC_GetResetCalibrationStatus(ADC1)); //Waiting for initialization to complete

    ADC_StartCalibration(ADC1);                 //Calibration Start
    while(ADC_GetCalibrationStatus(ADC1));      //Waiting for calibration to complete

    ADC_SoftwareStartConvCmd(ADC1, ENABLE); 
}

void IWDG_Configuration()  
{  
	
	/* IWDG_PR and IWDG_RLR  */  
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  
	
	/* IWDG*/  
	IWDG_SetPrescaler(IWDG_Prescaler_32); //IWDG_PR: 40KHz(LSI) / 32 = 1.25 KHz -> 800us  
	
	/* IWDG*/  
	IWDG_SetReload(349); //349 * 800us = 280ms  
	
	/* IWDGIWDG*/  
	IWDG_ReloadCounter();  //Start code and after 100ms refresh
	
	/* IWDG (the LSI oscillator will be enabled by hardware) */  
	IWDG_Enable();  
}

void WWDG_Configuration()
{
	/* WWDG configuration */
	/* Enable WWDG clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	
	/* WWDG clock counter = (36MHz[PCLK1]/4096*8[Prescaler]) = 1098.6 [Hz] = 1/1098.6 [s] = 0.91 [ms]  */
	WWDG_SetPrescaler(WWDG_Prescaler_8);
	
	/* Set Window value to 120; WWDG counter should be refreshed only when the counter
	is below 120 (and greater than 64) otherwise a reset will be generated */
	WWDG_SetWindowValue(120);
	/* Enable WWDG and set counter value to 127, WWDG timeout = 0.91 [ms] * (127-64) = 57.33 ms 
	In this case the refresh window is: 0.91 [ms] * (127-120) = 6.37 [ms] < refresh window < 0.91 [ms] * (127-64) = 57.33 [ms]
	*/
	WWDG_Enable(127);       //Start code
	
	/* Enables the WWDG Early Wakeup interrupt(EWI). */
	WWDG_ClearFlag();
	// WWDG_EnableIT();
}

void Init_RxMes(CanRxMsg *RxMessage, uint8_t CAN_IDE)
{
	u8 i = 0;
	
	RxMessage->StdId = 0x00;
	RxMessage->ExtId = 0x00;
	RxMessage->IDE = CAN_IDE;
	RxMessage->RTR = CAN_RTR_DATA;
	RxMessage->DLC = 8;
	RxMessage->FMI = 0;
	for (i = 0;i < 8;i++)
	{
		RxMessage->Data[i] = 0x00;
	}
}

void Init_TxMes(CanTxMsg *TxMessage, uint8_t CAN_IDE)
{
	u8 i = 0;
	
	TxMessage->StdId=0;
	TxMessage->ExtId=0;
	TxMessage->IDE=CAN_IDE;
	TxMessage->RTR=CAN_RTR_DATA;
	TxMessage->DLC=8;
	for (i = 0;i < 8;i++)
	{
		TxMessage->Data[i] = 0x00;
	}
}

void Init_CAN_buf(uint8_t *CAN_buf)
{
	u8 i = 0;
	for (i = 0;i < 8;i++)
	{
		CAN_buf[i] = 0x00;
	}
}

void GPIO_Config()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    /*********************** GPIO_OUTPUT ***********************/
    
                                        //V5.2_name             //Connected     //Acture_use
    
    GPIOA->BRR = GPIO_Pin_8;            //O_MC1                 //CN32          //MC1_Coil
    
    GPIOB->BRR = GPIO_Pin_0;            //O_RY3                 //CN3           //Not_used
    GPIOB->BRR = GPIO_Pin_1;            //O_RY2                 //CN2           //RY2_Coil
    GPIOB->BRR = GPIO_Pin_2;            //O_RY1                 //CN1           //RY1_Coil
    GPIOB->BSRR = GPIO_Pin_6;           //PWM_CP1               //CN23          //Not_used

    GPIOC->BRR = GPIO_Pin_5;            //O_RY4                 //CN4           //Not_used
    GPIOC->BRR = GPIO_Pin_6;            //O_ELCB_TRIP           //CN43          //MCCB1_SOR1
    GPIOC->BRR = GPIO_Pin_7;            //O_FAN                 //CN35          //MC2_Coil
    GPIOC->BRR = GPIO_Pin_8;            //O_MC3                 //CN34          //Not_used
    GPIOC->BRR = GPIO_Pin_9;            //O_MC2                 //CN33          //Not_used
    GPIOC->BRR = GPIO_Pin_11;           //O_STATUS_LED4         //LED34         //Not_used
    GPIOC->BRR = GPIO_Pin_12;           //O_STATUS_LED3         //LED29         //Not_used
    GPIOC->BRR = GPIO_Pin_13;           //U2_EN                 //U2_EN         //U2_EN
    GPIOC->BSRR = GPIO_Pin_14;          //UIRESET1              //CN17, 19      //Not_used
    GPIOC->BSRR = GPIO_Pin_15;          //UIRESET2              //CN18          //Not_used
    
    GPIOD->BRR = GPIO_Pin_0;            //O_STATUS_LED2         //LED30         //Not_used
    GPIOD->BRR = GPIO_Pin_1;            //O_STATUS_LED1         //LED31         //+12Vdc_Input
    GPIOD->BRR = GPIO_Pin_2;            //TP73                  //TP73          //Not_used
    GPIOD->BRR = GPIO_Pin_4;            //O_FAULT               //LED28         //Fault_LED
    GPIOD->BRR = GPIO_Pin_13;           //O_SOL                 //CN40-7        //Not_used        
    GPIOD->BRR = GPIO_Pin_14;           //O_WAKE_RY2            //CN40-2        //Not_used        
    GPIOD->BRR = GPIO_Pin_15;           //O_WAKE_RY1            //CN39-1        //Not_used
    
    GPIOE->BRR = GPIO_Pin_4;            //O_BLED                //CN38-4        //Blue_LED
    GPIOE->BRR = GPIO_Pin_5;            //O_GLED                //CN38-3        //Green_LED        
    GPIOE->BRR = GPIO_Pin_6;            //O_RLED                //CN38-2        //RED_LED        
    GPIOE->BRR = GPIO_Pin_9;            //O_CP_RY1              //CN23          //Not_used
    GPIOE->BRR = GPIO_Pin_11;           //O_DOOR3               //CN46          //Not_used
    GPIOE->BRR = GPIO_Pin_12;           //O_DOOR2               //CN45          //Not_used
    GPIOE->BRR = GPIO_Pin_13;           //O_DOOR1               //CN44          //Not_used
    GPIOE->BRR = GPIO_Pin_14;           //O_RY5                 //CN30          //Not_used
    GPIOE->BRR = GPIO_Pin_15;           //U3_EN                 //CN51, 22, 55, 24        //U3_EN
    
    /**********************************************************/
    
    /*********************** GPIO_INPUT ***********************/
    
    // GPIOA: Pin_6, Pin_7
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //GPIOA_Pin_6       //I_RY2         //CN2           //RY2_Contact
    //GPIOA_Pin_7       //I_RY3         //CN3           //Not_used
    
    // GPIOB: Pin_5, Pin_8, Pin_9, Pin_14, Pin_15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //GPIOB_Pin_5       //I_ELCB_TRIP   //CN56          //Not_used
    //GPIOB_Pin_8       //I_MC1         //CN8           //MC1_Contact
    //GPIOB_Pin_9       //LIMIT2        //CN7           //Fuse9_Contact
    //GPIOB_Pin_14      //I_DOOR1       //CN44          //Not_used
    //GPIOB_Pin_15      //I_DOOR2       //CN45          //Not_used
    
    // GPIOC: Pin_10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIOC_Pin_10      //I_RY4         //CN4           //Not_used
    
    // GPIOD: Pin_3, Pin_5, Pin_6, Pin_7, Pin_8, Pin_9, Pin_10, Pin_11, Pin_12
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    //GPIOD_Pin_3       //I_RY1         //CN1           //RY1_Contact
    //GPIOD_Pin_5       //I_ELCB        //CN25          //MCCB1_Contact
    //GPIOD_Pin_6       //I_LIMIT       //CN26          //DOOR_Contact
    //GPIOD_Pin_7       //I_MC3         //CN27          //SPD1_Contact
    //GPIOD_Pin_8       //I_DOOR3       //CN46          //Not_used
    //GPIOD_Pin_9       //I_EMG         //+12V EMG      //+12V_EMG
    //GPIOD_Pin_10      //WDD_B_R       //CN21-7        //Not_used
    //GPIOD_Pin_11      //I_WELDING     //CN21-1        //Not_used        
    //GPIOD_Pin_12      //I_RCD_TRIP    //CN20          //Not_used
    
    // GPIOE: Pin_0, Pin_1, Pin_2, Pin_3, Pin_7, Pin_8, Pin_10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    //GPIOE_Pin_0       //I_DCGF        //CN41          //Fuse8_Contact
    //GPIOE_Pin_1       //LIMIT1        //CN5           //COUPLER_SW1_Contact
    //GPIOE_Pin_2       //+3.3V or GND  //CN6           //Not_used
    //GPIOE_Pin_3       //LIMIT3        //CN10          //MC2_Contact
    //GPIOE_Pin_7       //I_Sol         //CN40-7        //Not_used
    //GPIOE_Pin_8       //I_PILOT       //CN40-3        //Not_used
    //GPIOE_Pin_10      //I_CABLE24V    //CN62          //Not_used
    
    /**********************************************************/

    /*************************** AD ***************************/
    
    // GPIOA: PA0, PA1, PA4, PA5 -> AIN 모드 설정
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //GPIOA_Pin_0,      //AD_PLC_CONN           //CN63-1        //CCS_CABLE1_CS
    //GPIOA_Pin_1,      //CABLE_AD3             //CN66          //Not_used
    //GPIOA_Pin_4,      //CABLE_AD4             //CN67          //Not_used
    //GPIOA_Pin_5,      //CABLE_AD1             //CN64          //Not_used
    
    // GPIOC: PC0, PC1, PC2, PC3, PC4 -> AIN 모드 설정
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIOC_Pin_0,      //AD_CP1                //CN23          //Not_used
    //GPIOC_Pin_1,      //AD_PLC_PT1000         //CN63-2        //CCS_CABLE1_PT1000
    //GPIOC_Pin_2,      //AD_DCPT               //CN57-2        //Not_used
    //GPIOC_Pin_3,      //CABLE_AD5             //CN68          //Not_used
    //GPIOC_Pin_4,      //CABLE_AD2             //CN65          //Not_used
    
    /**********************************************************/
    
}


void UART_Init(u8 uart,u32 baud)
{
  /*
  * USARTx configured as follow:
  * - BaudRate = baud
  * - Word Length = 8 Bits
  * - One Stop Bit
  * - No parity
  * - Hardware flow control disabled
  * - Receive and transmit enabled
  *
  * [Pin Mapping]
  * UART1 : PA9(Tx),  PA10(Rx) - RS232 Monitoring Terminal
  * UART2 : PA2(Tx),  PA3(Rx)  - HMI RS485
  * UART3 : PB10(Tx), PB11(Rx) - Not_used
  *
  * Note: USART1/2/3 clock is already enabled in System_Init().
  */
	
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;  
	
	if(uart == 1)
	{
		/* Enable USART1 clock */
		//Declaration completed within the “System_init()” function.
                
		/* Configure USART1 Tx (PA.9) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
                
		/* Configure USART1 Rx (PA.10) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //ST3232B-R1OUT Connected, always active output
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
                USART_InitStructure.USART_BaudRate            = baud;
                USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
                USART_InitStructure.USART_StopBits            = USART_StopBits_1;
                USART_InitStructure.USART_Parity              = USART_Parity_No;
                USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
                USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
                
                /* Configure UART1 */
                USART_Init(USART1, &USART_InitStructure);
                
                /* Enable the UART1 */
                USART_Cmd(USART1, ENABLE);
                
                /* Enable USART1 Receive interrupt */
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
                
                /* Enable the USART1 Interrupt */
                NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
                NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
                NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
                NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
                NVIC_Init(&NVIC_InitStructure);
                
	}
	
	else if(uart == 2)
	{
		/* Enable USART2 clock */
                //Declaration completed within the “System_init()” function.
                
		/* Configure USART2 Tx (PA.2) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
                
		/* Configure USART2 Rx (PA.3) as input pull-up */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           //During transmission, the MAX3430ESA-RO enters a high-impedance state.
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		USART_InitStructure.USART_BaudRate            = baud;
                USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
                USART_InitStructure.USART_StopBits            = USART_StopBits_1;
                USART_InitStructure.USART_Parity              = USART_Parity_No;
                USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
                USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
                
                /* Configure UART2 */
                USART_Init(USART2, &USART_InitStructure);
                
                /* Enable the UART2 */
                USART_Cmd(USART2, ENABLE);
                
                /* Enable USART2 Receive interrupt */
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
                
                /* Enable the USART2 Interrupt */
                NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;
                NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
                NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
                NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
                NVIC_Init(&NVIC_InitStructure);
                
	}
	else if(uart == 3)
	{
		/* Enable USART3 clock */
		//Declaration completed within the “System_init()” function.
                
		/* Configure USART3 Tx (PB.10) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
                
		/* Configure USART3 Rx (PB.11) as input pull-up */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           //During transmission, the MAX3430ESA-RO enters a high-impedance state.
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		USART_InitStructure.USART_BaudRate = baud;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
                
                /* Configure UART3 */
                USART_Init(USART3, &USART_InitStructure);
                
                /* Enable the UART3 */
                USART_Cmd(USART3, ENABLE);
                
                /* Enable USART3 Receive interrupt */
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
                
                /* Enable the USART3 Interrupt */
                NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQn;
                NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
                NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
                NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
                NVIC_Init(&NVIC_InitStructure);
	}
}


void CAN_Config(void)
{
  
/*----------------------------------CAN GPIO----------------------------------*/
  
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	/* GPIO clock enable */
	
	/* __CAN1_USED__ */
	/* Configure CAN pin: RX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure CAN pin: TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* __CAN2_USED__ */
	/* Configure CAN pin: RX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Configure CAN pin: TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
/*----------------------------------CAN1----------------------------------*/
        
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE; //time-triggered communication mode = DISABLED
	CAN_InitStructure.CAN_ABOM = ENABLE;// DISABLE; // automatic bus-off management mode = DISABLED
	CAN_InitStructure.CAN_AWUM = DISABLE;//DISABLE; // automatic wake-up mode = DISABLED
	CAN_InitStructure.CAN_NART = DISABLE; // non-automatic retransmission mode = DISABLED
	CAN_InitStructure.CAN_RFLM = DISABLE; // receive FIFO locked mode = DISABLED
	CAN_InitStructure.CAN_TXFP = DISABLE; // transmit FIFO priority = DISABLED
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;//CAN_Mode_LoopBack;//CAN_Mode_Normal;
	
        //Sample-point : 87.5%
        //[36Mhz/{(1+13+2)*18}] = 125,000
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;        // synchronization jump width = 1
	CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 18;
	CAN_Init(CAN1, &CAN_InitStructure);             //125 [kbit/s]
	
        
	/* CAN filter init for INFY*/ 
	CAN_FilterInitStructure.CAN_FilterNumber        = 0;
	CAN_FilterInitStructure.CAN_FilterMode          = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale         = CAN_FilterScale_32bit; // One 32-bit filter for the STDID[10:0], EXTID[17:0], IDE and RTR bits
                                                                                        //0x04 : IDE (1 = Extended ID)        
	CAN_FilterInitStructure.CAN_FilterIdHigh        = (uint16_t)(((0x02A3F000 << 3) | 0x04) >> 16);
	CAN_FilterInitStructure.CAN_FilterIdLow         = (uint16_t)(((0x02A3F000 << 3) | 0x04) & 0xFFFF);
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh    = (uint16_t)(((0x1F0FFF00 << 3) | 0x04) >> 16);
	CAN_FilterInitStructure.CAN_FilterMaskIdLow     = (uint16_t)(((0x1F0FFF00 << 3) | 0x04) & 0xFFFF);
        
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
	CAN_FilterInitStructure.CAN_FilterActivation    = ENABLE; 
	CAN_FilterInit(&CAN_FilterInitStructure);
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE); 
	
/*----------------------------------CAN2----------------------------------*/
        
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE; //time-triggered communication mode = DISABLED
	CAN_InitStructure.CAN_ABOM = ENABLE;// DISABLE; // automatic bus-off management mode = DISABLED
	CAN_InitStructure.CAN_AWUM = DISABLE;//DISABLE; // automatic wake-up mode = DISABLED
	CAN_InitStructure.CAN_NART = DISABLE; // non-automatic retransmission mode = DISABLED
	CAN_InitStructure.CAN_RFLM = DISABLE; // receive FIFO locked mode = DISABLED
	CAN_InitStructure.CAN_TXFP = DISABLE; // transmit FIFO priority = DISABLED
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;//CAN_Mode_LoopBack;//CAN_Mode_Normal;
	
        //Sample-point : 88.9%
        //[36Mhz/{(1+15+2)*4}] = 500,000
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;        // synchronization jump width = 1
	CAN_InitStructure.CAN_BS1 = CAN_BS1_15tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 4;            //500 [kbit/s]
	CAN_Init(CAN2, &CAN_InitStructure);
	
	
	//plc 3.0
	CAN_FilterInitStructure.CAN_FilterNumber        = 14;
	CAN_FilterInitStructure.CAN_FilterMode          = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale         = CAN_FilterScale_32bit; // One 32-bit filter for the STDID[10:0], EXTID[17:0], IDE and RTR bits
                                                                                        //0x04 : IDE (1 = Extended ID)
	CAN_FilterInitStructure.CAN_FilterIdHigh        = (uint16_t)(((0x15EC0000 << 3) | 0x04) >> 16);
	CAN_FilterInitStructure.CAN_FilterIdLow         = (uint16_t)(((0x15EC0000 << 3) | 0x04) & 0xFFFF);
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh    = (uint16_t)(((0x1FFF0000 << 3) | 0x04) >> 16);
	CAN_FilterInitStructure.CAN_FilterMaskIdLow     = (uint16_t)(((0x1FFF0000 << 3) | 0x04) & 0xFFFF);
        
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment= 0;
	CAN_FilterInitStructure.CAN_FilterActivation    = ENABLE;//DISABLE;//ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	CAN_ITConfig(CAN2,CAN_IT_FMP0, ENABLE);
        
        //Sendyne IMD : SIM200
        CAN_FilterInitStructure.CAN_FilterNumber        = 15;
	CAN_FilterInitStructure.CAN_FilterMode          = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale         = CAN_FilterScale_32bit; // One 32-bit filter for the STDID[10:0], EXTID[17:0], IDE and RTR bits
                                                                                        //0x04 : IDE (1 = Extended ID)
	CAN_FilterInitStructure.CAN_FilterIdHigh        = (uint16_t)(((0x0A100100 << 3) | 0x04) >> 16);
	CAN_FilterInitStructure.CAN_FilterIdLow         = (uint16_t)(((0x0A100100 << 3) | 0x04) & 0xFFFF);
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh    = (uint16_t)(((0x1FFFFF00 << 3) | 0x04) >> 16);
	CAN_FilterInitStructure.CAN_FilterMaskIdLow     = (uint16_t)(((0x1FFFFF00 << 3) | 0x04) & 0xFFFF);
        
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment= 0;
	CAN_FilterInitStructure.CAN_FilterActivation    = ENABLE;//DISABLE;//ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
        CAN_ITConfig(CAN2,CAN_IT_FMP0, ENABLE);
        

	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;//CAN_RX1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);       
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;//CAN_RX2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}


