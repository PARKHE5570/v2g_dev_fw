/**
 * @file sim200_can_sched.h
 * @brief SIM200 CAN2 — Init/Process 스케줄러, Rx ISR 진입점.
 *
 * 수신 데이터 구조체는 sim200_protocol.h 의
 *   sim200_data_0hz / sim200_data_100hz / sim200_data_4hz
 * 를 사용합니다.
 *
 * Tx: 단일 매크로 사이클 — Init에서 0Hz 테이블 1회 순환 후,
 *     (100Hz 테이블 × SIM200_MACRO_100HZ_ROUNDS) → GAP → (4Hz 1회) 반복.
 *     연속 Request 간격은 항상 SIM200_TX_GAP_MS (단, 4Hz 직전 GAP은
 *     SIM200_MACRO_GAP_MS_BEFORE_4HZ).
 *
 * 시간 베이스: 모듈 내부 `SIM200_GetMillis()` — **1ms ISR에서만** `SIM200_MillisTick()` 호출.
 */

#ifndef SIM200_CAN_SCHED_H
#define SIM200_CAN_SCHED_H

#include <stdint.h>

#include "sim200_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 연속 MUX Request 간격 (100Hz/4Hz 라운드 내부). */
#ifndef SIM200_TX_GAP_MS
#define SIM200_TX_GAP_MS   10u
#endif

/** 매크로 사이클 내 100Hz 테이블(배열 1회) 반복 횟수. */
#ifndef SIM200_MACRO_100HZ_ROUNDS
#define SIM200_MACRO_100HZ_ROUNDS  3u
#endif

/** 100Hz 마지막 라운드 종료 후 4Hz 시작까지 대기 (4Hz 갱신 여유). */
#ifndef SIM200_MACRO_GAP_MS_BEFORE_4HZ
#define SIM200_MACRO_GAP_MS_BEFORE_4HZ  10u
#endif

/** 1ms마다 한 번(ISR 권장). SIM200 전용 ms 카운터 증가. */
void SIM200_MillisTick(void);

/** SIM200 Tx 스케줄 간격·데드라인에 사용하는 경과 ms. */
uint32_t SIM200_GetMillis(void);

void SIM200_Init(void);
void SIM200_Process(void);

/** 매크로 Tx 폴링 — while(1) 매 주기 호출 권장. */
void SIM200_ProcessTxPoll(void);

/**
 * CAN2 RX에서 호출. ExtId == SIM200_CAN_ID_SIM200_TX 필터 후
 * SIM200_Protocol_ProcessRx() 로 위임.
 */
void CAN2_RxISR_Handler(uint8_t dlc, const uint8_t *rx_data);

#ifdef __cplusplus
}
#endif

#endif /* SIM200_CAN_SCHED_H */
