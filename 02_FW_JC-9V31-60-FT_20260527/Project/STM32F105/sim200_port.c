/**
 * @file sim200_port.c
 * @brief STM32F105 SPL — CAN extended-ID path for Sensata SIM200.
 *
 * Board note: CAN clock / pin remap / bit timing are normally configured in your
 * existing CAN_Config(). This file only performs ID/data handling compatible with SPL.
 *
 * Tx는 CAN_TransmitStatus(버스 상 전송 완료)를 대기하지 않는다.
 *
 * Override SIM200_CANx before including this translation unit if SIM200 is not on CAN2.
 */

#include "sim200_protocol.h"

#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include <string.h>

#ifndef SIM200_CANx
#define SIM200_CANx   CAN2
#endif

void SIM200_Port_TransmitHostFrame(uint8_t dlc, const uint8_t *payload)
{
    CanTxMsg tx;
    uint8_t i;

    memset(&tx, 0, sizeof(tx));

    tx.StdId = 0u;
    tx.ExtId = SIM200_CAN_ID_HOST_TX;
    tx.IDE = CAN_ID_EXT;
    tx.RTR = CAN_RTR_DATA;

    if (dlc > 8u) {
        dlc = 8u;
    }
    tx.DLC = dlc;

    for (i = 0u; i < dlc; i++) {
        tx.Data[i] = payload[i];
    }
    for (; i < 8u; i++) {
        tx.Data[i] = 0u;
    }

    (void)CAN_Transmit(SIM200_CANx, &tx);
}

void SIM200_Port_PollRx(void)
{
    CanRxMsg rx;

    while (CAN_MessagePending(SIM200_CANx, CAN_FIFO0) != 0u) {
        CAN_Receive(SIM200_CANx, CAN_FIFO0, &rx);

        if (rx.IDE == CAN_ID_EXT && rx.ExtId == SIM200_CAN_ID_SIM200_TX) {
            SIM200_Protocol_ProcessRx(rx.DLC, rx.Data);
        }
    }
}
