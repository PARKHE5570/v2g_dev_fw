/**
 * @file sim200_protocol.h
 * @brief Sensata SIM200 CAN protocol — HW-independent declarations (SIM200 CAN spec).
 */

#ifndef SIM200_PROTOCOL_H
#define SIM200_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/* Default 29-bit extended CAN identifiers (configurable in SIM200 setup mode) */
/* -------------------------------------------------------------------------- */
#define SIM200_CAN_ID_HOST_TX     ((uint32_t)0x0A100101u)  /* Host / ECU -> SIM200 */
#define SIM200_CAN_ID_SIM200_TX   ((uint32_t)0x0A100100u)  /* SIM200 -> Host / ECU */

/* -------------------------------------------------------------------------- */
/* Request / command multiplexers (Byte 0 of host request, DLC = 3 typical)    */
/* -------------------------------------------------------------------------- */
/* Electrical isolation — 문서 Mux High=14, Responses 0xE0..0xE7 */
#define SIM200_MUX_GET_ISO_STATE           0xE0u
#define SIM200_MUX_GET_ISO_RESISTANCES     0xE1u
#define SIM200_MUX_GET_SYS_Y_CAP           0xE2u  /**< 문서: Get system Y-capacitances / System capacitances */
#define SIM200_MUX_GET_HV_RAIL             0xE3u
#define SIM200_MUX_GET_BATTERY_VOLTAGE     0xE4u
#define SIM200_MUX_GET_ERROR_FLAGS         0xE5u
#define SIM200_MUX_GET_DYN_E_ISO           0xE6u
#define SIM200_MUX_GET_DYN_CAP_ENERGY      0xE7u

/*
 * 고해상도 측정 (Byte0 = Mux; Host DLC3, SIM200 DLC8 — sensata_sim200_can_protocol.md).
 * 0x68..0x6E 는 프로토콜에 없음. Setup Bootloader 등에서 Byte0=0x60 이 다른 의미로 쓰일 수 있음.
 */
#define SIM200_MUX_MEAS_VNEG_HR            0x60u  /**< v_neg_hi_res */
#define SIM200_MUX_MEAS_VPOS_HR            0x61u  /**< v_pos_hi_res */
#define SIM200_MUX_MEAS_VEXC_HR            0x62u  /**< v_exc_hi_res */
#define SIM200_MUX_MEAS_VBATT_HR           0x63u  /**< v_batt_hi_res */
#define SIM200_MUX_MEAS_VPOWER_HR          0x64u  /**< v_power_hi_res */
#define SIM200_MUX_MEAS_TEMPERATURE        0x65u  /**< temp_hi_res */
#define SIM200_MUX_MEAS_VCAL_NEG           0x66u  /**< vcal_neg_hi_res */
#define SIM200_MUX_MEAS_VCAL_POS           0x67u  /**< vcal_pos_hi_res */
#define SIM200_MUX_MEAS_UPTIME_COUNTER     0x6Fu  /**< uptime_counter */

/* Manufacturer — 문서 Requests/Responses Mux 0x01..0x0C */
#define SIM200_MUX_MFG_PART_NAME_0         0x01u  /**< Part name seg.0 — 7 ASCII @ data[1..7] typical */
#define SIM200_MUX_MFG_PART_NAME_1         0x02u
#define SIM200_MUX_MFG_PART_NAME_2         0x03u
#define SIM200_MUX_MFG_PART_NAME_3         0x04u
#define SIM200_MUX_MFG_FW_VERSION          0x05u  /**< FW ver: data[1..4] 매뉴얼 필드 정의 */
#define SIM200_MUX_MFG_VERSION_INFO_0      0x06u  /**< 7 ASCII @ data[1..7] typical */
#define SIM200_MUX_MFG_VERSION_INFO_1      0x07u
#define SIM200_MUX_MFG_VERSION_INFO_2      0x08u
#define SIM200_MUX_MFG_SERIAL_0            0x09u  /**< Serial chunk; uint32 BE @ data[1..4] typical */
#define SIM200_MUX_MFG_SERIAL_1            0x0Au
#define SIM200_MUX_MFG_SERIAL_2            0x0Bu
#define SIM200_MUX_MFG_SERIAL_3            0x0Cu

#define SIM200_MFG_TEXT7_SIZE              8u     /**< 7 wire bytes + NUL */

/* Configuration — 문서 Mux High=15 */
#define SIM200_MUX_GET_PROG_MAX_VOLTAGE    0xF0u
#define SIM200_MUX_GET_ISO_THRESHOLD_CFG   0xF1u  /**< 응답: iso_fault_thr, iso_warning_thr (문서명 iso_warning_thr) */

/* Commands — 문서 Command messages / Requests */
#define SIM200_MUX_CMD_RESET               0xC1u
#define SIM200_MUX_CMD_DISABLE             0xC2u
#define SIM200_MUX_CMD_SETUP_MODE          0xC3u

#define SIM200_RESET_BYTE1                 0x01u
#define SIM200_RESET_BYTE2                 0x23u

#define SIM200_DISABLE_BYTE1               0xECu
#define SIM200_DISABLE_LOCK_ZERO           0x00u
#define SIM200_DISABLE_LOCK_LOW            0x01u
#define SIM200_DISABLE_LOCK_HIGH           0x02u

#define SIM200_SETUP_BYTE1                 0x28u
#define SIM200_SETUP_BYTE2                 0xD5u

/* Setup mode challenge XOR key (7 bytes): "SNDSIMB" */
#define SIM200_SETUP_CHALLENGE_KEY_LEN     7u

/* ACK / NAK tokens (3 bytes, big-endian friendly compare) */
#define SIM200_ACK_B0                      0x41u
#define SIM200_ACK_B1                      0x43u
#define SIM200_ACK_B2                      0x4Bu
#define SIM200_NAK_B0                      0x4Eu
#define SIM200_NAK_B1                      0x41u
#define SIM200_NAK_B2                      0x4Bu

/* -------------------------------------------------------------------------- */
/* Disable command sub-mode (Byte 2 of 0xC2 frame) */
/* -------------------------------------------------------------------------- */
typedef enum {
    SIM200_DISABLE_EXCITATION_ZERO = 0,
    SIM200_DISABLE_EXCITATION_LOW  = 1,
    SIM200_DISABLE_EXCITATION_HIGH = 2
} SIM200_DisableExcitation_t;

/**
 * 0xE0–0xE7 응답 Byte1 status (동일 포맷, 마지막으로 처리된 MUX가 덮어씀).
 * sensata_sim200_can_protocol.md — 'Status' Byte Signals.
 * GCC 기준 비트필드: iso_0=LSB(Bit0), … se=MSB(Bit7).
 */
typedef union SIM200_Status {
    uint8_t raw;
    struct {
        uint8_t iso_0 : 1; /**< Bit0 ISO_0 — isolation OK(0)/fault 조합은 iso_0+iso_1 */
        uint8_t iso_1 : 1; /**< Bit1 ISO_1 */
        uint8_t lv : 1;    /**< Bit2 LV — 1: LV mode (배터리 < 15 V) */
        uint8_t hv : 1;    /**< Bit3 HV — prog_max 초과 측정 이력 */
        uint8_t dd : 1;    /**< Bit4 DD — 1: disabled, not monitoring */
        uint8_t hu : 1;    /**< Bit5 HU — 불확실도 > 10% */
        uint8_t fs : 1;    /**< Bit6 FS — 기능안전 구성 비호환 */
        uint8_t se : 1;    /**< Bit7 SE — 시스템 에러(err_flags 등) */
    } b;
} SIM200_Status_t;

/**
 * 0xE5 err_flags (BE uint16, 로컬 uint16 bit0=와이어 LSB).
 * sensata_sim200_can_protocol.md — 'err_flags' Low/High Byte Signals.
 * degrdn_diag: 고바이트 Bit3–7, 0=비활성, 1–31=진단 코드 표 참조.
 */
typedef union SIM200_ErrFlags {
    uint16_t raw;
    struct {
        uint16_t err_fso : 1;         /**< Low Bit0 */
        uint16_t err_temp : 1;
        uint16_t err_v_power : 1;
        uint16_t err_v_exc : 1;
        uint16_t err_vx_reversed : 1;
        uint16_t err_v_ch : 1;
        uint16_t err_v_pos : 1;
        uint16_t err_v_neg : 1;
        uint16_t err_can_ovrun : 1; /**< High Bit0 → uint16 Bit8 */
        uint16_t err_signal : 1;
        uint16_t no_calib : 1;
        uint16_t degrdn_diag : 5;   /**< 진단 코드 (0=정상/비저하) */
    } b;
} SIM200_ErrFlags_t;

/* -------------------------------------------------------------------------- */
/* Parsed SIM200 data (updated by SIM200_Protocol_ProcessRx / CAN2_Rx path)   */
/* 갱신 주기: 0 Hz(Init·설정), 10 ms(≈100 Hz), 250 ms(4 Hz) — 스케줄은 앱 테이블 참고 */
/* -------------------------------------------------------------------------- */

/** 0 Hz: 설정·MFG·명령 응답·Setup 플래그 등 */
typedef struct {
    uint16_t prog_max_voltage;   /**< 0xF0 — 단위는 매뉴얼 기준 */
    uint8_t  have_f0;

    uint16_t iso_fault_thr;      /**< 0xF1 예시 */
    uint16_t iso_warn_thr;
    uint8_t  have_f1;

    uint8_t  have_disable_ack;
    uint8_t  disable_cmd_ack;

    uint8_t  is_setup_mode;
    uint8_t  last_setup_challenge_ok;

    char     part_name[4][SIM200_MFG_TEXT7_SIZE];
    uint8_t  have_part_name[4];

    uint8_t  fw_ver_major;
    uint8_t  fw_ver_minor;
    uint8_t  fw_ver_patch;
    uint8_t  fw_ver_pre;
    uint8_t  have_fw_ver;

    char     version_info[3][SIM200_MFG_TEXT7_SIZE];
    uint8_t  have_version_info[3];

    uint32_t serial_u32[4];
    uint8_t  have_serial[4];
} SIM200_Data_0hz_t;

/** 10 ms 주기 폴링: 고해상도 0x60-0x67, uptime 0x6F */
typedef struct {
    uint8_t  meas_scratch[8];

    int32_t  v_neg_uV;
    uint8_t  have_60;
    int32_t  v_pos_uV;
    uint8_t  have_61;
    uint32_t uptime_s;
    uint8_t  have_6f;
} SIM200_Data_100hz_t;

/** 250 ms 주기 폴링: 절연 측정 그룹 0xE0..0xE7 */
typedef struct {
    SIM200_Status_t  sim200_status; /**< 0xE0–0xE7 공통 status(Byte1), 각 MUX 수신 시 갱신 */
    SIM200_ErrFlags_t err_flags;    /**< 0xE5 payload err_flags만 해당; .raw / .b */

    uint16_t e_iso;
    uint8_t  e_iso_u;
    uint16_t e_energy;
    uint8_t  e_energy_u;

    uint16_t iso_res_pos;
    uint8_t  iso_res_pos_u;
    uint16_t iso_res_neg;
    uint8_t  iso_res_neg_u;

    uint16_t iso_cap_pos;
    uint8_t  iso_cap_pos_u;
    uint16_t iso_cap_neg;
    uint8_t  iso_cap_neg_u;

    int16_t  v_pos;
    uint8_t  v_pos_u;
    int16_t  v_neg;
    uint8_t  v_neg_u;

    int16_t  v_batt;
    uint8_t  v_batt_u;
    int16_t  max_voltage;

    uint16_t iso_min_res;
    uint8_t  iso_min_res_u;
    uint16_t e_iso_dyn;
    uint8_t  e_iso_dyn_u;

    uint16_t iso_total_cap;
    uint8_t  iso_total_cap_u;
    uint16_t e_energy_dyn;
    uint8_t  e_energy_dyn_u;

    uint8_t  have_e0;
    uint8_t  have_e1;
    uint8_t  have_e2;
    uint8_t  have_e3;
    uint8_t  have_e4;
    uint8_t  have_e5;
    uint8_t  have_e6;
    uint8_t  have_e7;
} SIM200_Data_4hz_t;

extern SIM200_Data_0hz_t   sim200_data_0hz;
extern SIM200_Data_100hz_t sim200_data_100hz;
extern SIM200_Data_4hz_t   sim200_data_4hz;

/* -------------------------------------------------------------------------- */
/* Tx 폴링용 MUX(Byte0) 순서 — 정의는 sim200_protocol.c (순서·MUX만 수정)       */
/* -------------------------------------------------------------------------- */
extern const uint8_t SIM200_mux_poll_0hz[];
extern const size_t  SIM200_mux_poll_0hz_count;
extern const uint8_t SIM200_mux_poll_100hz[];
extern const size_t  SIM200_mux_poll_100hz_count;
extern const uint8_t SIM200_mux_poll_4hz[];
extern const size_t  SIM200_mux_poll_4hz_count;

/* -------------------------------------------------------------------------- */
/* Port hook: implemented in sim200_port.c (SPL CAN transmit Host -> SIM200)   */
/* DLC 0..8; payload copied from first `dlc` bytes, rest ignored by CAN HW. */
/* Does not wait on CAN_TransmitStatus (bus TX complete). */
/* -------------------------------------------------------------------------- */
void SIM200_Port_TransmitHostFrame(uint8_t dlc, const uint8_t *payload);

/**
 * Poll SIM200 CAN peripheral RX FIFO0 (non-ISR path).
 * Accepts only extended ID SIM200_CAN_ID_SIM200_TX; forwards to SIM200_Protocol_ProcessRx().
 * Peripheral: define SIM200_CANx in sim200_port.c (default CAN2 for this board).
 */
void SIM200_Port_PollRx(void);

/* -------------------------------------------------------------------------- */
/* Lifecycle */
/* -------------------------------------------------------------------------- */
void SIM200_Protocol_Init(void);

/**
 * Parse one SIM200 -> Host frame (after verifying IDE extended + expected ExtId).
 * Handles mux (Byte 0), electrical isolation, setup challenge/ACK/NAK, command ACKs.
 */
void SIM200_Protocol_ProcessRx(uint8_t dlc, const uint8_t *data);

/* -------------------------------------------------------------------------- */
/* Host -> SIM200: measurement requests (DLC 3) */
/* -------------------------------------------------------------------------- */
void SIM200_Request_GetIsolationState(void);
void SIM200_Request_GetIsolationResistances(void);
void SIM200_Request_GetSystemYCapacitances(void);
void SIM200_Request_GetHVRailMeasurements(void);
void SIM200_Request_GetBatteryVoltage(void);
void SIM200_Request_GetErrorFlags(void);
void SIM200_Request_GetDynamicElectricalIsolation(void);
void SIM200_Request_GetDynamicCapacitiveEnergy(void);

/* Host -> SIM200: high-resolution measurements (DLC 3, mux 0x60-0x67, 0x6F) */
void SIM200_Request_GetMeasVnegHr(void);
void SIM200_Request_GetMeasVposHr(void);
void SIM200_Request_GetMeasVexcHr(void);
void SIM200_Request_GetMeasVbattHr(void);
void SIM200_Request_GetMeasVpowerHr(void);
void SIM200_Request_GetMeasTemperatureHr(void);
void SIM200_Request_GetMeasVcalNegHr(void);
void SIM200_Request_GetMeasVcalPosHr(void);
void SIM200_Request_GetMeasUptimeCounter(void);

/**
 * 제품/제조사 정보 MUX 요청 (SIM200_MUX_MFG_* , 0x01..0x0C).
 * Init에서만 호출하는 것을 권장.
 */
void SIM200_Request_MfgRead(uint8_t mux);

/* -------------------------------------------------------------------------- */
/* Host -> SIM200: commands (DLC 3) */
/* -------------------------------------------------------------------------- */
void SIM200_Cmd_Reset(void);
void SIM200_Cmd_Disable(SIM200_DisableExcitation_t lock_mode);
void SIM200_Cmd_EnterSetupMode(void);

/* -------------------------------------------------------------------------- */
/* Pure pack helpers (no I/O): build buffers for custom transmit paths */
/* -------------------------------------------------------------------------- */
void SIM200_Pack_Request3(uint8_t mux, uint8_t b1, uint8_t b2, uint8_t out[3]);
void SIM200_Pack_DisableCommand(SIM200_DisableExcitation_t lock_mode, uint8_t out[3]);
void SIM200_Pack_ResetCommand(uint8_t out[3]);

/**
 * Build setup challenge answer from SIM200 challenge frame (DLC 8).
 * challenge[0] must be 0xC3; XOR challenge[1..7] with SIM200 setup key.
 */
void SIM200_BuildSetupChallengeResponse(const uint8_t challenge[8], uint8_t answer[8]);

#ifdef __cplusplus
}
#endif

#endif /* SIM200_PROTOCOL_H */
