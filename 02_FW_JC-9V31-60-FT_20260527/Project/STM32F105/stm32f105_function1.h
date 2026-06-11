#define __STM32F105_FUNCTION1_H

#include <stdbool.h>

typedef enum 
{
    default1 = 0,
    DC_12V_MODE,
    DC_9V_MODE,
    DC_6V_MODE,
    PWM_12V_MODE,
    PWM_9V_MODE,
    PWM_6V_MODE, 
    pwm_plus_Volt_error,
    pwm_minus_Volt_error,
    dc_plus_Volt_error
}_adc_step;

typedef enum
{
  default2 = 0,
   unlock,
    locked
}_connection_state;








#ifdef __STM32F105_FUNCTION1_C






void ADC_Calculation(void);







void Combo_connection(void);
uint8_t DM_CS_u8(uint8_t *nData, int n);
bool Combo_connection_check(void);

void Iout_AD(void);
void UV_LED_Service(void);

_adc_step adc_step, adc_step_now, adc_step_old, adc_step_old_old, cpline;
_connection_state connection_state;


#else



extern void ADC_Calculation(void);







extern void Combo_connection(void);
extern uint8_t DM_CS_u8(uint8_t *nData, int n);
extern bool Combo_connection_check(void);

extern void Iout_AD(void);
extern void UV_LED_Service(void);


extern _adc_step adc_step, adc_step_now, adc_step_old, adc_step_old_old, cpline;
extern _connection_state connection_state;


#endif