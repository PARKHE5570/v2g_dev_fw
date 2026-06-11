#define _PLC3_0_C


#include "stm32f10x.h"
#include "PLC_3_0.h"
#include "stm32f105_init.h"
#include "fault_check.h"
#include "main.h"
#include "sequence.h"
#include "HMI_modbus.h"

/*
Idle
Ready                                      : Plug in and Start - State A1 (12V) OR B1 (9V)
Low Level Communication                    : Communication started (9V) - SLAC
High Level Communication                   : (9V)
Authorization/ ContractAuthentication      : -State B2 (9V) 
ChargeParameterDiscovery                   : Exchange charging parameters -    State B2 (9V)
CableCheck                                 : Perform isolation test of the charging cable(6V)  EVSE Relay ON
PreCharge                                  : Ramp up output voltage(6V)
Charging                                   : (6V)
EV Initiated Stop Charging or EVSE Initiated Stop, or Emgergence Stop :
Terminate                                  : Plug out (9V)
Idle Wait                                  : Initialize
Idle
*/

void Variable_Initialize_1CH(void)
{
  EVSE_Msg[1].EVSE_Configurations.Id_Payment_Option_0.bit.evseIdConfig_DIN = 0; //0 (Default = 00), 1 (Custom EVSE ID, must conform to DIN SPEC 91286.)
  EVSE_Msg[1].EVSE_Configurations.Id_Payment_Option_0.bit.evseIdConfig_ISO = 0; //0 (Default = zz00000), 1 (Custom EVSE ID, must conform to IETF RFC 5234 and ISO 15118-2 Annex H.)
  EVSE_Msg[1].EVSE_Configurations.Id_Payment_Option_0.bit.supportedPaymentOption_PnC = Not_supported;
  EVSE_Msg[1].EVSE_Configurations.Id_Payment_Option_0.bit.supportedPaymentOption_EIM = Supported;
  EVSE_Msg[1].EVSE_Configurations.Id_Payment_Option_0.bit.environmentConfiguration = 0; //00 (Legacy, Default) ISO/DIN selection based on EVCC's priority / 01~14 (Reserved) / 15 (Force DIN) Force DIN
  EVSE_Msg[1].EVSE_Configurations.Id_Payment_Option_0.bit.Reserved = 0;
  EVSE_Msg[1].EVSE_Configurations.evseIdLength_DIN_1 = 0;
  EVSE_Msg[1].EVSE_Configurations.evseIdLength_ISO_2 = 0;
  EVSE_Msg[1].EVSE_Configurations.SupportedEnergyTransfer_3.bit.AC1 = Not_supported;
  EVSE_Msg[1].EVSE_Configurations.SupportedEnergyTransfer_3.bit.AC3 = Not_supported;
  EVSE_Msg[1].EVSE_Configurations.SupportedEnergyTransfer_3.bit.DCcore = Not_supported; 
  EVSE_Msg[1].EVSE_Configurations.SupportedEnergyTransfer_3.bit.DCextd_CCS = DC_extended_Supported;
  EVSE_Msg[1].EVSE_Configurations.SupportedEnergyTransfer_3.bit.DCcc = Not_supported;
  EVSE_Msg[1].EVSE_Configurations.SupportedEnergyTransfer_3.bit.DCunique = Not_supported;
  EVSE_Msg[1].EVSE_Configurations.SupportedEnergyTransfer_3.bit.Reserved = 0;
  EVSE_Msg[1].EVSE_Configurations.ServiceList_4.bit.Internet = Not_supported;
  EVSE_Msg[1].EVSE_Configurations.ServiceList_4.bit.ContractCert = Not_supported;
  EVSE_Msg[1].EVSE_Configurations.ServiceList_4.bit.OtherCustom = Not_supported;
  EVSE_Msg[1].EVSE_Configurations.ServiceList_4.bit.Reserved = 0;
  EVSE_Msg[1].EVSE_Configurations.freeService_5.bit.EVCharging = Payed_Service; 
  EVSE_Msg[1].EVSE_Configurations.freeService_5.bit.Internet = Payed_Service;
  EVSE_Msg[1].EVSE_Configurations.freeService_5.bit.ContractCert = Payed_Service;
  EVSE_Msg[1].EVSE_Configurations.freeService_5.bit.OtherCustom = Payed_Service;
  EVSE_Msg[1].EVSE_Configurations.freeService_5.bit.Reserved = 0;
  EVSE_Msg[1].EVSE_Configurations.saScheduleManagement_6 = Default;
  EVSE_Msg[1].EVSE_Configurations.Reserved_7 = 0;
  
  EVSE_Msg[1].EVSE_Status.evseHeartbeat_0 = 0; 
  EVSE_Msg[1].EVSE_Status.chargingControl_1 = NONE;
  EVSE_Msg[1].EVSE_Status.notificationMaxDelay_23 = 0; 
  EVSE_Msg[1].EVSE_Status.evseIsolationStatus_4 = Invalid;
  EVSE_Msg[1].EVSE_Status.Reserved_5 = 0;
  EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.evseProcessing_AuthEIM = Ongoing; 
  EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.evseProcessing_CPD = Ongoing; 
  EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.evseProcessing_CableCheck = Ongoing; 
  EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.isolationMonitoringActive = Not_active; 
  EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.triggerReNegotiation = None;
  EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.Reserved1 = 0;
  EVSE_Msg[1].EVSE_Status.Processing_trgger_6.bit.Reserved2 = 0;
  
  EVSE_Msg[1].EVSE_Status.LimitAchieved_7.bit.evseCurrentLimitAchieved = FALSE; 
  EVSE_Msg[1].EVSE_Status.LimitAchieved_7.bit.evseVoltageLimitAchieved = FALSE; 
  EVSE_Msg[1].EVSE_Status.LimitAchieved_7.bit.evsePowerLimitAchieved = FALSE; 
  EVSE_Msg[1].EVSE_Status.LimitAchieved_7.bit.Reserved = 0;
  EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumVoltageLimit_45 = 10000;      // x 0.1 1000V
  EVSE_Msg[1].EVSE_Charge_Parameters1.evsePeakCurrentRipple_67 = 60;          // x 0.1
  
  EVSE_Msg[1].EVSE_Charge_Parameters2.evseMinimumCurrentLimit_01 = 20;        // x 0.1  2A
  EVSE_Msg[1].EVSE_Charge_Parameters2.evseMinimumVoltageLimit_23 = 1500;      // x 0.1  150V
  EVSE_Msg[1].EVSE_Charge_Parameters2.evseCurrentRegulationTolerance_45 = 20; // x 0.1
  EVSE_Msg[1].EVSE_Charge_Parameters2.evseEnergyToBeDelivered_67 = 0;         // x 10
  
  EVSE_Msg[1].EVSE_Present_Voltage_Current.evsePresentVoltage = 0;              // x 0.1
  EVSE_Msg[1].EVSE_Present_Voltage_Current.evsePresentCurrent = 0;              // x 0.1
  EVSE_Msg[1].EVSE_Present_Voltage_Current.Reserved_45 = 0;
  EVSE_Msg[1].EVSE_Present_Voltage_Current.Reserved_67 = 0;
  
  EVSE_Msg[1].EVSE_DIN_ID1.evseDinId1_High = 0;
  EVSE_Msg[1].EVSE_DIN_ID1.evseDinId1_Low = 0;
  
  EVSE_Msg[1].EVSE_DIN_ID2.evseDinId2_High = 0;
  EVSE_Msg[1].EVSE_DIN_ID2.evseDinId2_Low = 0;
  
  EVSE_Msg[1].EVSE_DIN_ID3.evseDinId3_High = 0;
  EVSE_Msg[1].EVSE_DIN_ID3.evseDinId3_Low = 0;
  
  EVSE_Msg[1].EVSE_DIN_ID4.evseDinId4_High = 0;
  EVSE_Msg[1].EVSE_DIN_ID4.evseDinId4_Low = 0;
  
  EVSE_Msg[1].EVSE_ISO_ID1.evseIsoId1_High = 0;
  EVSE_Msg[1].EVSE_ISO_ID1.evseIsoId1_Low = 0;
  
  EVSE_Msg[1].EVSE_ISO_ID2.evseIsoId2_High = 0;
  EVSE_Msg[1].EVSE_ISO_ID2.evseIsoId2_Low = 0;
  
  EVSE_Msg[1].EVSE_ISO_ID3.evseIsoId3_High = 0;
  EVSE_Msg[1].EVSE_ISO_ID3.evseIsoId3_Low = 0;
  
  EVSE_Msg[1].EVSE_ISO_ID4.evseIsoId4_High = 0;
  EVSE_Msg[1].EVSE_ISO_ID4.evseIsoId4_Low = 0;
  
  EVSE_Msg[1].EVSE_ISO_ID5.evseIsoId5_High = 0;
  EVSE_Msg[1].EVSE_ISO_ID5.evseIsoId5_Low = 0;
}

void Variable_Initialize_2CH(void)
{
  EVSE_Msg[2].EVSE_Configurations.Id_Payment_Option_0.bit.evseIdConfig_DIN = 0; //0 (Default = 00), 1 (Custom EVSE ID, must conform to DIN SPEC 91286.)
  EVSE_Msg[2].EVSE_Configurations.Id_Payment_Option_0.bit.evseIdConfig_ISO = 0; //0 (Default = zz00000), 1 (Custom EVSE ID, must conform to IETF RFC 5234 and ISO 15118-2 Annex H.)
  EVSE_Msg[2].EVSE_Configurations.Id_Payment_Option_0.bit.supportedPaymentOption_PnC = Not_supported;
  EVSE_Msg[2].EVSE_Configurations.Id_Payment_Option_0.bit.supportedPaymentOption_EIM = Supported;
  EVSE_Msg[2].EVSE_Configurations.Id_Payment_Option_0.bit.environmentConfiguration = 0; //00 (Legacy, Default) ISO/DIN selection based on EVCC's priority / 01~14 (Reserved) / 15 (Force DIN) Force DIN
  EVSE_Msg[2].EVSE_Configurations.Id_Payment_Option_0.bit.Reserved = 0;
  EVSE_Msg[2].EVSE_Configurations.evseIdLength_DIN_1 = 0;
  EVSE_Msg[2].EVSE_Configurations.evseIdLength_ISO_2 = 0;
  EVSE_Msg[2].EVSE_Configurations.SupportedEnergyTransfer_3.bit.AC1 = Not_supported;
  EVSE_Msg[2].EVSE_Configurations.SupportedEnergyTransfer_3.bit.AC3 = Not_supported;
  EVSE_Msg[2].EVSE_Configurations.SupportedEnergyTransfer_3.bit.DCcore = Not_supported; 
  EVSE_Msg[2].EVSE_Configurations.SupportedEnergyTransfer_3.bit.DCextd_CCS = DC_extended_Supported;
  EVSE_Msg[2].EVSE_Configurations.SupportedEnergyTransfer_3.bit.DCcc = Not_supported;
  EVSE_Msg[2].EVSE_Configurations.SupportedEnergyTransfer_3.bit.DCunique = Not_supported;
  EVSE_Msg[2].EVSE_Configurations.SupportedEnergyTransfer_3.bit.Reserved = 0;
  EVSE_Msg[2].EVSE_Configurations.ServiceList_4.bit.Internet = Not_supported;
  EVSE_Msg[2].EVSE_Configurations.ServiceList_4.bit.ContractCert = Not_supported;
  EVSE_Msg[2].EVSE_Configurations.ServiceList_4.bit.OtherCustom = Not_supported;
  EVSE_Msg[2].EVSE_Configurations.ServiceList_4.bit.Reserved = 0;
  EVSE_Msg[2].EVSE_Configurations.freeService_5.bit.EVCharging = Payed_Service; 
  EVSE_Msg[2].EVSE_Configurations.freeService_5.bit.Internet = Payed_Service;
  EVSE_Msg[2].EVSE_Configurations.freeService_5.bit.ContractCert = Payed_Service;
  EVSE_Msg[2].EVSE_Configurations.freeService_5.bit.OtherCustom = Payed_Service;
  EVSE_Msg[2].EVSE_Configurations.freeService_5.bit.Reserved = 0;
  EVSE_Msg[2].EVSE_Configurations.saScheduleManagement_6 = Default;
  EVSE_Msg[2].EVSE_Configurations.Reserved_7 = 0;
  
  EVSE_Msg[2].EVSE_Status.evseHeartbeat_0 = 0; 
  EVSE_Msg[2].EVSE_Status.chargingControl_1 = NONE;
  EVSE_Msg[2].EVSE_Status.notificationMaxDelay_23 = 0; 
  EVSE_Msg[2].EVSE_Status.evseIsolationStatus_4 = Invalid;
  EVSE_Msg[2].EVSE_Status.Reserved_5 = 0;
  EVSE_Msg[2].EVSE_Status.Processing_trgger_6.bit.evseProcessing_AuthEIM = Ongoing; 
  EVSE_Msg[2].EVSE_Status.Processing_trgger_6.bit.evseProcessing_CPD = Ongoing; 
  EVSE_Msg[2].EVSE_Status.Processing_trgger_6.bit.evseProcessing_CableCheck = Ongoing; 
  EVSE_Msg[2].EVSE_Status.Processing_trgger_6.bit.isolationMonitoringActive = Not_active; 
  EVSE_Msg[2].EVSE_Status.Processing_trgger_6.bit.triggerReNegotiation = None;
  EVSE_Msg[2].EVSE_Status.Processing_trgger_6.bit.Reserved1 = 0;
  EVSE_Msg[2].EVSE_Status.Processing_trgger_6.bit.Reserved2 = 0;
  
  EVSE_Msg[2].EVSE_Status.LimitAchieved_7.bit.evseCurrentLimitAchieved = FALSE; 
  EVSE_Msg[2].EVSE_Status.LimitAchieved_7.bit.evseVoltageLimitAchieved = FALSE; 
  EVSE_Msg[2].EVSE_Status.LimitAchieved_7.bit.evsePowerLimitAchieved = FALSE; 
  EVSE_Msg[2].EVSE_Status.LimitAchieved_7.bit.Reserved = 0;
  EVSE_Msg[2].EVSE_Charge_Parameters1.evseMaximumVoltageLimit_45 = 10000;      // x 0.1 1000V
  EVSE_Msg[2].EVSE_Charge_Parameters1.evsePeakCurrentRipple_67 = 60;          // x 0.1
  
  EVSE_Msg[2].EVSE_Charge_Parameters2.evseMinimumCurrentLimit_01 = 20;        // x 0.1  2A
  EVSE_Msg[2].EVSE_Charge_Parameters2.evseMinimumVoltageLimit_23 = 2000;      // x 0.1  200V
  EVSE_Msg[2].EVSE_Charge_Parameters2.evseCurrentRegulationTolerance_45 = 20; // x 0.1
  EVSE_Msg[2].EVSE_Charge_Parameters2.evseEnergyToBeDelivered_67 = 0;         // x 10
  
  EVSE_Msg[2].EVSE_Present_Voltage_Current.evsePresentVoltage = 0;              // x 0.1
  EVSE_Msg[2].EVSE_Present_Voltage_Current.evsePresentCurrent = 0;              // x 0.1
  EVSE_Msg[2].EVSE_Present_Voltage_Current.Reserved_45 = 0;
  EVSE_Msg[2].EVSE_Present_Voltage_Current.Reserved_67 = 0;
  
  EVSE_Msg[2].EVSE_DIN_ID1.evseDinId1_High = 0;
  EVSE_Msg[2].EVSE_DIN_ID1.evseDinId1_Low = 0;
  
  EVSE_Msg[2].EVSE_DIN_ID2.evseDinId2_High = 0;
  EVSE_Msg[2].EVSE_DIN_ID2.evseDinId2_Low = 0;
  
  EVSE_Msg[2].EVSE_DIN_ID3.evseDinId3_High = 0;
  EVSE_Msg[2].EVSE_DIN_ID3.evseDinId3_Low = 0;
  
  EVSE_Msg[2].EVSE_DIN_ID4.evseDinId4_High = 0;
  EVSE_Msg[2].EVSE_DIN_ID4.evseDinId4_Low = 0;
  
  EVSE_Msg[2].EVSE_ISO_ID1.evseIsoId1_High = 0;
  EVSE_Msg[2].EVSE_ISO_ID1.evseIsoId1_Low = 0;
  
  EVSE_Msg[2].EVSE_ISO_ID2.evseIsoId2_High = 0;
  EVSE_Msg[2].EVSE_ISO_ID2.evseIsoId2_Low = 0;
  
  EVSE_Msg[2].EVSE_ISO_ID3.evseIsoId3_High = 0;
  EVSE_Msg[2].EVSE_ISO_ID3.evseIsoId3_Low = 0;
  
  EVSE_Msg[2].EVSE_ISO_ID4.evseIsoId4_High = 0;
  EVSE_Msg[2].EVSE_ISO_ID4.evseIsoId4_Low = 0;
  
  EVSE_Msg[2].EVSE_ISO_ID5.evseIsoId5_High = 0;
  EVSE_Msg[2].EVSE_ISO_ID5.evseIsoId5_Low = 0;
}


void PLC_3_0_Msg_Send_1CH(void) // 10ms polling, 충전기 메시지 보내기 , prime number 적용, 스위치 케이스문 적용으로 인해서 연산율 감소
{
  if(++EVSE_SECC_cnt_1CH_50ms_1 >= 5)
  {
    EVSE_SECC_cnt_1CH_50ms_1 = 0;
    EVSE_SECC_Period_1CH = 51;
  }
  if(++EVSE_SECC_cnt_1CH_50ms_2 >= 7)
  {
    EVSE_SECC_cnt_1CH_50ms_2 = 2;
  }
  if(++EVSE_SECC_cnt_1CH_200ms_1 >= 23)
  {
    EVSE_SECC_cnt_1CH_200ms_1 = 3;
    EVSE_SECC_Period_1CH = 200;
  }
  if(++EVSE_SECC_cnt_1CH_500ms_1 >= 53)
  {
    EVSE_SECC_cnt_1CH_500ms_1 = 3;
  }
  if(++EVSE_SECC_cnt_1CH_500ms_2 >= 59)
  {
    EVSE_SECC_cnt_1CH_500ms_2 = 9;
  }
  if(++EVSE_SECC_cnt_1CH_500ms_3 >= 61)
  {
    EVSE_SECC_cnt_1CH_500ms_3 = 11;
  }
  if(EVSE_SECC_cnt_1CH_reboot == 1)
  {
    EVSE_SECC_cnt_1CH_reboot = 0;
    EVSE_SECC_Period_1CH = 1000;
  }
  
  switch(EVSE_SECC_Period_1CH)
  {
  case 51:
    EVSE_SECC_Period_1CH = 0;
    
    TxMessage2.ExtId   = EVSE_STATUS_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_Status.evseHeartbeat_0++;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_Status.chargingControl_1;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_Status.notificationMaxDelay_23;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_Status.notificationMaxDelay_23 >> 8;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_Status.evseIsolationStatus_4;
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_Status.Reserved_5;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_Status.Processing_trgger_6.all;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_Status.LimitAchieved_7.all;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_PRESENT_VOLTAGE_CURRENT_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_Present_Voltage_Current.evsePresentVoltage;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_Present_Voltage_Current.evsePresentVoltage >> 8;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_Present_Voltage_Current.evsePresentCurrent;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_Present_Voltage_Current.evsePresentCurrent >> 8;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_Present_Voltage_Current.Reserved_45;
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_Present_Voltage_Current.Reserved_45 >> 8;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_Present_Voltage_Current.Reserved_67;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_Present_Voltage_Current.Reserved_67 >> 8;
    CAN_Transmit(CAN2, &TxMessage2);    
    break;
    
  case 52:
    EVSE_SECC_Period_1CH = 0;
    
    TxMessage2.ExtId   = EVSE_DIN_ID_1_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_DIN_ID1.evseDinId1_High;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_DIN_ID1.evseDinId1_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_DIN_ID1.evseDinId1_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_DIN_ID1.evseDinId1_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_DIN_ID1.evseDinId1_Low;
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_DIN_ID1.evseDinId1_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_DIN_ID1.evseDinId1_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_DIN_ID1.evseDinId1_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_DIN_ID_2_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_DIN_ID2.evseDinId2_High;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_DIN_ID2.evseDinId2_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_DIN_ID2.evseDinId2_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_DIN_ID2.evseDinId2_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_DIN_ID2.evseDinId2_Low; 
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_DIN_ID2.evseDinId2_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_DIN_ID2.evseDinId2_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_DIN_ID2.evseDinId2_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  case 200:
    EVSE_SECC_Period_1CH = 0;
    
    TxMessage2.ExtId   = EVSE_CONFIGURATIONS_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_Configurations.Id_Payment_Option_0.all;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_Configurations.evseIdLength_DIN_1;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_Configurations.evseIdLength_ISO_2;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_Configurations.SupportedEnergyTransfer_3.all;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_Configurations.ServiceList_4.all;
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_Configurations.freeService_5.all;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_Configurations.saScheduleManagement_6;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_Configurations.Reserved_7;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_CHARGE_PARAMETERS_1_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumCurrentLimit_01;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumCurrentLimit_01 >> 8;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumPowerLimit_23;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumPowerLimit_23 >> 8;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumVoltageLimit_45;
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_Charge_Parameters1.evseMaximumVoltageLimit_45 >> 8;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_Charge_Parameters1.evsePeakCurrentRipple_67;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_Charge_Parameters1.evsePeakCurrentRipple_67 >> 8;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_CHARGE_PARAMETERS_2_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_Charge_Parameters2.evseMinimumCurrentLimit_01;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_Charge_Parameters2.evseMinimumCurrentLimit_01 >> 8;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_Charge_Parameters2.evseMinimumVoltageLimit_23;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_Charge_Parameters2.evseMinimumVoltageLimit_23 >> 8;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_Charge_Parameters2.evseCurrentRegulationTolerance_45;
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_Charge_Parameters2.evseCurrentRegulationTolerance_45 >> 8;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_Charge_Parameters2.evseEnergyToBeDelivered_67;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_Charge_Parameters2.evseEnergyToBeDelivered_67 >> 8;
    CAN_Transmit(CAN2, &TxMessage2);       
    break;
    
  case 501:
    EVSE_SECC_Period_1CH = 0;
    
    TxMessage2.ExtId   = EVSE_DIN_ID_3_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_DIN_ID3.evseDinId3_High;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_DIN_ID3.evseDinId3_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_DIN_ID3.evseDinId3_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_DIN_ID3.evseDinId3_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_DIN_ID3.evseDinId3_Low;
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_DIN_ID3.evseDinId3_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_DIN_ID3.evseDinId3_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_DIN_ID3.evseDinId3_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_DIN_ID_4_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_DIN_ID4.evseDinId4_High;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_DIN_ID4.evseDinId4_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_DIN_ID4.evseDinId4_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_DIN_ID4.evseDinId4_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_DIN_ID4.evseDinId4_Low;
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_DIN_ID4.evseDinId4_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_DIN_ID4.evseDinId4_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_DIN_ID4.evseDinId4_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_ISO_ID_1_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_ISO_ID1.evseIsoId1_High;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_ISO_ID1.evseIsoId1_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_ISO_ID1.evseIsoId1_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_ISO_ID1.evseIsoId1_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_ISO_ID1.evseIsoId1_Low;
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_ISO_ID1.evseIsoId1_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_ISO_ID1.evseIsoId1_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_ISO_ID1.evseIsoId1_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  case 502:
    EVSE_SECC_Period_1CH = 0;
    
    TxMessage2.ExtId   = EVSE_ISO_ID_2_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_ISO_ID2.evseIsoId2_High;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_ISO_ID2.evseIsoId2_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_ISO_ID2.evseIsoId2_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_ISO_ID2.evseIsoId2_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_ISO_ID2.evseIsoId2_Low;
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_ISO_ID2.evseIsoId2_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_ISO_ID2.evseIsoId2_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_ISO_ID2.evseIsoId2_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_ISO_ID_3_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_ISO_ID3.evseIsoId3_High;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_ISO_ID3.evseIsoId3_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_ISO_ID3.evseIsoId3_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_ISO_ID3.evseIsoId3_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_ISO_ID3.evseIsoId3_Low; 
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_ISO_ID3.evseIsoId3_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_ISO_ID3.evseIsoId3_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_ISO_ID3.evseIsoId3_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  case 503:
    EVSE_SECC_Period_1CH = 0;
    
    TxMessage2.ExtId   = EVSE_ISO_ID_4_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_ISO_ID4.evseIsoId4_High;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_ISO_ID4.evseIsoId4_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_ISO_ID4.evseIsoId4_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_ISO_ID4.evseIsoId4_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_ISO_ID4.evseIsoId4_Low;
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_ISO_ID4.evseIsoId4_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_ISO_ID4.evseIsoId4_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_ISO_ID4.evseIsoId4_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_ISO_ID_5_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = EVSE_Msg[1].EVSE_ISO_ID5.evseIsoId5_High;
    TxMessage2.Data[1] = EVSE_Msg[1].EVSE_ISO_ID5.evseIsoId5_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[1].EVSE_ISO_ID5.evseIsoId5_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[1].EVSE_ISO_ID5.evseIsoId5_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[1].EVSE_ISO_ID5.evseIsoId5_Low; 
    TxMessage2.Data[5] = EVSE_Msg[1].EVSE_ISO_ID5.evseIsoId5_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[1].EVSE_ISO_ID5.evseIsoId5_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[1].EVSE_ISO_ID5.evseIsoId5_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
	case 1000:
    EVSE_SECC_Period_1CH = 0;
    
    TxMessage2.ExtId   = EVSE_SECC_REBOOT_G1;
    TxMessage2.IDE     = CAN_ID_EXT;
    TxMessage2.DLC     = 8;
    TxMessage2.Data[0] = 0x52;
    TxMessage2.Data[1] = 0x45;
    TxMessage2.Data[2] = 0x42;
    TxMessage2.Data[3] = 0x4F;
    TxMessage2.Data[4] = 0x4F;
    TxMessage2.Data[5] = 0x54;
    TxMessage2.Data[6] = 0x00;
    TxMessage2.Data[7] = 0x00;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
	
  default: 
    break;         
  }    
}

void PLC_3_0_Msg_Send_2CH(void) // 10ms polling, 충전기 메시지 보내기 , prime number 적용, 스위치 케이스문 적용으로 인해서 연산율 감소
{
  if(++EVSE_SECC_cnt_2CH_50ms_1 >= 5)
  {
    EVSE_SECC_cnt_2CH_50ms_1 = 0;
    EVSE_SECC_Period_2CH = 51;
  }
  if(++EVSE_SECC_cnt_2CH_50ms_2 >= 7)
  {
    EVSE_SECC_cnt_2CH_50ms_2 = 2;
  }
  if(++EVSE_SECC_cnt_2CH_200ms_1 >= 23)
  {
    EVSE_SECC_cnt_2CH_200ms_1 = 3;
    EVSE_SECC_Period_2CH = 200;
  }
  if(++EVSE_SECC_cnt_2CH_500ms_1 >= 53)
  {
    EVSE_SECC_cnt_2CH_500ms_1 = 3;
  }
  if(++EVSE_SECC_cnt_2CH_500ms_2 >= 59)
  {
    EVSE_SECC_cnt_2CH_500ms_2 = 9;
  }
  if(++EVSE_SECC_cnt_2CH_500ms_3 >= 61)
  {
    EVSE_SECC_cnt_2CH_500ms_3 = 11;
  }
 if(EVSE_SECC_cnt_2CH_reboot == 1)
  {
    EVSE_SECC_cnt_2CH_reboot = 0;
    EVSE_SECC_Period_2CH = 1000;
  }
  
  switch(EVSE_SECC_Period_2CH)
  {
  case 51:
    EVSE_SECC_Period_2CH = 0;
    
    TxMessage2.ExtId   = EVSE_STATUS_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_Status.evseHeartbeat_0++;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_Status.chargingControl_1;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_Status.notificationMaxDelay_23;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_Status.notificationMaxDelay_23 >> 8;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_Status.evseIsolationStatus_4;
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_Status.Reserved_5;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_Status.Processing_trgger_6.all;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_Status.LimitAchieved_7.all;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_PRESENT_VOLTAGE_CURRENT_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_Present_Voltage_Current.evsePresentVoltage;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_Present_Voltage_Current.evsePresentVoltage >> 8;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_Present_Voltage_Current.evsePresentCurrent;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_Present_Voltage_Current.evsePresentCurrent >> 8;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_Present_Voltage_Current.Reserved_45;
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_Present_Voltage_Current.Reserved_45 >> 8;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_Present_Voltage_Current.Reserved_67;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_Present_Voltage_Current.Reserved_67 >> 8;
    CAN_Transmit(CAN2, &TxMessage2);    
    break;
    
  case 52:
    EVSE_SECC_Period_2CH = 0;
    
    TxMessage2.ExtId   = EVSE_DIN_ID_1_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_DIN_ID1.evseDinId1_High;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_DIN_ID1.evseDinId1_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_DIN_ID1.evseDinId1_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_DIN_ID1.evseDinId1_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_DIN_ID1.evseDinId1_Low;
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_DIN_ID1.evseDinId1_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_DIN_ID1.evseDinId1_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_DIN_ID1.evseDinId1_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_DIN_ID_2_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_DIN_ID2.evseDinId2_High;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_DIN_ID2.evseDinId2_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_DIN_ID2.evseDinId2_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_DIN_ID2.evseDinId2_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_DIN_ID2.evseDinId2_Low; 
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_DIN_ID2.evseDinId2_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_DIN_ID2.evseDinId2_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_DIN_ID2.evseDinId2_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
        
  case 200:
    EVSE_SECC_Period_2CH = 0;
    
    TxMessage2.ExtId   = EVSE_CONFIGURATIONS_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_Configurations.Id_Payment_Option_0.all;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_Configurations.evseIdLength_DIN_1;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_Configurations.evseIdLength_ISO_2;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_Configurations.SupportedEnergyTransfer_3.all;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_Configurations.ServiceList_4.all;
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_Configurations.freeService_5.all;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_Configurations.saScheduleManagement_6;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_Configurations.Reserved_7;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_CHARGE_PARAMETERS_1_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_Charge_Parameters1.evseMaximumCurrentLimit_01;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_Charge_Parameters1.evseMaximumCurrentLimit_01 >> 8;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_Charge_Parameters1.evseMaximumPowerLimit_23;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_Charge_Parameters1.evseMaximumPowerLimit_23 >> 8;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_Charge_Parameters1.evseMaximumVoltageLimit_45;
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_Charge_Parameters1.evseMaximumVoltageLimit_45 >> 8;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_Charge_Parameters1.evsePeakCurrentRipple_67;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_Charge_Parameters1.evsePeakCurrentRipple_67 >> 8;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_CHARGE_PARAMETERS_2_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_Charge_Parameters2.evseMinimumCurrentLimit_01;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_Charge_Parameters2.evseMinimumCurrentLimit_01 >> 8;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_Charge_Parameters2.evseMinimumVoltageLimit_23;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_Charge_Parameters2.evseMinimumVoltageLimit_23 >> 8;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_Charge_Parameters2.evseCurrentRegulationTolerance_45;
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_Charge_Parameters2.evseCurrentRegulationTolerance_45 >> 8;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_Charge_Parameters2.evseEnergyToBeDelivered_67;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_Charge_Parameters2.evseEnergyToBeDelivered_67 >> 8;
    CAN_Transmit(CAN2, &TxMessage2);       
    break;
        
  case 501:
    EVSE_SECC_Period_2CH = 0;
    
    TxMessage2.ExtId   = EVSE_DIN_ID_3_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_DIN_ID3.evseDinId3_High;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_DIN_ID3.evseDinId3_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_DIN_ID3.evseDinId3_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_DIN_ID3.evseDinId3_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_DIN_ID3.evseDinId3_Low;
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_DIN_ID3.evseDinId3_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_DIN_ID3.evseDinId3_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_DIN_ID3.evseDinId3_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_DIN_ID_4_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_DIN_ID4.evseDinId4_High;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_DIN_ID4.evseDinId4_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_DIN_ID4.evseDinId4_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_DIN_ID4.evseDinId4_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_DIN_ID4.evseDinId4_Low;
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_DIN_ID4.evseDinId4_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_DIN_ID4.evseDinId4_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_DIN_ID4.evseDinId4_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_ISO_ID_1_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_ISO_ID1.evseIsoId1_High;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_ISO_ID1.evseIsoId1_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_ISO_ID1.evseIsoId1_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_ISO_ID1.evseIsoId1_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_ISO_ID1.evseIsoId1_Low;
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_ISO_ID1.evseIsoId1_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_ISO_ID1.evseIsoId1_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_ISO_ID1.evseIsoId1_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  case 502:
    EVSE_SECC_Period_2CH = 0;
    
    TxMessage2.ExtId   = EVSE_ISO_ID_2_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_ISO_ID2.evseIsoId2_High;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_ISO_ID2.evseIsoId2_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_ISO_ID2.evseIsoId2_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_ISO_ID2.evseIsoId2_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_ISO_ID2.evseIsoId2_Low;
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_ISO_ID2.evseIsoId2_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_ISO_ID2.evseIsoId2_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_ISO_ID2.evseIsoId2_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_ISO_ID_3_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_ISO_ID3.evseIsoId3_High;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_ISO_ID3.evseIsoId3_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_ISO_ID3.evseIsoId3_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_ISO_ID3.evseIsoId3_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_ISO_ID3.evseIsoId3_Low; 
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_ISO_ID3.evseIsoId3_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_ISO_ID3.evseIsoId3_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_ISO_ID3.evseIsoId3_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
  case 503:
    EVSE_SECC_Period_2CH = 0;
    
    TxMessage2.ExtId   = EVSE_ISO_ID_4_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_ISO_ID4.evseIsoId4_High;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_ISO_ID4.evseIsoId4_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_ISO_ID4.evseIsoId4_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_ISO_ID4.evseIsoId4_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_ISO_ID4.evseIsoId4_Low;
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_ISO_ID4.evseIsoId4_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_ISO_ID4.evseIsoId4_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_ISO_ID4.evseIsoId4_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    
    TxMessage2.ExtId   = EVSE_ISO_ID_5_G2;
    TxMessage2.Data[0] = EVSE_Msg[2].EVSE_ISO_ID5.evseIsoId5_High;
    TxMessage2.Data[1] = EVSE_Msg[2].EVSE_ISO_ID5.evseIsoId5_High >> 8;
    TxMessage2.Data[2] = EVSE_Msg[2].EVSE_ISO_ID5.evseIsoId5_High >> 16;
    TxMessage2.Data[3] = EVSE_Msg[2].EVSE_ISO_ID5.evseIsoId5_High >> 24;
    TxMessage2.Data[4] = EVSE_Msg[2].EVSE_ISO_ID5.evseIsoId5_Low; 
    TxMessage2.Data[5] = EVSE_Msg[2].EVSE_ISO_ID5.evseIsoId5_Low >> 8;
    TxMessage2.Data[6] = EVSE_Msg[2].EVSE_ISO_ID5.evseIsoId5_Low >> 16;
    TxMessage2.Data[7] = EVSE_Msg[2].EVSE_ISO_ID5.evseIsoId5_Low >> 24;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
    
	case 1000:
    EVSE_SECC_Period_2CH = 0;
    
    TxMessage2.ExtId   = EVSE_SECC_REBOOT_G2;
    TxMessage2.Data[0] = 0x52;
    TxMessage2.Data[1] = 0x45;
    TxMessage2.Data[2] = 0x42;
    TxMessage2.Data[3] = 0x4F;
    TxMessage2.Data[4] = 0x4F;
    TxMessage2.Data[5] = 0x54;
    TxMessage2.Data[6] = 0x00;
    TxMessage2.Data[7] = 0x00;
    CAN_Transmit(CAN2, &TxMessage2);
    break;
	
  default: 
    break;         
  }    
}


void SECC_Msg_Receive_1CH(void)
{
  
  switch(RxMessage2.ExtId)
  { 
  case SECC_STATUS1_G1: //50ms
    
    plc_can_communication_rx_fault_count_1CH = 0;
    
    SECC_Msg[1].SECC_Status_1.seccHeartbeat     =                       RxMessage2.Data[0];
    SECC_Msg[1].SECC_Status_1.seccStatusCode    = (_seccStatusCode)     RxMessage2.Data[1];
    SECC_Msg[1].SECC_Status_1.evErrorCode		= (_evErrorCode)        RxMessage2.Data[2];
    SECC_Msg[1].SECC_Status_1.seccErrorCode		= (_seccErrorCode)		RxMessage2.Data[3];
    SECC_Msg[1].SECC_Status_1.seccVersionMajor	=                       RxMessage2.Data[4];
    SECC_Msg[1].SECC_Status_1.seccVersionMinor	=                       RxMessage2.Data[5];
    SECC_Msg[1].SECC_Status_1.seccVersionPatch	=                       RxMessage2.Data[6];
    SECC_Msg[1].SECC_Status_1.Reserved			=                       RxMessage2.Data[7];
    
    if(sequence_step_1CH >= 999)
	{
		if(SECC_Msg[1].SECC_Status_1.seccStatusCode == EV_Init_Stop_Charging)			stop_code_1CH = 1;
		if(SECC_Msg[1].SECC_Status_1.seccStatusCode == EVSE_Init_Stop_Charging)			stop_code_1CH = 2;
		if(SECC_Msg[1].SECC_Status_1.seccStatusCode == SECC_ERROR)						stop_code_1CH = 4;
		if(SECC_Msg[1].SECC_Status_1.evErrorCode	== FAILED_RESSTemperatureInhibit)	stop_code_1CH = 40;
		if(SECC_Msg[1].SECC_Status_1.evErrorCode	== FAILED_EVShiftPosition)			stop_code_1CH = 41;
		if(SECC_Msg[1].SECC_Status_1.evErrorCode	== FAILED_ChargerConnectorLockFault)	stop_code_1CH = 42;
		if(SECC_Msg[1].SECC_Status_1.evErrorCode	== FAILED_EVRESSMalfunction)		stop_code_1CH = 43;
		
		if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargingCurrentdifferential)			stop_code_1CH = 44;
		if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargingVoltageOutOfRange)			stop_code_1CH = 45;
		if(SECC_Msg[1].SECC_Status_1.evErrorCode == Reserved_A)									stop_code_1CH = 46;
		if(SECC_Msg[1].SECC_Status_1.evErrorCode == Reserved_B)									stop_code_1CH = 47;
		if(SECC_Msg[1].SECC_Status_1.evErrorCode == Reserved_C)									stop_code_1CH = 48;
		if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargingVoltageOutOfRange)			stop_code_1CH = 49;
		if(SECC_Msg[1].SECC_Status_1.evErrorCode == NoData)										stop_code_1CH = 50;
			
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED)									stop_code_1CH = 6;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_SequenceError)						stop_code_1CH = 7;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ServiceIDInvalid)					stop_code_1CH = 8;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_UnknownSession)					Input_Memory[1].Fault2.bit.PLC_UnknownSession = 1;		   	    	//stop_code_1CH = 9;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ServiceSelectionInvalid)			Input_Memory[1].Fault2.bit.PLC_ServiceSelectionInvalid = 1;			//stop_code_1CH = 10;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_PaymentSelectionInvalid)			Input_Memory[1].Fault2.bit.PLC_PaymentSelectionInvalid = 1;			//stop_code_1CH = 11;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CertificateExpired)				stop_code_1CH = 12;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_SignatureError)					stop_code_1CH = 13;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_NoCertificateAvailable)			stop_code_1CH = 14;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CerChainError)						stop_code_1CH = 15;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ChallengeInvalid)					stop_code_1CH = 16;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ContractCanceled)					stop_code_1CH = 17;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_WrongChargeParameter)				Input_Memory[1].Fault2.bit.PLC_WrongChargeParameter = 1;		   		//stop_code_1CH = 18;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_PowerDeliveryNotApplied)			stop_code_1CH = 19;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_TariffSelectionInvalid)			Input_Memory[1].Fault2.bit.PLC_TariffSelectionInvalid = 1;			//stop_code_1CH = 20;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ChargingProfileInvalid)			Input_Memory[1].Fault2.bit.PLC_ChargingProfileInvalid = 1;	   		//stop_code_1CH = 21;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_MeteringSignatureNotValid)			stop_code_1CH = 22;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_NoChargeServiceSelected)			Input_Memory[1].Fault3.bit.PLC_NoChargeServiceSelected = 1;			//stop_code_1CH = 23;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_WrongEnergyTransferMode)			Input_Memory[1].Fault3.bit.PLC_WrongEnergyTransferMode = 1;			//stop_code_1CH = 24;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ContactorError)					stop_code_1CH = 25;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CertificateNotAllowedAtThisEVSE)	stop_code_1CH = 26;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CertificateRevoked)   				stop_code_1CH = 27;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_NoNegotiation)        				Input_Memory[1].Fault3.bit.FAILED_NoNegotiation = 1;		   	    	//stop_code_1CH = 28;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_CommunicationSetup)  				Input_Memory[1].Fault3.bit.PLC_TIMEOUT_CommunicationSetup = 1;			//stop_code_1CH = 29;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_Sequence)            				Input_Memory[1].Fault3.bit.PLC_TIMEOUT_Sequence = 1;		   	    	//stop_code_1CH = 30;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_NotificationMaxDelay)				Input_Memory[1].Fault3.bit.PLC_TIMEOUT_NotificationMaxDelay = 1;		//stop_code_1CH = 31;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_WeldingDetection)    				Input_Memory[1].Fault3.bit.TIMEOUT_WeldingDetection = 1;				//stop_code_1CH = 32;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_WrongCPLevel)         				Input_Memory[1].Fault3.bit.PLC_WrongCPLevel = 1;     					//stop_code_1CH = 33;
		
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ProximityError)                    stop_code_1CH = 34;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_HLCError)                          Input_Memory[1].Fault3.bit.PLC_HLCError = 1;    										//stop_code_1CH = 35;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_HeartbeatError)                    Input_Memory[1].Fault3.bit.PLC_HeartbeatError = 1;    										//stop_code_1CH = 36;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_EVSECANInit)                       Input_Memory[1].Fault3.bit.PLC_EVSECANInit = 1;    										//stop_code_1CH = 37;
		if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAULT_HPGPLinkDown)                       Input_Memory[1].ERROR2.bit.FAULT_HPGPLinkDown = 1;   										//stop_code_1CH = 38;
	}
	else Input_Memory[1].Fault3.all = 0;
    break;
    
  case SECC_STATUS2_G1:
    
    SECC_Msg[1].SECC_Status_2.CP_Status_0.all    = RxMessage2.Data[0];
    SECC_Msg[1].SECC_Status_2.cpVoltage_12       = (uint16_t)((RxMessage2.Data[2] << 8) | RxMessage2.Data[1]);
    SECC_Msg[1].SECC_Status_2.HPGPLink_3.all     = RxMessage2.Data[3];
    SECC_Msg[1].SECC_Status_2.Reserved_45        = (uint16_t)((RxMessage2.Data[5] << 8) | RxMessage2.Data[4]);
    SECC_Msg[1].SECC_Status_2.Reserved_67        = (uint16_t)((RxMessage2.Data[7] << 8) | RxMessage2.Data[6]);       
   
    if ((SECC_Msg[1].SECC_Status_2.cpVoltage_12 < 105) && (SECC_Msg[1].SECC_Status_2.cpVoltage_12 > 15))  {control_pilot_1CH = 1;}
    else                                                                                                  {control_pilot_1CH = 0;}
    break;
    
    
  case SECC_EV_SERVICE_SELECTION_G1:
    
    SECC_Msg[1].SECC_EV_Service_Selection.selectedChargingProtocol_0    = (_selectedChargingProtocol)   RxMessage2.Data[0];
    SECC_Msg[1].SECC_EV_Service_Selection.selectedPaymentOption_1       = (_selectedPaymentOption)      RxMessage2.Data[1];
    SECC_Msg[1].SECC_EV_Service_Selection.selectedService_2.all         =                               RxMessage2.Data[2];
    SECC_Msg[1].SECC_EV_Service_Selection.selectedEnergyTransferMode_3  = (_selectedEnergyTransferMode) RxMessage2.Data[3];
    SECC_Msg[1].SECC_EV_Service_Selection.selectedSAScheduleTupleID_4   =                               RxMessage2.Data[4];
    SECC_Msg[1].SECC_EV_Service_Selection.Reserved_56                   = (uint16_t)((RxMessage2.Data[6] << 8) | RxMessage2.Data[5]);
    SECC_Msg[1].SECC_EV_Service_Selection.Reserved_7                    =                               RxMessage2.Data[7];
    
    break;
    
    
  case SECC_SESSION_ID_G1:
    
    SECC_Msg[1].SECC_Session_ID.sessionID_High     = RxMessage2.Data[0] << 24;
    SECC_Msg[1].SECC_Session_ID.sessionID_High    |= RxMessage2.Data[1] << 16;
    SECC_Msg[1].SECC_Session_ID.sessionID_High    |= RxMessage2.Data[2] << 8;
    SECC_Msg[1].SECC_Session_ID.sessionID_High    |= RxMessage2.Data[3];
    SECC_Msg[1].SECC_Session_ID.sessionID_Low      = RxMessage2.Data[4] << 24;
    SECC_Msg[1].SECC_Session_ID.sessionID_Low     |= RxMessage2.Data[5] << 16;
    SECC_Msg[1].SECC_Session_ID.sessionID_Low     |= RxMessage2.Data[6] << 8;
    SECC_Msg[1].SECC_Session_ID.sessionID_Low     |= RxMessage2.Data[7];
    break;
    
    
  case SECC_EVCC_INFO_G1:
    SECC_Msg[1].SECC_EVCC_Info.evccMAC_0       = RxMessage2.Data[0];
    SECC_Msg[1].SECC_EVCC_Info.evccMAC_1       = RxMessage2.Data[1];
    SECC_Msg[1].SECC_EVCC_Info.evccMAC_2       = RxMessage2.Data[2];
    SECC_Msg[1].SECC_EVCC_Info.evccMAC_3       = RxMessage2.Data[3];
    SECC_Msg[1].SECC_EVCC_Info.evccMAC_4       = RxMessage2.Data[4];
    SECC_Msg[1].SECC_EVCC_Info.evccMAC_5       = RxMessage2.Data[5];
    SECC_Msg[1].SECC_EVCC_Info.evccAttn_6      = RxMessage2.Data[6];
    SECC_Msg[1].SECC_EVCC_Info.Reserved_7      = RxMessage2.Data[7];
    break;
    
    
  case SECC_EV_CHARGE_PARAMETERS_1_G1:
    
    SECC_Msg[1].SECC_EV_Charge_Parameters_1.maxEntriesSAScheduleTule_01        = (uint16_t)((RxMessage2.Data[1] << 8) | RxMessage2.Data[0]);
    
    SECC_Msg[1].SECC_EV_Charge_Parameters_1.departureTime_2345                 = RxMessage2.Data[2] << 24;
    SECC_Msg[1].SECC_EV_Charge_Parameters_1.departureTime_2345                |= RxMessage2.Data[3] << 16;
    SECC_Msg[1].SECC_EV_Charge_Parameters_1.departureTime_2345                |= RxMessage2.Data[4] << 8;
    SECC_Msg[1].SECC_EV_Charge_Parameters_1.departureTime_2345                |= RxMessage2.Data[5];
    
    SECC_Msg[1].SECC_EV_Charge_Parameters_1.evMaximumPowerLimit_67             = (uint16_t)((RxMessage2.Data[7] << 8)| RxMessage2.Data[6]);            
    break;
    
    
  case SECC_EV_CHARGE_PARAMETERS_2_G1:
    
    SECC_Msg[1].SECC_EV_Charge_Parameters_2.evMaximumCurrentLimit_01   = (uint16_t)((RxMessage2.Data[1] << 8) | RxMessage2.Data[0]);
    SECC_Msg[1].SECC_EV_Charge_Parameters_2.evMaximumVoltageLimit_23   = (uint16_t)((RxMessage2.Data[3] << 8) | RxMessage2.Data[2]);
    SECC_Msg[1].SECC_EV_Charge_Parameters_2.evEnergyCapacity_45        = (uint16_t)((RxMessage2.Data[5] << 8) | RxMessage2.Data[4]);
    SECC_Msg[1].SECC_EV_Charge_Parameters_2.evEnergyRequest_67         = (uint16_t)((RxMessage2.Data[7] << 8) | RxMessage2.Data[6]);
    break; 
    
    
  case SECC_EV_SOC_RELATED_PARAMETERS_G1:
    
    SECC_Msg[1].SECC_EV_SoC_Related_Parameters._Complete_Conditioning_0.all       = RxMessage2.Data[0];
    SECC_Msg[1].SECC_EV_SoC_Related_Parameters.evSOC_1                            = RxMessage2.Data[1];
    SECC_Msg[1].SECC_EV_SoC_Related_Parameters.fullSOC_2                          = RxMessage2.Data[2];
    SECC_Msg[1].SECC_EV_SoC_Related_Parameters.bulkSOC_3                          = RxMessage2.Data[3];
    SECC_Msg[1].SECC_EV_SoC_Related_Parameters.remainingTimeToFullSOC_45          = (uint16_t)((RxMessage2.Data[5] << 8) | RxMessage2.Data[4]);
    SECC_Msg[1].SECC_EV_SoC_Related_Parameters.remainingTimeToBulkSOC_67          = (uint16_t)((RxMessage2.Data[7] << 8) | RxMessage2.Data[6]);
    
    break;
    
    
  case SECC_EV_TARGET_VOLTAGE_CURRENT_G1:
    
    SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetVoltage_01   = (uint16_t)((RxMessage2.Data[1] << 8) | RxMessage2.Data[0]);
    SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetCurrent_23   = (uint16_t)((RxMessage2.Data[3] << 8) | RxMessage2.Data[2]);
    SECC_Msg[1].SECC_EV_TargetVoltage_Current.Reserved_45        = (uint16_t)((RxMessage2.Data[5] << 8) | RxMessage2.Data[4]);
    SECC_Msg[1].SECC_EV_TargetVoltage_Current.Reserved_67        = (uint16_t)((RxMessage2.Data[7] << 8) | RxMessage2.Data[6]); 
    break;
    
    
  case SECC_TIMESTAMP_G1:
    
    SECC_Msg[1].SECC_Timestamp.seccTimestamp_High     = RxMessage2.Data[0] << 24;
    SECC_Msg[1].SECC_Timestamp.seccTimestamp_High    |= RxMessage2.Data[1] << 16;
    SECC_Msg[1].SECC_Timestamp.seccTimestamp_High    |= RxMessage2.Data[2] << 8;
    SECC_Msg[1].SECC_Timestamp.seccTimestamp_High    |= RxMessage2.Data[3];
    
    SECC_Msg[1].SECC_Timestamp.seccTimestamp_Low      = RxMessage2.Data[4] << 24;
    SECC_Msg[1].SECC_Timestamp.seccTimestamp_Low     |= RxMessage2.Data[5] << 16;
    SECC_Msg[1].SECC_Timestamp.seccTimestamp_Low     |= RxMessage2.Data[6] << 8;
    SECC_Msg[1].SECC_Timestamp.seccTimestamp_Low     |= RxMessage2.Data[7];      
    
  default:
    break;
  }
}



