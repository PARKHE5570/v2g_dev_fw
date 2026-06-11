/**
 * @file   Board_Pin_Config.h
 * @brief  V2G Charger (JC-9V31-60-FT) V5.2 Board Pin Map 
 *        
 *  [Digital INPUT]  접점 Close=HIGH=1 / Open=LOW=0 으로 통일
 *  논리 신호             Pin       접점    	기능
 *  PIN_I_EMG          PD9        NC    	비상정지(정상=Close, 눌림=Open)
 *  PIN_I_DOOR         PD6        NC    	DOOR1(닫힘=Close, 열림=Open)
 *  PIN_I_SPD          PD7        NC    	SPD1(정상=Close, 고장=Open)
 *  PIN_I_RY1          PD3        NO    	RY1 주접점 피드백
 *  PIN_I_RY2          PA6        NO    	RY2 주접점 피드백
 *  PIN_I_MCCB1        PD5        NC    	MCCB1(Close=정상, Open=차단)
 *  PIN_I_CABLE_1CH    PE1        NC    	커플러 거치(거치=Close)
 *  PIN_I_FUSE9        PB9        NC    	FUSE9(정상=Close, 고장=Open)
 *  PIN_I_MC1          PB8        NO    	MC1 주접점 피드백
 *  PIN_I_FUSE8        PE0        NC    	FUSE8(정상=Close, 고장=Open)
 *  PIN_I_FAN_MC       PE3        NO    	MC2(FAN MC) 주접점 피드백
 *
 *  [Digital OUTPUT]
 *  논리 신호                 Pin        기능명      					커넥터
 *  PIN_O_MC1              PA8        O_MC1  (MC1 코일, 메인전원)  		CN32
 *  PIN_O_RY2              PB1        O_RY2  (RY2 코일)  				CN2
 *  PIN_O_RY1              PB2        O_RY1  (RY1 코일)  				CN1
 *  PIN_O_MCCB1_TRIP       PC6        O_ELCB (MCCB1 션트트립) 			CN43
 *  PIN_O_FAN_MC           PC7        O_FAN  (MC2 코일, FAN MC) 		CN35
 *  PIN_O_RLED_1CH         PE6        O_RLED (LED RED)   			CN38
 *  PIN_O_GLED_1CH         PE5        O_GLED (LED GREEN) 			CN38
 *  PIN_O_BLED_1CH         PE4        O_BLED (LED BLUE)  			CN38
 *  PIN_O_U2_EN            PA1        U2_EN              			(HMI RS485 DIR)
 *  PIN_O_U3_EN            PE15       U3_EN              			(UART DIR, 동일 핀)
 *
 *  [ADC]  BOARD_ADC_NUM_CH: 2
 *  논리 채널                            Pin
 *  BOARD_ADC_COUPLER_SW_CH           ADC_Ch0  (PA0, 커플러 스위치 버튼 ADC) [0]
 *  BOARD_ADC_PT1000_CH               ADC_Ch11 (PC1, 커플러 PT1000 온도)   [1]
 *  ※ DMA 버퍼/배열 크기는 BOARD_ADC_NUM_CH 매크로로 결정 (V5.2=2)
 */

#ifndef __BOARD_PIN_CONFIG_H__
#define __BOARD_PIN_CONFIG_H__

#include "stm32f10x.h"

/* ===========================================================================
 * PIN MAP (edit_EVRange-AutoCharging--JC-9A31-30-CT, SmartCharging_V106)
 * ===========================================================================*/

/*===========================================================================*/
/* ----- Digital INPUT (접점 Close=HIGH=1 / Open=LOW=0) ----- */
/*===========================================================================*/
/* I_EMG (NC): 	비상정지 정상=Close=HIGH, 눌림=Open=LOW. PD9. PIN_I_EMG */
#define PIN_I_EMG_GPIO_PORT           GPIOD
#define PIN_I_EMG_GPIO_PIN            GPIO_Pin_9

/* I_DOOR (NC): DOOR1 닫힘=Close=HIGH, 열림=Open=LOW. PD6. PIN_I_DOOR*/
#define PIN_I_DOOR_GPIO_PORT          GPIOD
#define PIN_I_DOOR_GPIO_PIN           GPIO_Pin_6

/* I_SPD (NC): 	SPD1 정상=Close=HIGH, 고장=Open=LOW. PD7. PIN_I_SPD*/
#define PIN_I_SPD_GPIO_PORT           GPIOD
#define PIN_I_SPD_GPIO_PIN            GPIO_Pin_7

/* I_RY1 (NO): 	RY1 주접점 Close=HIGH. PD3. PIN_I_RY1*/
#define PIN_I_RY1_GPIO_PORT           GPIOD
#define PIN_I_RY1_GPIO_PIN            GPIO_Pin_3

/* I_RY2 (NO): 	RY2 주접점 Close=HIGH. PA7. PIN_I_RY2*/
#define PIN_I_RY2_GPIO_PORT           GPIOA
#define PIN_I_RY2_GPIO_PIN            GPIO_Pin_6

/* I_MCCB1 (NC): MCCB1 정상=Close=HIGH, 차단=Open=LOW. PD5. PIN_I_MCCB1*/
#define PIN_I_MCCB1_GPIO_PORT         GPIOD
#define PIN_I_MCCB1_GPIO_PIN          GPIO_Pin_5

/* I_CABLE_1CH (NC): 커플러 거치=Close=HIGH. PE1. PIN_I_CABLE_1CH*/
#define PIN_I_CABLE_1CH_GPIO_PORT     GPIOE
#define PIN_I_CABLE_1CH_GPIO_PIN      GPIO_Pin_1

/* I_FUSE9 (NC): FUSE9 정상=Close=HIGH, 고장=Open=LOW. PB9. PIN_I_FUSE9*/
#define PIN_I_FUSE9_GPIO_PORT         GPIOB
#define PIN_I_FUSE9_GPIO_PIN          GPIO_Pin_9

/* I_MC1 (NO): MC1 주접점 Close=HIGH. PB8. PIN_I_MC1*/
#define PIN_I_MC1_GPIO_PORT           GPIOB
#define PIN_I_MC1_GPIO_PIN            GPIO_Pin_8

/* I_FUSE8 (NC): FUSE8 정상=Close=HIGH, 고장=Open=LOW. PE0. PIN_I_FUSE8*/
#define PIN_I_FUSE8_GPIO_PORT         GPIOE
#define PIN_I_FUSE8_GPIO_PIN          GPIO_Pin_0

/* I_FAN_MC (NO): MC2(FAN MC) 주접점 Close=HIGH. PE3. PIN_I_FAN_MC*/
#define PIN_I_MC2_GPIO_PORT        GPIOE
#define PIN_I_MC2_GPIO_PIN         GPIO_Pin_3

/*===========================================================================*/
/* ----- Digital OUTPUT ----- */
/*===========================================================================*/
/* O_RY1 (RY1 코일): O_RY1 = PB2 (CN1). PIN_O_RY1*/
#define PIN_O_RY1_GPIO_PORT           GPIOB
#define PIN_O_RY1_GPIO_PIN            GPIO_Pin_2
   
/* O_RY2 (RY2 코일): O_RY2 = PB1 (CN2). PIN_O_RY2*/
#define PIN_O_RY2_GPIO_PORT           GPIOB
#define PIN_O_RY2_GPIO_PIN            GPIO_Pin_1

/* O_MC1 (MC1 코일, 메인전원): O_MC1 = PA8 (CN32). PIN_O_MC1*/
#define PIN_O_MC1_GPIO_PORT           GPIOA
#define PIN_O_MC1_GPIO_PIN            GPIO_Pin_8
   
/* O_MC2 (FAN MC(MC2) 코일): O_FAN = PC7 (CN35). PIN_O_FAN_MC*/
#define PIN_O_MC2_GPIO_PORT        GPIOC
#define PIN_O_MC2_GPIO_PIN         GPIO_Pin_7

/* O_MCCB1_TRIP (MCCB1 션트트립 코일): O_ELCB = PC6 (CN43). PIN_O_MCCB1_TRIP*/
#define PIN_O_MCCB1_TRIP_GPIO_PORT    GPIOC
#define PIN_O_MCCB1_TRIP_GPIO_PIN     GPIO_Pin_6

/* O_RLED_1CH (LED RED): O_RLED = PE6 (CN38). PIN_O_RLED_1CH*/
#define PIN_O_RLED_1CH_GPIO_PORT      GPIOE
#define PIN_O_RLED_1CH_GPIO_PIN       GPIO_Pin_6

/* O_GLED_1CH (LED GREEN): O_GLED = PE5 (CN38). PIN_O_GLED_1CH*/
#define PIN_O_GLED_1CH_GPIO_PORT      GPIOE
#define PIN_O_GLED_1CH_GPIO_PIN       GPIO_Pin_5

/* O_BLED_1CH (LED BLUE): O_BLED = PE4 (CN38). PIN_O_BLED_1CH*/
#define PIN_O_BLED_1CH_GPIO_PORT      GPIOE
#define PIN_O_BLED_1CH_GPIO_PIN       GPIO_Pin_4

/* TP73 (O_RUN): PD2.  */
#define PIN_O_TP73_GPIO_PORT          GPIOD
#define PIN_O_TP73_GPIO_PIN           GPIO_Pin_2

/* U2_EN (HMI RS485 DIR): U2_EN = PA1. PIN_O_U2_EN*/
#define PIN_O_U2_EN_GPIO_PORT         GPIOA
#define PIN_O_U2_EN_GPIO_PIN          GPIO_Pin_1

/* U3_EN (전력모듈 UART DIR): U3_EN = PE15. PIN_O_U3_EN*/
#define PIN_O_U3_EN_GPIO_PORT         GPIOE
#define PIN_O_U3_EN_GPIO_PIN          GPIO_Pin_15


/*===========================================================================*/
/* ----- ADC : → 2채널 -----
 * [0] = ADC_Ch0  (PA0) 커플러 스위치 저항회로 버튼 감지 ADC
 * [1] = ADC_Ch11 (PC1) 커플러 PT1000 온도 측정            */
/*===========================================================================*/
#define BOARD_ADC_NUM_CH              9
#define BOARD_ADC_COUPLER_SW_CH       ADC_Channel_0    /* PA0 [0] 커플러 스위치 버튼 ADC */
#define BOARD_ADC_PT1000_CH           ADC_Channel_11   /* PC1 [1] 커플러 PT1000 온도    */


/* ---------------------------------------------------------------------------*
 * 편의 매크로
 * ---------------------------------------------------------------------------*/
#define PIN_READ_INPUT(port, pin)    GPIO_ReadInputDataBit(port, pin)
#define PIN_SET(port, pin)           do { (port)->BSRR = (pin); } while(0)
#define PIN_CLEAR(port, pin)         do { (port)->BRR  = (pin); } while(0)

#endif /* __BOARD_PINMAP_H__ */
