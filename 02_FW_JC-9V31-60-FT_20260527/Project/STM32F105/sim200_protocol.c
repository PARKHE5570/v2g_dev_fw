/**
 * @file sim200_protocol.c
 * @brief Sensata SIM200 CAN protocol — pack/unpack only (no MCU peripherals).
 */

#include "sim200_protocol.h"

#include <stddef.h>
#include <string.h>

SIM200_Data_0hz_t   sim200_data_0hz;
SIM200_Data_100hz_t sim200_data_100hz;
SIM200_Data_4hz_t   sim200_data_4hz;

/* Setup challenge: expect ACK/NAK on next 0xC3 after we transmit XOR answer */
static uint8_t s_setup_expect_c3_result;

static const uint8_t s_setup_xor_key[SIM200_SETUP_CHALLENGE_KEY_LEN] = {
    0x53u, 0x4Eu, 0x44u, 0x53u, 0x49u, 0x4Du, 0x42u
};

/* -------------------------------------------------------------------------- */
/* Big-endian helpers (no bit-fields, no unaligned struct overlay) */
/* -------------------------------------------------------------------------- */
static uint16_t sim200_be16u(const uint8_t *p)
{
    return (uint16_t)(((uint16_t)p[0] << 8) | (uint16_t)p[1]);
}

static int16_t sim200_be16s(const uint8_t *p)
{
    return (int16_t)sim200_be16u(p);
}

static uint32_t sim200_be32u(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16)
         | ((uint32_t)p[2] << 8)  | (uint32_t)p[3];
}

static int32_t sim200_be32s(const uint8_t *p)
{
    return (int32_t)sim200_be32u(p);
}

static int sim200_is_ack3(const uint8_t *p)
{
    return (p[0] == SIM200_ACK_B0 && p[1] == SIM200_ACK_B1 && p[2] == SIM200_ACK_B2) ? 1 : 0;
}

static int sim200_is_nak3(const uint8_t *p)
{
    return (p[0] == SIM200_NAK_B0 && p[1] == SIM200_NAK_B1 && p[2] == SIM200_NAK_B2) ? 1 : 0;
}

void SIM200_Protocol_Init(void)
{
    memset(&sim200_data_0hz,   0, sizeof(sim200_data_0hz));
    memset(&sim200_data_100hz, 0, sizeof(sim200_data_100hz));
    memset(&sim200_data_4hz,   0, sizeof(sim200_data_4hz));
    s_setup_expect_c3_result = 0u;
}

void SIM200_Pack_Request3(uint8_t mux, uint8_t b1, uint8_t b2, uint8_t out[3])
{
    out[0] = mux;
    out[1] = b1;
    out[2] = b2;
}

void SIM200_Pack_ResetCommand(uint8_t out[3])
{
    SIM200_Pack_Request3(SIM200_MUX_CMD_RESET, SIM200_RESET_BYTE1, SIM200_RESET_BYTE2, out);
}

void SIM200_Pack_DisableCommand(SIM200_DisableExcitation_t lock_mode, uint8_t out[3])
{
    uint8_t b2 = SIM200_DISABLE_LOCK_ZERO;
    if (lock_mode == SIM200_DISABLE_EXCITATION_LOW) {
        b2 = SIM200_DISABLE_LOCK_LOW;
    } else if (lock_mode == SIM200_DISABLE_EXCITATION_HIGH) {
        b2 = SIM200_DISABLE_LOCK_HIGH;
    }
    SIM200_Pack_Request3(SIM200_MUX_CMD_DISABLE, SIM200_DISABLE_BYTE1, b2, out);
}

void SIM200_BuildSetupChallengeResponse(const uint8_t challenge[8], uint8_t answer[8])
{
    size_t i;

    answer[0] = challenge[0];
    for (i = 0u; i < (size_t)SIM200_SETUP_CHALLENGE_KEY_LEN; i++) {
        answer[i + 1u] = (uint8_t)(challenge[i + 1u] ^ s_setup_xor_key[i]);
    }
}

static void sim200_handle_setup_c3(uint8_t dlc, const uint8_t *d)
{
    /* After XOR reply, SIM200 responds with opcode 0xC3 + 3-byte ACK/NAK */
    if (s_setup_expect_c3_result != 0u && dlc >= 4u) {
        if (sim200_is_ack3(&d[1])) {
            sim200_data_0hz.is_setup_mode = 1u;
            sim200_data_0hz.last_setup_challenge_ok = 1u;
            s_setup_expect_c3_result = 0u;
            return;
        }
        if (sim200_is_nak3(&d[1])) {
            sim200_data_0hz.is_setup_mode = 0u;
            sim200_data_0hz.last_setup_challenge_ok = 0u;
            s_setup_expect_c3_result = 0u;
            return;
        }
    }

    /* Challenge frame: DLC 8, Byte 0 = 0xC3, seven device-specific bytes follow */
    if (dlc == 8u) {
        uint8_t response[8];
        SIM200_BuildSetupChallengeResponse(d, response);
        s_setup_expect_c3_result = 1u;
        SIM200_Port_TransmitHostFrame(8, response);
    }
}

static void sim200_rx_status_byte(const uint8_t *d)
{
    sim200_data_4hz.sim200_status.raw = d[1];
}

static void sim200_parse_e0(const uint8_t *d)
{
    sim200_rx_status_byte(d);
    sim200_data_4hz.e_iso = sim200_be16u(&d[2]);
    sim200_data_4hz.e_iso_u = d[4];
    sim200_data_4hz.e_energy = sim200_be16u(&d[5]);
    sim200_data_4hz.e_energy_u = d[7];
    sim200_data_4hz.have_e0 = 1u;
}

static void sim200_parse_e1(const uint8_t *d)
{
    sim200_rx_status_byte(d);
    sim200_data_4hz.iso_res_pos = sim200_be16u(&d[2]);
    sim200_data_4hz.iso_res_pos_u = d[4];
    sim200_data_4hz.iso_res_neg = sim200_be16u(&d[5]);
    sim200_data_4hz.iso_res_neg_u = d[7];
    sim200_data_4hz.have_e1 = 1u;
}

static void sim200_parse_e2(const uint8_t *d)
{
    sim200_rx_status_byte(d);
    sim200_data_4hz.iso_cap_pos = sim200_be16u(&d[2]);
    sim200_data_4hz.iso_cap_pos_u = d[4];
    sim200_data_4hz.iso_cap_neg = sim200_be16u(&d[5]);
    sim200_data_4hz.iso_cap_neg_u = d[7];
    sim200_data_4hz.have_e2 = 1u;
}

static void sim200_parse_e3(const uint8_t *d)
{
    sim200_rx_status_byte(d);
    sim200_data_4hz.v_pos = sim200_be16s(&d[2]);
    sim200_data_4hz.v_pos_u = d[4];
    sim200_data_4hz.v_neg = sim200_be16s(&d[5]);
    sim200_data_4hz.v_neg_u = d[7];
    sim200_data_4hz.have_e3 = 1u;
}

static void sim200_parse_e4(const uint8_t *d)
{
    sim200_rx_status_byte(d);
    sim200_data_4hz.v_batt = sim200_be16s(&d[2]);
    sim200_data_4hz.v_batt_u = d[4];
    sim200_data_4hz.max_voltage = sim200_be16s(&d[5]);
    sim200_data_4hz.have_e4 = 1u;
}

static void sim200_parse_e5(const uint8_t *d)
{
    sim200_rx_status_byte(d);
    sim200_data_4hz.err_flags.raw = sim200_be16u(&d[2]);
    sim200_data_4hz.have_e5 = 1u;
}

static void sim200_parse_e6(const uint8_t *d)
{
    sim200_rx_status_byte(d);
    sim200_data_4hz.iso_min_res = sim200_be16u(&d[2]);
    sim200_data_4hz.iso_min_res_u = d[4];
    sim200_data_4hz.e_iso_dyn = sim200_be16u(&d[5]);
    sim200_data_4hz.e_iso_dyn_u = d[7];
    sim200_data_4hz.have_e6 = 1u;
}

static void sim200_parse_e7(const uint8_t *d)
{
    sim200_rx_status_byte(d);
    sim200_data_4hz.iso_total_cap = sim200_be16u(&d[2]);
    sim200_data_4hz.iso_total_cap_u = d[4];
    sim200_data_4hz.e_energy_dyn = sim200_be16u(&d[5]);
    sim200_data_4hz.e_energy_dyn_u = d[7];
    sim200_data_4hz.have_e7 = 1u;
}

void SIM200_Protocol_ProcessRx(uint8_t dlc, const uint8_t *data)
{
    uint8_t mux;

    if (data == NULL || dlc == 0u) {
        return;
    }

    mux = data[0];

    switch (mux) {
    case SIM200_MUX_GET_ISO_STATE:
        if (dlc >= 8u) {
            sim200_parse_e0(data);
        }
        break;

    case SIM200_MUX_GET_ISO_RESISTANCES:
        if (dlc >= 8u) {
            sim200_parse_e1(data);
        }
        break;

    case SIM200_MUX_GET_SYS_Y_CAP:
        if (dlc >= 8u) {
            sim200_parse_e2(data);
        }
        break;

    case SIM200_MUX_GET_HV_RAIL:
        if (dlc >= 8u) {
            sim200_parse_e3(data);
        }
        break;

    case SIM200_MUX_GET_BATTERY_VOLTAGE:
        if (dlc >= 8u) {
            sim200_parse_e4(data);
        }
        break;

    case SIM200_MUX_GET_ERROR_FLAGS:
        if (dlc >= 8u) {
            sim200_parse_e5(data);
        }
        break;

    case SIM200_MUX_GET_DYN_E_ISO:
        if (dlc >= 8u) {
            sim200_parse_e6(data);
        }
        break;

    case SIM200_MUX_GET_DYN_CAP_ENERGY:
        if (dlc >= 8u) {
            sim200_parse_e7(data);
        }
        break;

    case SIM200_MUX_CMD_DISABLE:
        /* Response: [0xC2][0xEC][disable_cmd_ack]... */
        if (dlc >= 3u && data[1] == SIM200_DISABLE_BYTE1) {
            sim200_data_0hz.disable_cmd_ack = data[2];
            sim200_data_0hz.have_disable_ack = 1u;
        }
        break;

    case SIM200_MUX_CMD_SETUP_MODE:
        sim200_handle_setup_c3(dlc, data);
        break;

    /* ---------------- Manufacturer / product (N/A, Init) 0x01 .. 0x0C ---------------- */
    case SIM200_MUX_MFG_PART_NAME_0:
    case SIM200_MUX_MFG_PART_NAME_1:
    case SIM200_MUX_MFG_PART_NAME_2:
    case SIM200_MUX_MFG_PART_NAME_3:
        if (dlc >= 8u) {
            uint8_t idx = (uint8_t)(mux - SIM200_MUX_MFG_PART_NAME_0);
            memcpy(sim200_data_0hz.part_name[idx], &data[1], 7u);
            sim200_data_0hz.part_name[idx][7] = '\0';
            sim200_data_0hz.have_part_name[idx] = 1u;
        }
        break;

    case SIM200_MUX_MFG_FW_VERSION:
        if (dlc >= 5u) {
            sim200_data_0hz.fw_ver_major = data[1];
            sim200_data_0hz.fw_ver_minor = data[2];
            sim200_data_0hz.fw_ver_patch = data[3];
            sim200_data_0hz.fw_ver_pre   = data[4];
            sim200_data_0hz.have_fw_ver = 1u;
        }
        break;

    case SIM200_MUX_MFG_VERSION_INFO_0:
    case SIM200_MUX_MFG_VERSION_INFO_1:
    case SIM200_MUX_MFG_VERSION_INFO_2:
        if (dlc >= 8u) {
            uint8_t idx = (uint8_t)(mux - SIM200_MUX_MFG_VERSION_INFO_0);
            memcpy(sim200_data_0hz.version_info[idx], &data[1], 7u);
            sim200_data_0hz.version_info[idx][7] = '\0';
            sim200_data_0hz.have_version_info[idx] = 1u;
        }
        break;

    case SIM200_MUX_MFG_SERIAL_0:
    case SIM200_MUX_MFG_SERIAL_1:
    case SIM200_MUX_MFG_SERIAL_2:
    case SIM200_MUX_MFG_SERIAL_3:
        if (dlc >= 5u) {
            uint8_t idx = (uint8_t)(mux - SIM200_MUX_MFG_SERIAL_0);
            sim200_data_0hz.serial_u32[idx] = sim200_be32u(&data[1]);
            sim200_data_0hz.have_serial[idx] = 1u;
        }
        break;

    /* ---------------- Measurements 0x60 .. 0x6F (10 ms / 약 100 Hz 그룹) ---------------- */
    case SIM200_MUX_MEAS_VNEG_HR:
        if (dlc >= 5u) {
            memcpy(sim200_data_100hz.meas_scratch, data, (dlc > 8u) ? 8u : dlc);
            sim200_data_100hz.v_neg_uV = sim200_be32s(&data[1]);
            sim200_data_100hz.have_60 = 1u;
        }
        break;
    case SIM200_MUX_MEAS_VPOS_HR:
        if (dlc >= 5u) {
            memcpy(sim200_data_100hz.meas_scratch, data, (dlc > 8u) ? 8u : dlc);
            sim200_data_100hz.v_pos_uV = sim200_be32s(&data[1]);
            sim200_data_100hz.have_61 = 1u;
        }
        break;
    case SIM200_MUX_MEAS_VEXC_HR:
    case SIM200_MUX_MEAS_VBATT_HR:
    case SIM200_MUX_MEAS_VPOWER_HR:
    case SIM200_MUX_MEAS_TEMPERATURE:
    case SIM200_MUX_MEAS_VCAL_NEG:
    case SIM200_MUX_MEAS_VCAL_POS:
        if (dlc >= 5u) {
            memcpy(sim200_data_100hz.meas_scratch, data, (dlc > 8u) ? 8u : dlc);
        }
        break;
    case SIM200_MUX_MEAS_UPTIME_COUNTER:
        if (dlc >= 5u) {
            memcpy(sim200_data_100hz.meas_scratch, data, (dlc > 8u) ? 8u : dlc);
            sim200_data_100hz.uptime_s = sim200_be32u(&data[1]);
            sim200_data_100hz.have_6f = 1u;
        }
        break;

    /* ---------------- Configuration 0xF0 .. 0xF1 (0 Hz / Init 그룹) ---------------- */
    case SIM200_MUX_GET_PROG_MAX_VOLTAGE:
        if (dlc >= 3u) {
            sim200_data_0hz.prog_max_voltage = sim200_be16u(&data[1]);
            sim200_data_0hz.have_f0 = 1u;
        }
        break;
    case SIM200_MUX_GET_ISO_THRESHOLD_CFG:
        if (dlc >= 5u) {
            sim200_data_0hz.iso_fault_thr = sim200_be16u(&data[1]);
            sim200_data_0hz.iso_warn_thr = sim200_be16u(&data[3]);
            sim200_data_0hz.have_f1 = 1u;
        }
        break;

    default:
        break;
    }
}

void SIM200_Request_GetIsolationState(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_GET_ISO_STATE, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetIsolationResistances(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_GET_ISO_RESISTANCES, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetSystemYCapacitances(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_GET_SYS_Y_CAP, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetHVRailMeasurements(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_GET_HV_RAIL, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetBatteryVoltage(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_GET_BATTERY_VOLTAGE, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetErrorFlags(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_GET_ERROR_FLAGS, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetDynamicElectricalIsolation(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_GET_DYN_E_ISO, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetDynamicCapacitiveEnergy(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_GET_DYN_CAP_ENERGY, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetMeasVnegHr(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_MEAS_VNEG_HR, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetMeasVposHr(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_MEAS_VPOS_HR, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetMeasVexcHr(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_MEAS_VEXC_HR, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetMeasVbattHr(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_MEAS_VBATT_HR, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetMeasVpowerHr(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_MEAS_VPOWER_HR, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetMeasTemperatureHr(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_MEAS_TEMPERATURE, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetMeasVcalNegHr(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_MEAS_VCAL_NEG, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetMeasVcalPosHr(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_MEAS_VCAL_POS, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_GetMeasUptimeCounter(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_MEAS_UPTIME_COUNTER, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Request_MfgRead(uint8_t mux)
{
    uint8_t p[3];

    if (mux < SIM200_MUX_MFG_PART_NAME_0 || mux > SIM200_MUX_MFG_SERIAL_3) {
        return;
    }
    SIM200_Pack_Request3(mux, 0u, 0u, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Cmd_Reset(void)
{
    uint8_t p[3];
    SIM200_Pack_ResetCommand(p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Cmd_Disable(SIM200_DisableExcitation_t lock_mode)
{
    uint8_t p[3];
    SIM200_Pack_DisableCommand(lock_mode, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

void SIM200_Cmd_EnterSetupMode(void)
{
    uint8_t p[3];
    SIM200_Pack_Request3(SIM200_MUX_CMD_SETUP_MODE, SIM200_SETUP_BYTE1, SIM200_SETUP_BYTE2, p);
    SIM200_Port_TransmitHostFrame(3u, p);
}

/* -------------------------------------------------------------------------- */
/* Tx 폴링 MUX 목록 (스케줄 정책 — 필요 시 이 배열만 편집)                      */
/* -------------------------------------------------------------------------- */
/* Init 1회: 제품정보(N/A) 후 설정 MUX — 순서는 필요 시 변경 */
const uint8_t SIM200_mux_poll_0hz[] = {
    SIM200_MUX_MFG_PART_NAME_0,
    SIM200_MUX_MFG_PART_NAME_1,
    SIM200_MUX_MFG_PART_NAME_2,
    SIM200_MUX_MFG_PART_NAME_3,
    SIM200_MUX_MFG_FW_VERSION,
    SIM200_MUX_MFG_VERSION_INFO_0,
    SIM200_MUX_MFG_VERSION_INFO_1,
    SIM200_MUX_MFG_VERSION_INFO_2,
    SIM200_MUX_MFG_SERIAL_0,
    SIM200_MUX_MFG_SERIAL_1,
    SIM200_MUX_MFG_SERIAL_2,
    SIM200_MUX_MFG_SERIAL_3,
    SIM200_MUX_GET_PROG_MAX_VOLTAGE,
    SIM200_MUX_GET_ISO_THRESHOLD_CFG
};

const size_t SIM200_mux_poll_0hz_count =
    sizeof(SIM200_mux_poll_0hz) / sizeof(SIM200_mux_poll_0hz[0]);

const uint8_t SIM200_mux_poll_100hz[] = {
    SIM200_MUX_MEAS_VNEG_HR,
    SIM200_MUX_MEAS_VPOS_HR,
    SIM200_MUX_MEAS_VEXC_HR,
    SIM200_MUX_MEAS_VBATT_HR,
    SIM200_MUX_MEAS_VPOWER_HR,
    SIM200_MUX_MEAS_TEMPERATURE,
    SIM200_MUX_MEAS_VCAL_NEG,
    SIM200_MUX_MEAS_VCAL_POS,
    SIM200_MUX_MEAS_UPTIME_COUNTER
};

const size_t SIM200_mux_poll_100hz_count =
    sizeof(SIM200_mux_poll_100hz) / sizeof(SIM200_mux_poll_100hz[0]);

const uint8_t SIM200_mux_poll_4hz[] = {
    SIM200_MUX_GET_ISO_STATE,
    SIM200_MUX_GET_ISO_RESISTANCES,
    SIM200_MUX_GET_SYS_Y_CAP,
    SIM200_MUX_GET_HV_RAIL,
    SIM200_MUX_GET_BATTERY_VOLTAGE,
    SIM200_MUX_GET_ERROR_FLAGS,
    SIM200_MUX_GET_DYN_E_ISO,
    SIM200_MUX_GET_DYN_CAP_ENERGY
};

const size_t SIM200_mux_poll_4hz_count =
    sizeof(SIM200_mux_poll_4hz) / sizeof(SIM200_mux_poll_4hz[0]);
