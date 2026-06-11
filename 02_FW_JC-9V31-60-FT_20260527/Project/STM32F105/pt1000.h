#ifndef __PT1000_H
#define __PT1000_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ====================================================================
 * PT1000 Sensor Data Structure
 * ==================================================================== */
typedef struct {
	float ave_AD;// 200회 누적 평균 ADC 값
	float current_Temperature;// 선형 보간법으로 계산된 현재 온도 (소수점 포함)
    
    // 아래 변수들은 기존 수식 계산법에서 사용되었으나 룩업 테이블 방식에서는 
    // 불필요하여 주석 처리하였습니다. 필요시 주석을 해제하세요.
    // float Volt;       
    // float Rt;         
} PT1000_TypeDef;


/* ====================================================================
 * Global Variables (extern 선언)
 * ==================================================================== */
// 구조체 인스턴스
extern PT1000_TypeDef PT1000_1CH;

// ADC 측정 및 누적용 변수
extern uint16_t PT1000_AD_1CH;
extern uint32_t sum_PT1000_AD_1CH; // 200번 누적을 위해 충분한 크기(32bit) 할당
extern uint16_t sum_counter_1CH;

// 최종 온도를 저장하는 전역 변수 (영하 및 127도 이상 측정을 위해 반드시 int16_t)
extern int16_t coupler_temperature_P_1CH;
extern int16_t coupler_temperature_P_2CH;

/* ====================================================================
 * Function Prototypes
 * ==================================================================== */
// 룩업 테이블을 통한 온도 변환 함수 (내부 호출용 및 테스트용)
float Get_Temperature_From_LUT(uint16_t adc_val);

// 메인 루프 또는 타이머 인터럽트에서 주기적으로 호출할 온도 측정 함수
int16_t Get_coupler_temperature_1CH(void);

#ifdef __cplusplus
}
#endif

#endif /* __PT1000_H */