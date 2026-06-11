
#define __STM32F105_FUNCTION1_C   //  I/O (LED, RELAY, DOOR, EMG, etc)

#include "stm32f10x.h"
#include "main.h"
#include "stm32f105_function1.h"
#include "sequence.h"
#include "PLC_3_0.h"
#include "HMI_modbus.h"
#include "pt1000.h"
#include "infy_module.h"


// adc 함수는 adc 파일에 만들 것. 1ms 마다 들어옴 

// 콤보 커넥터와 무관하게  전압형 CT 센싱으로 변경함. 2017.05.15
// 10ms 마다 수행하는데 더 빠르게 수행하도록 변경예정
// 평균해서 사용할지 순시값을 사용할지도 결정해야 함.

uint16_t current_sensor_cnt = 0;

float current_sensor_sum = 0;
float current_sensor_avg = 0;

void out_current_read(void) // 버스충전기는 전류 센서 없음. 사용하지 않음. 과전압 과전류 검출은 DC 전력량계를 활용할 수 밖에 없음. 
{
    current_sensor_cnt++;
    //Real_current =       ADCConvertedValue[4]  *  ( 150A / 4095 ), 
    current_sensor_sum += (float)(ADCConvertedValue[4] * 0.03663003F); // 3V 기준

    if(current_sensor_cnt >= 100)
    {
        current_sensor_cnt = 0;
        current_sensor_avg  = current_sensor_sum * 0.01F;
        current_sensor_sum = 0;
    }

    current_sensor = current_sensor_avg;
//    _unsignedint_Iout_real_curr = (uint16_t)current_sensor;
 
//    if(_unsignedint_Iout_real_curr > 200) // 110%
//    {
//       // UI fault 적용 프로토콜 중복으로 할 것. Iout over current 주소로.
////       usRegInputBuf_2[9] |= 0x0080;
//    }
}



//plc 3.0
void Combo_connection(void)
{
    Combo_con_voltage = (float)(ADCConvertedValue[4] * 0.000732421875F );// ((ADC1ConvertedValue[2]*3.3)/4096);

    sum_Combo_con_voltage += Combo_con_voltage;
    ++counter_mode2;
    if (counter_mode2 >= 10)
    {
      ave_Combo_con_voltage_3 = ave_Combo_con_voltage_2;
      ave_Combo_con_voltage_2 = ave_Combo_con_voltage_1;
      ave_Combo_con_voltage_1 = sum_Combo_con_voltage * 0.1F;
      sum_Combo_con_voltage = 0;
      counter_mode2 = 0;
      
      ave_Combo_con_voltage = (ave_Combo_con_voltage_1 + ave_Combo_con_voltage_2 + ave_Combo_con_voltage_3)/3;
    }
    
    if( (ave_Combo_con_voltage >= 1) && (ave_Combo_con_voltage <= 2.1F) )    connection_state = locked;
    else                                                                     connection_state = unlock;  
}

//plc 3.0
bool Combo_connection_check(void)
{
    if((adc_step != DC_12V_MODE) && (connection_state == unlock))
    {
        counter_combo_con_check2 = 0;
        if(counter_combo_con_check++ > 100)       
        {
            counter_combo_con_check = 100; 
            flag_fault_combo_con = 1;
        }
    }
    else if(((adc_step != DC_12V_MODE) && (connection_state == locked)) || (connection_state == default2))
    {
        counter_combo_con_check = 0;
        if(counter_combo_con_check2++ > 100)
        {
            counter_combo_con_check2 = 100;
            flag_fault_combo_con = 0;
        }
    }
    
    //if((adc_step == PWM_6V_MODE) && (connection_state == unlock))  CCS1 은 충전기에서 제어, CCS2는 EV에서 제어
//    if((((SECC_Msg.SECC_Status_2.cpVoltage_12 < 75) && (SECC_Msg.SECC_Status_2.cpVoltage_12 > 45) && (SECC_Msg.SECC_Status_2.CP_Status_0.bit.cpOscillator == TRUE))) && (connection_state == unlock))  
//    {
//        fault_combo_con = 1;
//        return 1;
//    }
    return 0;
}

void ADC_Calculation(void)
{
    
    //ad_cp_rising_voltage = 0.0008056640625F*(ad_cp_rising-1861)*10;
    //ad_cp_falling_voltage = 0.0008056640625F*(ad_cp_falling-1861)*10;
    
    ad_cp_rising_voltage = 0.000732421875F*(ad_cp_rising-2047)*10;
    ad_cp_falling_voltage = 0.000732421875F*(ad_cp_falling-2047)*10;
    
    //Vout = ADCConvertedValue[2] * 0.26862026862026862026862026862027F;
    Vout = ADCConvertedValue[2] * 0.244140625F;
    
    if(pwm_duty != 1)     // PWM Mode
    {
        counter_mode1 = 0;
        
        sum_ad_cp_falling_voltage += ad_cp_falling_voltage;
        sum_ad_cp_rising_voltage += ad_cp_rising_voltage;
        
        if (++counter_mode4 >= 8)
        {
            counter_mode4 = 0;
            //GPIOC->ODR ^= GPIO_Pin_11;
            ave_ad_cp_rising_voltage = sum_ad_cp_rising_voltage * 0.125F;
            sum_ad_cp_rising_voltage = 0;
            if(ave_ad_cp_rising_voltage >= 10.5F)            adc_step_now = PWM_12V_MODE;
            else if(ave_ad_cp_rising_voltage >= 7.5F)        adc_step_now = PWM_9V_MODE;
            else if(ave_ad_cp_rising_voltage >= 4.5F)        adc_step_now = PWM_6V_MODE;
            else	
            {
              adc_step_now = pwm_plus_Volt_error;
              flag_error |= PWM_MODE_Voltage_error; //error
            }
            
            ave_ad_cp_falling_voltage = sum_ad_cp_falling_voltage * 0.125F;
            sum_ad_cp_falling_voltage = 0;
            if(ave_ad_cp_falling_voltage < -10.5F)
            {
            }
            else 	
            {
              adc_step_now = pwm_minus_Volt_error;
              flag_error |= PWM_MODE_falling_Voltage_error; //error
            }
                
            if((adc_step_old_old == adc_step_old) && (adc_step_old == adc_step_now))
            {
              if(adc_step_now == PWM_12V_MODE) 
              {
                adc_step = PWM_12V_MODE;
                adc_step_fault_counter = 0;
              }
              else if(adc_step_now == PWM_9V_MODE) 
              {
                adc_step = PWM_9V_MODE;
                adc_step_fault_counter = 0;
              }
              else if(adc_step_now == PWM_6V_MODE)
              {
                adc_step = PWM_6V_MODE;
                adc_step_fault_counter = 0;
              }
              else if(adc_step_now == pwm_plus_Volt_error) 
              {
                CP_error = 1;
                adc_step_fault_counter = 0;
                adc_step = pwm_plus_Volt_error;
              }
              else if(adc_step_now == pwm_minus_Volt_error) 
              {
                CP_error = 1;
                adc_step_fault_counter = 0;
                adc_step = pwm_minus_Volt_error;
              }
              else
              {
                 if(adc_step_fault_counter < 10)    adc_step_fault_counter++;
                 else    adc_step_fault = 1;
              }
            }
            else 
            {
                 if(adc_step_fault_counter < 10)    adc_step_fault_counter++;
                 else    adc_step_fault = 1;
            }
            adc_step_old_old = adc_step_old;
            adc_step_old = adc_step_now;
            
        }
        else { }
        
        if(adc_step == PWM_12V_MODE)
        {
            if(counter_PWM_12V_MODE < 5)	counter_PWM_12V_MODE++;
        }
        else 	                                counter_PWM_12V_MODE = 0;
        
        
        //pwm6V에서 바로 pwm12V로 가게되면 에러발생
        if(adc_step == PWM_6V_MODE) 	flag_pwm_6v = 1;
        else  if((adc_step == PWM_9V_MODE)  ||  (adc_step == DC_9V_MODE) ||  (adc_step == DC_12V_MODE)) flag_pwm_6v = 0;
        
        if((adc_step == PWM_12V_MODE) &&  (flag_pwm_6v == 1))    error_pwm6to12 = 1;    
    }
    else                // DC Mode
    {
        counter_mode4 = 0;
        ad_cp_falling_voltage = 0;
        ave_ad_cp_falling_voltage = 0;
        sum_ad_cp_rising_voltage += ad_cp_rising_voltage;
    
        if (++counter_mode1 >= 8)
        {
            counter_mode1 = 0;
            ave_ad_cp_rising_voltage = sum_ad_cp_rising_voltage * 0.125F;
            sum_ad_cp_rising_voltage = 0;
            if(ave_ad_cp_rising_voltage >= 10.5F)            adc_step_now = DC_12V_MODE;
            else if(ave_ad_cp_rising_voltage >= 7.5F)        adc_step_now = DC_9V_MODE;
            else if(ave_ad_cp_rising_voltage >= 4.5F)        adc_step_now = DC_6V_MODE;
            else
            {
              adc_step_now = dc_plus_Volt_error;
              flag_error |= DC_MODE_Voltage_error; // ERROR  
            }
            
            if((adc_step_old_old == adc_step_old) && (adc_step_old== adc_step_now))
            { 
               if(adc_step_now == DC_12V_MODE) 
              {
                adc_step_fault_counter = 0;
                adc_step = DC_12V_MODE;
              }
              else if(adc_step_now == DC_9V_MODE) 
              {
                adc_step_fault_counter = 0;
                adc_step = DC_9V_MODE;
              }
              else if(adc_step_now == DC_6V_MODE) 
              {
                adc_step_fault_counter = 0;
                adc_step = DC_6V_MODE;
              }
              else if(adc_step_now == dc_plus_Volt_error) 
              {
                CP_error = 1;
                adc_step_fault_counter = 0;
                adc_step = dc_plus_Volt_error;
              }
              else
              {
                 if(adc_step_fault_counter < 10)    adc_step_fault_counter++;
                 else    adc_step_fault = 1;
              }
            }
            else 
            {
                 if(adc_step_fault_counter < 10)    adc_step_fault_counter++;
                 else    adc_step_fault = 1;
            }
            adc_step_old_old = adc_step_old;
            adc_step_old = adc_step_now;
            
        }
        else { }
    }
    
    if     (adc_step == DC_12V_MODE)       cpline = DC_12V_MODE;
    else if(adc_step == DC_9V_MODE)        cpline = DC_9V_MODE;
    else if(adc_step == PWM_9V_MODE)       cpline = PWM_9V_MODE;
    else if(adc_step == PWM_6V_MODE)       cpline = PWM_6V_MODE;
    else  ;
    
    if ((adc_step == PWM_12V_MODE) && (counter_PWM_12V_MODE > 3) && (flag_pwm_12v_cp_relay == 1)) 
    {
        pwm_duty = 1;
        error_pwm_12v = 1;
    }
}




float iout_check=0., iout_ad=0.,  sum_iout_check=0.;
uint8_t counter_iout=0;
void Iout_AD(void)
{
    iout_check = ADCConvertedValue[3] * 0.04885197F;
    
    sum_iout_check += iout_check;
    ++counter_iout;
    if (counter_iout >= 10)
    {
        counter_iout = 0;
        
        iout_ad = sum_iout_check * 0.1F;
        sum_iout_check = 0;
    }
}



