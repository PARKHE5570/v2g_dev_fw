
#define __MAIN_H

#include "stdint.h"
#include "math.h"
#include "Board_Pin_Config.h"

/* Compiler-independent, fixed size types */
typedef signed char           INT8 ; /* Signed 8-bit integer    */
typedef unsigned char        UINT8 ; /* Unsigned 8-bit integer  */
typedef signed short int     INT16 ; /* Signed 16-bit integer   */
typedef unsigned short int  UINT16 ; /* Unsigned 16-bit integer */
typedef long int             INT32 ; /* Signed 32-bit integer   */
typedef unsigned long       UINT32 ; /* Unsigned 32-bit integer   */
typedef int64_t              INT64 ;
typedef uint64_t            UINT64 ;
typedef float               FLOAT32; /* 32-bit IEEE single precision */
typedef double              FLOAT64; /* 64-bit IEEE double precision */

#define VOLTAGE_SCALE  (1000/4095.)  // type Casting 주의
#define OVER_VOLTAGE_SET_VALUE (990) // V
#define OVER_CURRENT_SET_VALUE (80) // A 
#define COUPLER_OVERTEMPRATURE_SET_VALUE   (100)  // 폴트 과온도
#define COUPLER_TEMPRATURE_CONTROL_HIGH_SET_VALUE   (80)   // 전류 제어 온도 기준치
#define COUPLER_TEMPRATURE_CONTROL_MIDDLE_SET_VALUE (75)   // 전류 제어 온도 기준치
#define COUPLER_TEMPRATURE_CONTROL_LOW_SET_VALUE    (70)   // 전류 복구 온도 기준치


#define MAX_GCNT (35)   

#define TP73_ON         GPIOB->BSRR = GPIO_Pin_7      // PIN98 : TP93
#define TP73_OFF        GPIOB->BRR  = GPIO_Pin_7
#define TP73_TOG        GPIOB->ODR ^= GPIO_Pin_7

#define TP67_ON         GPIOB->BSRR = GPIO_Pin_5      // PIN91 : TP67
#define TP67_OFF        GPIOB->BRR  = GPIO_Pin_5
#define TP67_TOG        GPIOB->ODR ^= GPIO_Pin_5

#define U2_Enable       GPIOC->BSRR = GPIO_Pin_13      // PIN7 : U2_EN
#define U2_Disable      GPIOC->BRR  = GPIO_Pin_13

#define U3_Enable       GPIOE->BSRR = GPIO_Pin_15     // PIN46 : U3_EN
#define U3_Disable      GPIOE->BRR  = GPIO_Pin_15




typedef struct
{
  UINT16 occur_day;
  UINT8 occur_hour;
  UINT8 occur_min;
  UINT8 occur_sec;
  
  short  cpVoltage;
  UINT8  cpOscillator;
  UINT8  started_soc;
  UINT8  finished_soc;
  UINT8  evccmac0;
  UINT8  evccmac1;
  UINT8  evccmac2;
  UINT8  evccmac3;
  UINT8  evccmac4;
  UINT8  evccmac5;
  UINT16 targetV;
  UINT16 demendI;
  UINT16 present_V;
  UINT16 present_I;
  float dc_electric_energy_cpy;
  
}__cpfault_chager_status;


#ifdef _MAIN_C

bool _10ms_flag, _50ms_flag, _100ms_flag;

float           DCVout_1CH = 0, DCIout_1CH = 0, DCWatt_1CH = 0;

/* TIMER ------------------------------------------------------------------------------------------------*/
uint16_t         timer_1ms = 0, timer_5ms = 0,  timer_10ms = 0, timer_50ms = 0, timer_100ms = 0, timer_200ms = 0, timer_500ms = 0, timer_1s;


/* UART ------------------------------------------------------------------------------------------------*/
//Protocol_Joas_20200512_Ver_2_0
uint16_t	usRegInputBuf_2_1[60] = {0,}, usRegHoldingBuf_2_1[50];
uint16_t    usRegHoldingBuf_Test[10];

/* CRC16 implementation acording to CCITT standards */

uint16_t crc16tab[256]= {
  0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
  0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
  0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
  0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
  0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
  0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
  0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
  0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
  0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
  0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
  0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
  0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
  0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
  0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
  0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
  0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
  0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
  0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
  0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
  0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
  0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
  0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
  0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
  0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
  0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
  0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
  0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
  0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
  0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
  0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
  0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
  0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

/* sequence ------------------------------------------------------------------------------------------------*/

uint16_t        fault_timer_1CH=0;

uint16_t        sequence_step_1CH=0;

uint16_t        cnt_ry1_w=0, cnt_ry2_w=0, timeout_timer_1CH=0;

uint8_t         stop_code_1CH=0;

uint8_t         Combo_start_button_1CH=0 ;
uint8_t         Combo_stop_button_1CH=0;


float           Iout_G1=0.,Iout_G2=0.;
uint16_t 		SOC_1CH, SOC_old, start_SOC, end_SOC;;



/* ADC -------------------------------------------------------------------------------------------------- */
float  Vout_G1=0,Vout_G2=0;

volatile uint16_t ADCConvertedValue[BOARD_ADC_NUM_CH]={0,}; // 260403 박형은 수정

//========================================20171116 DM add
uint8_t          Uart3_Trnsmt_count=0;
uint8_t          Uart1_Trnsmt_count=0;
//---------------------------------->
uint8_t         Channel1_RUN = 0;

uint16_t gCNT[MAX_GCNT];

uint8_t my_stand_standby_1CH = 1;


uint16_t set_total_power_fix;

uint16_t charger_out_maximum_current_1CH = 0;
uint16_t        cnt_ry1_w, cnt_ry2_w;
uint16_t        cnt_ry1_o, cnt_ry2_o;

char term_rx_msg_buffer_copy[29];

uint8_t control_pilot_1CH = 0;



uint8_t power_module_can_communication_fail_1CH = 0, power_module_can_communication_fail_2CH = 0;


uint8_t self_check_start = false;
__cpfault_chager_status cp_error_chager_status[256];
uint8_t cp_fault_status_copy_flag = 0;
uint8_t fault_memory_cnt = 0;
#else

extern uint8_t cp_fault_status_copy_flag;
extern __cpfault_chager_status cp_error_chager_status[256];
extern uint8_t fault_memory_cnt;

extern uint8_t self_check_start;


extern uint8_t power_module_can_communication_fail_1CH, power_module_can_communication_fail_2CH;


extern uint8_t control_pilot_1CH;
extern char term_rx_msg_buffer_copy[29];

extern uint16_t charger_out_maximum_current_1CH;











extern uint16_t set_total_power_fix;
extern uint16_t total_dispenser_number;

extern uint8_t my_stand_standby_1CH;

extern uint8_t Vbat_over_check_flag; //default



extern uint8_t Combo_stop_button_1CH;
extern uint8_t Combo_start_button_1CH ;



extern uint16_t timeout_timer_1CH;
extern uint16_t gCNT[MAX_GCNT];


extern uint8_t Channel1_RUN;
extern float           ACVout, ACIout, ACWatt, DCVout_1CH, DCVout_2CH, DCIout_1CH, DCIout_2CH, DCWatt_1CH, DCWatt_2CH;

/*--CAN --*/
extern uint8_t           usart2_tx_ok;



extern float            delta_Icompensate, V_out, V_in_old, V_in, V_out_old, iout_ad;


/*--Sequence --*/
extern uint16_t        sequence_step_1CH;
extern uint8_t          count_can_sgs, can_combo_error, can_sgs_error, can_chademo_error, count_mc3_check;
extern uint16_t        cnt_ry1_w, cnt_ry2_w;
extern uint16_t        cnt_ry1_o, cnt_ry2_o;



extern uint8_t         stop_code_1CH;

extern uint16_t        fault_timer_1CH,fault_timer_2CH;

extern uint8_t         BlinkTmr, count_rcd_check;



extern float           Iout_G1,Iout_G2;
extern uint16_t SOC_1CH, SOC_old, start_SOC, end_SOC;

extern float           delta_Iout_ref_temp;



/*---Uart---*/
//Protocol_Joas_20200512_Ver_2_0
extern uint16_t	       usRegInputBuf_2_1[60], usRegHoldingBuf_2_1[50];
extern uint16_t        usRegHoldingBuf_Test[10];




/* ADC -------------------------------------------------------------------------------------------------- */
extern float  Vout_G1,Vout_G2;



extern volatile uint16_t ADCConvertedValue[BOARD_ADC_NUM_CH];          

//================================20171116 DM add





extern uint8_t      txing_1, Uart3_Trnsmt_count, Uart1_Trnsmt_count;

//================================ PM control





extern uint16_t         timer_1ms, timer_5ms,  timer_10ms, timer_50ms, timer_100ms, timer_200ms, timer_500ms, timer_1s;

#endif




