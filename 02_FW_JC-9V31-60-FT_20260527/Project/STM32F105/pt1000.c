#include "pt1000.h"
#include "stdint.h"
#include "stm32f10x.h"
#include "main.h"

// -70도 부터 180도까지 5도 단위의 ADC 값 테이블 (분압저항 1k옴, VCC 3.0V, 12bit ADC 기준 반올림 적용)
#define PT1000_MIN_TEMP -70
#define PT1000_TEMP_STEP 5
#define PT1000_TABLE_SIZE 51 // -70 ~ 180도 (총 51개 구간)
int16_t coupler_temperature_P_1CH;
int16_t coupler_temperature_P_2CH;

// 구조체 인스턴스
PT1000_TypeDef PT1000_1CH;

// ADC 측정 및 누적용 변수
uint16_t PT1000_AD_1CH;
uint32_t sum_PT1000_AD_1CH; // 200번 누적을 위해 충분한 크기(32bit) 할당
uint16_t sum_counter_1CH;

// 최종 온도를 저장하는 전역 변수 (영하 및 127도 이상 측정을 위해 반드시 int16_t)
int16_t coupler_temperature_P_1CH;
int16_t coupler_temperature_P_2CH;

const uint16_t pt1000_adc_table[PT1000_TABLE_SIZE] = {
    1719, // -70도 (Rt: 723.35옴)
    1746, // -65도 (Rt: 743.33옴)
    1773, // -60도 (Rt: 763.28옴)
    1799, // -55도 (Rt: 783.19옴)
    1825, // -50도 (Rt: 803.06옴)
    1850, // -45도 (Rt: 822.90옴)
    1874, // -40도 (Rt: 842.71옴)
    1899, // -35도 (Rt: 862.48옴)
    1923, // -30도 (Rt: 882.22옴)
    1946, // -25도 (Rt: 901.92옴)
    1969, // -20도 (Rt: 921.60옴)
    1991, // -15도 (Rt: 941.24옴)
    2013, // -10도 (Rt: 960.86옴)
    2035, // -5도  (Rt: 980.44옴)
    2048, // 0도   (Rt: 1000.0옴)
    2067, // 5도   (Rt: 1019.5옴)
    2086, // 10도  (Rt: 1039.0옴)
    2106, // 15도  (Rt: 1058.5옴)
    2124, // 20도  (Rt: 1077.9옴)
    2143, // 25도  (Rt: 1097.3옴)
    2160, // 30도  (Rt: 1116.7옴)
    2178, // 35도  (Rt: 1136.1옴)
    2195, // 40도  (Rt: 1155.4옴)
    2212, // 45도  (Rt: 1174.7옴)
    2229, // 50도  (Rt: 1194.0옴)
    2245, // 55도  (Rt: 1213.2옴)
    2261, // 60도  (Rt: 1232.4옴)
    2277, // 65도  (Rt: 1251.6옴)
    2292, // 70도  (Rt: 1270.8옴)
    2307, // 75도  (Rt: 1289.9옴)
    2322, // 80도  (Rt: 1309.0옴)
    2337, // 85도  (Rt: 1328.0옴)
    2351, // 90도  (Rt: 1347.1옴)
    2365, // 95도  (Rt: 1366.1옴)
    2379, // 100도 (Rt: 1385.1옴)
    2392, // 105도 (Rt: 1404.0옴)
    2406, // 110도 (Rt: 1422.9옴)
    2419, // 115도 (Rt: 1441.8옴)
    2432, // 120도 (Rt: 1460.7옴)
    2444, // 125도 (Rt: 1479.5옴)
    2456, // 130도 (Rt: 1498.3옴)
    2469, // 135도 (Rt: 1517.1옴)
    2480, // 140도 (Rt: 1535.8옴)
    2492, // 145도 (Rt: 1554.6옴)
    2503, // 150도 (Rt: 1573.3옴)
    2515, // 155도 (Rt: 1591.9옴)
    2526, // 160도 (Rt: 1610.5옴)
    2536, // 165도 (Rt: 1629.1옴)
    2547, // 170도 (Rt: 1647.7옴)
    2558, // 175도 (Rt: 1666.3옴)
    2568  // 180도 (Rt: 1684.8옴)
};

// 선형 보간법을 이용해 현재 ADC 값으로 정확한 온도를 계산하는 함수
float Get_Temperature_From_LUT(uint16_t adc_val) 
{
    // 1. 측정 범위 이탈 방지 (예외 처리)
    if (adc_val <= pt1000_adc_table[0]) return (float)PT1000_MIN_TEMP;
    if (adc_val >= pt1000_adc_table[PT1000_TABLE_SIZE - 1]) return (float)(PT1000_MIN_TEMP + (PT1000_TABLE_SIZE - 1) * PT1000_TEMP_STEP);

    // 2. 현재 ADC 값이 속한 구간 찾기 및 보간
    for (int i = 0; i < PT1000_TABLE_SIZE - 1; i++) 
    {
        if (adc_val >= pt1000_adc_table[i] && adc_val <= pt1000_adc_table[i+1]) 
        {
            float temp_low = PT1000_MIN_TEMP + (i * PT1000_TEMP_STEP);
            float temp_high = temp_low + PT1000_TEMP_STEP;
            uint16_t adc_low = pt1000_adc_table[i];
            uint16_t adc_high = pt1000_adc_table[i+1];

            // 보간 공식 (구간이 좁아졌으므로 오차가 더욱 감소함)
            return temp_low + ((float)(adc_val - adc_low) * (temp_high - temp_low) / (float)(adc_high - adc_low));
        }
    }
    return 0.0f; 
}

// 180도까지 측정하기 위해 반드시 int16_t를 사용해야 함
int16_t Get_coupler_temperature_1CH(void)
{
  PT1000_AD_1CH = ADCConvertedValue[5]; 
  
  sum_PT1000_AD_1CH += PT1000_AD_1CH;
  
  if (++sum_counter_1CH >= 200) 
  {
    sum_counter_1CH = 0;   
    PT1000_1CH.ave_AD = (float)(sum_PT1000_AD_1CH * 0.005F); 
    sum_PT1000_AD_1CH = 0;
    
    // 5도 단위 룩업 테이블 보간 함수 호출
    PT1000_1CH.current_Temperature = Get_Temperature_From_LUT((uint16_t)PT1000_1CH.ave_AD);
    
    // 전역 변수 업데이트 (int16_t)
    coupler_temperature_P_1CH = (int16_t)PT1000_1CH.current_Temperature;
  }
  
  return (int16_t)PT1000_1CH.current_Temperature + 1; 
}