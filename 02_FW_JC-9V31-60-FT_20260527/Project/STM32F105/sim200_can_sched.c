/**
 * @file sim200_can_sched.c
 * @brief SIM200 CAN2 — 단일 매크로 사이클 Tx 스케줄러.
 */

#include "sim200_can_sched.h"

#include <stddef.h>

static volatile uint32_t s_sim200_millis;

void SIM200_MillisTick(void)
{
    s_sim200_millis++;
}

uint32_t SIM200_GetMillis(void)
{
    return s_sim200_millis;
}

typedef enum {
    SIM200_PHASE_INIT0 = 0,
    SIM200_PHASE_100HZ,
    SIM200_PHASE_GAP,
    SIM200_PHASE_4HZ
} sim200_phase_t;

static sim200_phase_t s_phase;
static uint8_t         s_mux_idx;      /* 현재 테이블 내 다음 송신 인덱스 */
static uint8_t         s_100hz_round; /* 0 .. SIM200_MACRO_100HZ_ROUNDS-1 */
static uint32_t        s_next_tx_ms;

static void sim200_host_tx_request3(uint8_t mux, uint8_t b1, uint8_t b2)
{
    uint8_t p[3];
    SIM200_Pack_Request3(mux, b1, b2, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

/**
 * 호출당 최대 1 Tx. 같은 ms에 여러 단계 전이만 할 때 goto again (guard 상한).
 */
static void sim200_macro_tx_poll(void)
{
    uint32_t now;
    uint8_t  guard;

    guard = 0u;

again:
    now = SIM200_GetMillis();
    if ((int32_t)(now - s_next_tx_ms) < 0) {
        return;
    }
    if (guard >= 12u) {
        return;
    }
    guard++;

    switch (s_phase) {
    case SIM200_PHASE_INIT0: {
        size_t n0 = SIM200_mux_poll_0hz_count;

        if (n0 == 0u) {
            s_phase       = SIM200_PHASE_100HZ;
            s_mux_idx     = 0u;
            s_100hz_round = 0u;
            s_next_tx_ms  = now;
            goto again;
        }
        if (s_mux_idx >= n0) {
            s_phase       = SIM200_PHASE_100HZ;
            s_mux_idx     = 0u;
            s_100hz_round = 0u;
            s_next_tx_ms  = now;
            goto again;
        }
        sim200_host_tx_request3(SIM200_mux_poll_0hz[s_mux_idx], 0u, 0u);
        s_mux_idx++;
        s_next_tx_ms = now + (uint32_t)SIM200_TX_GAP_MS;
        return;
    }

    case SIM200_PHASE_100HZ: {
        size_t n100 = SIM200_mux_poll_100hz_count;

        if (n100 == 0u) {
            if (SIM200_mux_poll_4hz_count > 0u) {
                s_phase      = SIM200_PHASE_GAP;
                s_next_tx_ms = now + (uint32_t)SIM200_MACRO_GAP_MS_BEFORE_4HZ;
            } else {
                /* 100Hz·4Hz 모두 없음 — 빈 루프 방지 */
                s_next_tx_ms = now + 1000u;
            }
            return;
        }
        if (s_mux_idx >= n100) {
            s_mux_idx = 0u;
            s_100hz_round++;
            if (s_100hz_round >= (uint8_t)SIM200_MACRO_100HZ_ROUNDS) {
                s_phase      = SIM200_PHASE_GAP;
                s_next_tx_ms = now + (uint32_t)SIM200_MACRO_GAP_MS_BEFORE_4HZ;
                return;
            }
        }
        sim200_host_tx_request3(SIM200_mux_poll_100hz[s_mux_idx], 0u, 0u);
        s_mux_idx++;
        s_next_tx_ms = now + (uint32_t)SIM200_TX_GAP_MS;
        return;
    }

    case SIM200_PHASE_GAP:
        s_phase      = SIM200_PHASE_4HZ;
        s_mux_idx    = 0u;
        s_next_tx_ms = now;
        goto again;

    case SIM200_PHASE_4HZ: {
        size_t n4 = SIM200_mux_poll_4hz_count;

        if (n4 == 0u) {
            s_phase       = SIM200_PHASE_100HZ;
            s_mux_idx     = 0u;
            s_100hz_round = 0u;
            s_next_tx_ms  = now;
            goto again;
        }
        if (s_mux_idx >= n4) {
            s_phase       = SIM200_PHASE_100HZ;
            s_mux_idx     = 0u;
            s_100hz_round = 0u;
            s_next_tx_ms  = now;
            goto again;
        }
        sim200_host_tx_request3(SIM200_mux_poll_4hz[s_mux_idx], 0u, 0u);
        s_mux_idx++;
        s_next_tx_ms = now + (uint32_t)SIM200_TX_GAP_MS;
        return;
    }

    default:
        s_phase = SIM200_PHASE_INIT0;
        s_mux_idx = 0u;
        s_100hz_round = 0u;
        s_next_tx_ms = now;
        return;
    }
}

void CAN2_RxISR_Handler(uint8_t dlc, const uint8_t *rx_data)
{
    SIM200_Protocol_ProcessRx(dlc, rx_data);
}

void SIM200_ProcessTxPoll(void)
{
    sim200_macro_tx_poll();
}

void SIM200_Init(void)
{
    SIM200_Protocol_Init();

    s_phase       = SIM200_PHASE_INIT0;
    s_mux_idx     = 0u;
    s_100hz_round = 0u;
    s_next_tx_ms  = SIM200_GetMillis();
}

void SIM200_Process(void)
{
    sim200_macro_tx_poll();
}
