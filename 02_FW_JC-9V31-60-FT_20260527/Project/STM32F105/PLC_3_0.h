
#define __PLC_3_0_H

/*
  History
  3.0.0 Initial release.

  3.0.1 Multiple connector support.

  3.0.2 Modified
        -evseIsolationStatus description (0x15ECC002.4.0)
        Added
        -triggerStateF (0x15ECC002.6.6)
        -seccErrorCode = TIMEOUT_WeldingDetection (0x15ECC101.3.0 0x33)
        Removed
        -evCabinConditioning (0x15ECC108.0.4)
        -evRESSConditioning (0x15ECC108.0.6)

  3.0.3 Modified
        -evccID --> evccMAC (0x15ECC105.0.0)
        Added
        -evseAttnCorrection (0x15ECC001.7.0)
        Removed
        -triggerReceiptRequired (0x15ECC002.6.5)

  3.0.4 Added
        -environmentConfiguration (0x15ECC001.0.4)
        -SECC Reboot (0x15ECC0FF)
        -seccErrorCode (0x15ECC101.3.0)
        (1) FAILED_NoNegotiation
        (2) FAULT_HPGPLinkDown
        -hpgpLinkStatus (0x15ECC102.2.0)
        -evccAttn (0x15ECC105.6.0)

        Modified
        -evccAttnCorrection (Removed)
        -notificationMaxDelay:default 5s --> 60s (0x15ECC002.2.0)
        -SECC Status --> SECC Status1 (0x15ECC101)
        -seccErrorCode:TIMEOUT_SLACSDP --> TIMEOUT_CommunicationSetup (0x15ECC101.3.0)
        -SECC CP/PP Status --> SECC Status2 (0x15ECC102)

*/

#include "stdint.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//EVSE -> SECC (송신) ID1
#define EVSE_STATUS_G1                           (0x15ECC002)//50ms
#define EVSE_PRESENT_VOLTAGE_CURRENT_G1          (0x15ECC005)//50ms
#define EVSE_DIN_ID_1_G1                         (0x15ECC006)//50ms          //Optional
#define EVSE_DIN_ID_2_G1                         (0x15ECC007)//50ms          //Optional

#define EVSE_CONFIGURATIONS_G1                   (0x15ECC001)//200ms
#define EVSE_CHARGE_PARAMETERS_1_G1              (0x15ECC003)//200ms
#define EVSE_CHARGE_PARAMETERS_2_G1              (0x15ECC004)//200ms

#define EVSE_DIN_ID_3_G1                         (0x15ECC008)//500ms         //Optional
#define EVSE_DIN_ID_4_G1                         (0x15ECC009)//500ms         //Optional
#define EVSE_ISO_ID_1_G1                         (0x15ECC00A)//500ms         //Optional, for ISO 15118
#define EVSE_ISO_ID_2_G1                         (0x15ECC00B)//500ms         //Optional, for ISO 15118
#define EVSE_ISO_ID_3_G1                         (0x15ECC00C)//500ms         //Optional, for ISO 15118
#define EVSE_ISO_ID_4_G1                         (0x15ECC00D)//500ms         //Optional, for ISO 15118
#define EVSE_ISO_ID_5_G1                         (0x15ECC00E)//500ms         //Optional, for ISO 15118

#define EVSE_TIMESTAMP_G1                        (0x15ECC00F)//EVENT         //Optional
#define EVSE_SECC_REBOOT_G1                      (0x15ECC0FF)//EVENT 

//EVSE -> SECC (송신) ID2
#define EVSE_STATUS_G2                           (0x15ECC202)//50ms
#define EVSE_PRESENT_VOLTAGE_CURRENT_G2          (0x15ECC205)//50ms
#define EVSE_DIN_ID_1_G2                         (0x15ECC206)//50ms          //Optional
#define EVSE_DIN_ID_2_G2                         (0x15ECC207)//50ms          //Optional

#define EVSE_CONFIGURATIONS_G2                   (0x15ECC201)//200ms
#define EVSE_CHARGE_PARAMETERS_1_G2              (0x15ECC203)//200ms
#define EVSE_CHARGE_PARAMETERS_2_G2              (0x15ECC204)//200ms

#define EVSE_DIN_ID_3_G2                         (0x15ECC208)//500ms         //Optional
#define EVSE_DIN_ID_4_G2                         (0x15ECC209)//500ms         //Optional
#define EVSE_ISO_ID_1_G2                         (0x15ECC20A)//500ms         //Optional, for ISO 15118
#define EVSE_ISO_ID_2_G2                         (0x15ECC20B)//500ms         //Optional, for ISO 15118
#define EVSE_ISO_ID_3_G2                         (0x15ECC20C)//500ms         //Optional, for ISO 15118
#define EVSE_ISO_ID_4_G2                         (0x15ECC20D)//500ms         //Optional, for ISO 15118
#define EVSE_ISO_ID_5_G2                         (0x15ECC20E)//500ms         //Optional, for ISO 15118

#define EVSE_TIMESTAMP_G2                        (0x15ECC20F)//EVENT         //Optional
#define EVSE_SECC_REBOOT_G2                      (0x15ECC2FF)//EVENT 

//SECC -> EVSE (수신) ID1
#define SECC_STATUS1_G1                          (0x15ECC101)//50ms          
#define SECC_STATUS2_G1                          (0x15ECC102)//50ms         
#define SECC_EV_SERVICE_SELECTION_G1             (0x15ECC103)//100ms
#define SECC_SESSION_ID_G1                       (0x15ECC104)//500ms
#define SECC_EVCC_INFO_G1                        (0x15ECC105)//500ms         
#define SECC_EV_CHARGE_PARAMETERS_1_G1           (0x15ECC106)//200ms
#define SECC_EV_CHARGE_PARAMETERS_2_G1           (0x15ECC107)//200ms
#define SECC_EV_SOC_RELATED_PARAMETERS_G1        (0x15ECC108)//50ms
#define SECC_EV_TARGET_VOLTAGE_CURRENT_G1        (0x15ECC109)//50ms
#define SECC_TIMESTAMP_G1                        (0x15ECC10A)//1000ms

//SECC -> EVSE (수신) ID2
#define SECC_STATUS1_G2                          (0x15ECC301)//50ms          
#define SECC_STATUS2_G2                          (0x15ECC302)//50ms         
#define SECC_EV_SERVICE_SELECTION_G2             (0x15ECC303)//100ms
#define SECC_SESSION_ID_G2                       (0x15ECC304)//500ms
#define SECC_EVCC_INFO_G2                        (0x15ECC305)//500ms         
#define SECC_EV_CHARGE_PARAMETERS_1_G2           (0x15ECC306)//200ms
#define SECC_EV_CHARGE_PARAMETERS_2_G2           (0x15ECC307)//200ms
#define SECC_EV_SOC_RELATED_PARAMETERS_G2        (0x15ECC308)//50ms
#define SECC_EV_TARGET_VOLTAGE_CURRENT_G2        (0x15ECC309)//50ms
#define SECC_TIMESTAMP_G2                        (0x15ECC30A)//1000ms

        //Description
#define Not_supported                   0
#define Supported                       1
#define AC_single_phase_Supported       1
#define AC_three_phase_Supported        1
#define DC_core_Supported               1
#define DC_extended_Supported           1
#define DC_combo_core_Supported         1
#define DC_unique_Supported             1
#define Payed_Service                   0
#define Free_Service                    1
#define Ongoing                         0
#define Finished                        1
#define Not_active                      0
#define Active                          1
#define None                            0       //EVSE Status 0x15ECC002.6.4
#define Request_ReNefotiation           1       //EVSE Status 0x15ECC002.6.4
#define Not_required                    0       //Removed for 0x15ECC002.6.5
#define Required                        1       //Removed for 0x15ECC002.6.5
#define FALSE                           0       //Down for 0x15ECC102.3.0
#define TRUE                            1       //Up for 0x15ECC102.3.0
#define Not_selected                    0
#define Selected                        1
#define Disabled                        0
#define Enabled                         1

//Sequence
#define Sequence_Charging           1075
#define Sequence_Stop               1500 // EVSE STOP
#define Sequence_Normal_Stop        1600 // (SECC)EV STOP
#define Sequence_Emergency_Stop     1700

/*
#define     IDLE_WAIT                            (0)
#define     IDLE                                 (1)
#define     READY                                (2)
#define     LOW_LEVEL_COMM                       (3)
#define     HIGH_LEVEL_COMM                      (4)
#define     AUTHORIZATION_CONTRACTAUTHENTICATION (5)
#define     CHARGEPARAMETERDISCOVER              (6)
#define     CABLECHECK                           (7)
#define     PRECHARGE                            (8)
#define     CHARGING                             (9)
#define     RENEGOTIATION                        (10)
#define     EV_INIT_STOP_CHARGING                (11)
#define     EVSE_INIT_STOP_CHARGING              (12)
#define     PAUSE                                (13)
#define     TERMINATE                            (14)
#define     CERTIFICATEINSTALLATION              (15)
#define     CERTIFICATEDUPDATE                   (16)
#define     SECC_ERROR                           (17)
*/
////------------------------------- SECC -> EVSE -------------------------------////
////------------------------------------ RX ------------------------------------////
typedef enum
{
    SECC_Default                   = 0,
    SECC_Status1                   = 1,
    SECC_Status2                   = 2,
    SECC_EV_Service_Selection      = 3,
    SECC_Session_ID                = 4,
    SECC_EVCC_Info                 = 5,
    SECC_EV_Charge_Parameters_1    = 6,
    SECC_EV_Charge_Parameters_2    = 7,
    SECC_EV_SoC_Related_Parameters = 8,
    SECC_EV_Target_Voltage_Current = 9,
    SECC_Timestamp                 = 10
}eplc_data;

extern eplc_data ePLC_DATA;

//SECC Status1
typedef enum
{
    /*Note
    Regotiation, Pause, CertificateInstallation, CertificateUpdate -> Not used in DIN
    
    CertificateInstallation, CertificateUpdate -> Reserved for PnC
    */
    Idle_Wait = 0x00,                                   //State E/F. chargingControl = "Initialize parameters" triggers "Idle" state
    Idle = 0x01,                                        //State E/F. Parameters initialized and ready to charge / chargingControl = "Start charging" triggers "Ready" state.
    Ready = 0x02,                                       //State F or A/B. Charging sequence shall begin immediately with plug-in
    Low_Level_Comm = 0x03,                      
    High_Level_Comm = 0x04,
    Authorization_ContractAuthentication = 0x05,
    ChargeParameterDiscovery = 0x06,
    CableCheck = 0x07,
    PreCharge = 0x08,
    Charging = 0x09,
    Renegotiation = 0x0A,
    EV_Init_Stop_Charging = 0x0B,
    EVSE_Init_Stop_Charging = 0x0C,
    Pause = 0x0D,
    Terminate = 0x0E,                                   //State A/B. Changes to "Idle Wait" after timeout.
    CertificateInstallation = 0x0F,
    CertificatedUpdate = 0x10,
    SECC_ERROR = 0x11                                   //Changes to "Terminate" after timeout.
}_seccStatusCode;

typedef enum
{
    EV_NO_ERROR                          = 0,                                 //Default value, when EVCC has no Error detected
    FAILED_RESSTemperatureInhibit        = 1,               //Battery Temperature Inhibit, Battery too hot/cold to accept charge.
    FAILED_EVShiftPosition               = 2,                      //Vehicle Shift Position, Vehicle is not in Park.
    FAILED_ChargerConnectorLockFault     = 3,            //Vehicle has not detected the Charge cord connector locked into the inlet or failure where connector cannot be unlocked from the charging inlet.
    FAILED_EVRESSMalfunction             = 4,                    //Vehivle RESS Malfunction, Any non-recoverable fault or error condition of the Vehivle RESS.
    FAILED_ChargingCurrentdifferential   = 5,          //Indication that vehicle has stopped the charging session after detecting that the chargig station is not able to maintain the current request.
    FAILED_ChargingVoltageOutOfRange     = 6,            //Indication that vehicle has stopped the charging session after detecting that the RESS is either under or above normal operating voltage range.
    Reserved_A                           = 7,
    Reserved_B                           = 8,
    Reserved_C                           = 9,
    FAILED_ChargingSystemIncompatibility = 10,        //If the vehicle determines that the charging station is incompatible. Using this value is optional; as an alternative, the vehicle can use EVReady in DC_EVStatusType equal to "FALSE"
    NoData                               = 11                                       //Only used when vehicle has not yet determined its operating state.
}_evErrorCode;

typedef enum
{
    /*Note
    0x07 - 12, 0x17, 0x21, 0x22 -> Reserved for PnC
    0x20 -> Reserved for AC
    */
    SECC_NO_ERROR                          = 0,                               //Default value, when PEPPERMINT has no Error detected.
    FAILED                                 = 1,                                      //Reserved
    FAILED_SequenceError                   = 2,                        //PEPPERMINT has received an unexpected request message from the EVCC.
    FAILED_ServiceIDInvalid                = 3,
    FAILED_UnknownSession                  = 4,                       //The SessionID in any request message except the SessionSetupReq message sent by the EVCC is not equal to the SessionID randomly selected by PEPPERMINT.
    FAILED_ServiceSelectionInvalid         = 5,
    FAILED_PaymentSelectionInvalid         = 6,              //The selectedPaymentOption in the paymentServiceSelectedReq message sent by the EVCC was not offered in the supportedPaymentOption list by the EVSE.
    FAILED_CertificateExpired              = 7,
    FAILED_SignatureError                  = 8,
    FAILED_NoCertificateAvailable          = 9,
    FAILED_CerChainError                   = 10,
    FAILED_ChallengeInvalid                = 11,
    FAILED_ContractCanceled                = 12,
    FAILED_WrongChargeParameter            = 13,                 //The EVChargeParameter in the ChargeParameterDiscoveryReq sent by the EVCC is not valid. E.g. Wrong parameter set, the maximum limit value of the EV is smaller than the minimum limit value of the EVSE.
    FAILED_PowerDeliveryNotApplied         = 14,              //Reserved
    FAILED_TariffSelectionInvalid          = 15,               //The SAtupleID in the PowerDeliveryReq message sent by the EVCC is invalid.
    FAILED_ChargingProfileInvalid          = 16,               //The ChargingProfile in the PowerDeliveryReq message sent by the EVCC violates the power limitations provided in the ChargingSchedule presented by PEPPERMINT.
    FAILED_MeteringSignatureNotValid       = 17,
    FAILED_NoChargeServiceSelected         = 18,              //The selectedService list in the PaymentServiceSelectionReq message sent by the EVCC does not contan the mandatory "EVCharging" service.
    FAILED_WrongEnergyTransferMode         = 19,              //The selectedEnergyTransferMode in the ChargeParameterDiscoveryReq message sent by the EVCC is not valid. E.g. Inconsistent with the supportedEnergyTransferMode list sent by PEPPERMINT.
    FAILED_ContactorError                  = 20,
    FAILED_CertificateNotAllowedAtThisEVSE = 21,
    FAILED_CertificateRevoked              = 22,
    FAILED_NoNegotiation                   = 23,                        //                       
    TIMEOUT_CommunicationSetup             = 30,                  //PEPPERMINT did not receive SessionSetupReq message from the EVCC after HPGP link up detection for 20s.
    TIMEOUT_Sequence                       = 31,                            //PEPPERMINT did not receive any request message from the EVCC for 60s.
    TIMEOUT_NotificationMaxDelay           = 32,                //The EVCC failed to perform the appropriate action within the NotificationMaxDelay timeout in response to the EVSENotification flag sent by PEPPERMINT. E.g. StopCharging, ReNegotiation.
    TIMEOUT_WeldingDetection               = 33,                    //
    FAILED_WrongCPLevel                    = 40,                         //The measured CP voltage level from PEPPERMINT did not match the defined voltage range of the current charging state. May also indicate E-Stop from the EV from "CableCheck" to "Charging"
    FAILED_ProximityError                  = 41,                       //Reserved
    FAILED_HLCError                        = 42,                             //An error occured during data interpretation. It may also indicate E-Stop from the EV from "High Level Comm." to "ChargeParameterDiscovery"
    FAILED_HeartbeatError                  = 43,                       //The EVSE Controller failed to update the evseHeartbeat value for 500ms.
    FAILED_EVSECANInit                     = 44,                          //The EVSE Controller failed the CAN data verification during "Ready" state.
    FAULT_HPGPLinkDown                     = 45                           //
}_seccErrorCode; //ResponseCode

typedef struct
{
    uint8_t             seccHeartbeat;        //binary //0~255 //SECC heartbeat. Cycles through 0 to 255.
    _seccStatusCode     seccStatusCode;       //enum type  열거형
    _evErrorCode        evErrorCode;          //enum type  열거형
    _seccErrorCode      seccErrorCode;        //enum type  열거형
    uint8_t             seccVersionMajor;     //byte //3       
    uint8_t             seccVersionMinor;     //byte //0
    uint8_t             seccVersionPatch;     //byte //0
    uint8_t             Reserved;             
//    uint32_t            CNT;
}_S_01;



//SECC Status2
struct __CP_Status
{
    uint8_t             cpOscillator:1;         //0
    uint8_t             Reserved:7;             //1234567
};

typedef union
{
    uint8_t                           all;
    struct              __CP_Status   bit;
}_CP_Status;

struct __HPGPLink
{
    uint8_t             Status:1;       //0
    uint8_t             Reserved:7;     //1234567
};

typedef union
{
    uint8_t                               all;
    struct              __HPGPLink        bit;
}_HPGPLink;

typedef struct
{
    _CP_Status          CP_Status_0;     //FALSE(OFF), TRUE(ON)
    short               cpVoltage_12;    //-32768~32767 //0.1V //Control pilot voltage level.
    _HPGPLink           HPGPLink_3;      //FALSE(DOWN), TRUE(UP)   
    uint16_t            Reserved_45;
    uint16_t            Reserved_67;
//    uint32_t            CNT;
}_S_02;

//SECC EV Service Selection
typedef enum
{
    /*Note
    Default = 0xFF
    selected during "High Level Comm."
    */
    DIN_SPEC_70121_2014 = 0,
    ISO_15118_2_2013    = 1,
    ISO_15118_2_2016    = 2,
    Charing_default     = 0xFF
}_selectedChargingProtocol;

typedef enum
{
    /*Note
    Default = 0xFF
    Selected during "ChargeParameterDiscovery"
    */
    Contract        = 0,
    ExternalPayment = 1,
    Payment_default = 0xFF
}_selectedPaymentOption;

struct __selectedService
{
    /*Note
    Selected during "High Level Comm."
    */
    uint8_t     EVCharging   :1; //0
    uint8_t     Internet     :1; //1
    uint8_t     ContractCert :1; //2
    uint8_t     OtherCustom  :1; //3
    uint8_t     Reserved     :4; //4567
};

typedef union
{
    uint8_t                          all;
    struct      __selectedService    bit;
}_selectedService;

typedef enum
{      
    /*Note
    Default = 0xFF
    Selected during "ChargeParameterDiscovery"
    */

    AC_single_phase_core = 0,
    AC_three_phase_core  = 1,
    DC_core              = 2,
    DC_extended          = 3,
    DC_combo_core        = 4,
    DC_unique            = 5,
    Mode_default         = 0xFF
}_selectedEnergyTransferMode;

typedef struct
{
    _selectedChargingProtocol           selectedChargingProtocol_0;          
    _selectedPaymentOption              selectedPaymentOption_1;          
    _selectedService                    selectedService_2;                
    _selectedEnergyTransferMode         selectedEnergyTransferMode_3;     
    uint8_t                             selectedSAScheduleTupleID_4;    //unsignedByte //1~255 //Selected during "Charging"
    uint16_t                            Reserved_56;
    uint8_t                             Reserved_7;                     
//    uint32_t                            CNT;
}_S_03;

//SECC Session ID
typedef struct
{
    /*Note
    After SessionSetupRes
    hexBinary
    */
    uint32_t    sessionID_High;
    uint32_t    sessionID_Low;
//    uint32_t    CNT;
}_S_04;

//SECC EVCC Info
typedef struct
{
    /*Note
    MAC address of the EVCC.
    In case of DIN, max lenght is 8, while 6 in ISO.
    hexBinary
    */
  
    //evccID -> evccMAC, evccAttn
    //evccAttn, SLAC attenuation result. 0: -50dBm/Hz, 25: -75dBm/Hz, 50: -100dBm/Hz
    //Refer to DIN SPEC 70121 8.3.7.4.1 or ISO 15118-3 A.11.4.1
  
    uint8_t     evccMAC_0;
    uint8_t     evccMAC_1;
    uint8_t     evccMAC_2;
    uint8_t     evccMAC_3;
    uint8_t     evccMAC_4;
    uint8_t     evccMAC_5;
    uint8_t     evccAttn_6;
    uint8_t     Reserved_7;
    //uint32_t    evccID_High;
    //uint32_t    evccID_Low;
//    uint32_t    CNT;
}_S_05;

//SECC EV Charge Parameters 1
typedef struct
{
    /*Note
    //01        //Default = 12                   //unsignedShort 
                Indicates the maximal number of entries in the SAScheduleTuple (applies for both Pmax and Tariff).
                The EVSE can transmit up to the maximum number of entries defined in the parameter.
    //2345      //Default = 86400                //unsignedInt 
                This element is used to indicate when the vehicle intends to finish the charging process.
                Offset in seconds from the point in time of sending this message.
    //67        //Default = 0 (Not yet received) //short 
                Maximum power supported by the EV
    */
    uint16_t    maxEntriesSAScheduleTule_01;    //1~1024    
    uint32_t    departureTime_2345;             //0~4294967295 //s
    uint16_t    evMaximumPowerLimit_67;         //0~20000      //10w
//    uint32_t    CNT;
}_S_06;

//SECC EV Charge Parameters 2
typedef struct
{
    /*Note
    //0~7 //Default = 0 (Not yet received)
    */
    uint16_t    evMaximumCurrentLimit_01;  //01 //short //0~4000  //0.1A   //Maximum current supported by the EV
    uint16_t    evMaximumVoltageLimit_23;  //23 //short //0~10000 //0.1A   //Maximum voltage supported by the EV
    uint16_t    evEnergyCapacity_45;       //45 //short //0~20000 //10Wh   //Energy capacity of the EV
    uint16_t    evEnergyRequest_67;        //67 //short //0~20000 //10Wh   //Amount of energy the EV requests from the EVSE.
//    uint32_t    CNT;
}_S_07;

//SECC EV SoC Related Parameters
typedef enum
{
    Not_complete  = 0,
    Complete      = 1,
    Complete_NONE = 3
}_Complete;

typedef enum
{
    Off = 0,
    On = 1,
    Conditioning_NONE = 3
}_Conditioning;

struct  __Complete_Conditioning
{
    _Complete           chargingComplete:2;     //Default = 3(Not yet received)
    _Complete           bulkChargingComplete:2; //Default = 3(Not yet received)
    //_Conditioning       evCabinConditioning:2;  //Default = 3(Not yet received) //Only used in DIN //The EV informs whether is uses passanger A/C or heating system during charging.  
    //_Conditioning       evRESSConditioning:2;   //Default = 3(Not yet received) //Only used in DIN //The EV informs whether is uses A/C or heating system for the battery pack during charging. 
    uint8_t             Reserved:4;             //4567
};

typedef union
{
    uint8_t                                all;
    struct      __Complete_Conditioning    bit;
}_Complete_Conditioning;

typedef struct
{
    _Complete_Conditioning      _Complete_Conditioning_0; 
    uint8_t                     evSOC_1;                        //byte  //Default = 0(Not yet received) 0~100%          //State of charge of the EV's battery (RESS)
    uint8_t                     fullSOC_2;                      //byte  //Default = 0(Not yet received) 0~100%          //SoC at which the EV considers to be fully charged.
    uint8_t                     bulkSOC_3;                      //byte  //Default = 0(Not yet received) 0~100%          //SoC at which the EV considers a CC charging process to end.
    uint16_t                    remainingTimeToFullSOC_45;      //short //Default = 0(Not yet received) 0~17280, 10s    //Estimated or calculated time until full charge (100% SOC) is complete
    uint16_t                    remainingTimeToBulkSOC_67;      //short //Default = 0(Not yet received) 0~17280, 10s    //Estimated or calculated time unitl bulk charge (approx. 80% SOC) is complete
//    uint32_t                    CNT;
}_S_08;

//SECC EV TargetVoltage/Current
typedef struct
{
    uint16_t    targetVoltage_01;       //short //0~10000 //0.1V   //Target Voltage requested by EV
    uint16_t    targetCurrent_23;       //short //0~4000  //0.1A   //Current demanded by EV
    uint16_t    Reserved_45;            
    uint16_t    Reserved_67;            
//    uint32_t    CNT;
}_S_09;

//SECC Timestamp
typedef struct
{
    /*Note
    Unix Time Stamp format
    Default : 1970-01-01
    */
    uint32_t    seccTimestamp_High;
    uint32_t    seccTimestamp_Low;
//    uint32_t    CNT;
}_S_0A;

typedef struct
{
    CanRxMsg    CanRxMessage;
}SCan;

typedef struct
{
    _S_01       SECC_Status_1;
    _S_02       SECC_Status_2;
    _S_03       SECC_EV_Service_Selection;
    _S_04       SECC_Session_ID;
    _S_05       SECC_EVCC_Info;
    _S_06       SECC_EV_Charge_Parameters_1; // 확인할 것
    _S_07       SECC_EV_Charge_Parameters_2; // 확인할 것
    _S_08       SECC_EV_SoC_Related_Parameters;
    _S_09       SECC_EV_TargetVoltage_Current;
    _S_0A       SECC_Timestamp;
}SECC_MSG;



////------------------------------- EVSE -> SECC -------------------------------////
////------------------------------------ TX ------------------------------------////
typedef enum
{
    EVSE_Default = 0,
    EVSE_Configurations = 1,
    EVSE_Status = 2,
    EVSE_Charge_Parameters1 = 3,
    EVSE_Charge_Parameters2 = 4,
    EVSE_Present_Voltage_Current = 5,
    EVSE_DIN_ID1 = 6,
    EVSE_DIN_ID2 = 7,
    EVSE_DIN_ID3 = 8,
    EVSE_DIN_ID4 = 9,
    EVSE_ISO_ID1 = 10,
    EVSE_ISO_ID2 = 11,
    EVSE_ISO_ID3 = 12,
    EVSE_ISO_ID4 = 13,
    EVSE_ISO_ID5 = 14,
    EVSE_Timestamp = 15
}ECharger_Data;

//extern ECharger_Data    eCharger_Data;

//EVSE Configurations
struct    __Id_Payment_Option
{
    /*
    environmentConfiguration
    00          / (Legacy, Default) ISO/DIN selection based on EVCC's priority
    01~14       / (Reserved)
    15          / (Force DIN) Force DIN
    */
    
    uint8_t     evseIdConfig_DIN:1;             //0     //0 Default = 00        //1 (Custom EVSE ID, must conform to DIN SPEC 91286.)
    uint8_t     evseIdConfig_ISO:1;             //1     //0 Default = zz00000)  //1 (Custom EVSE ID, must conrorm to IETF RFC 5234 and ISO 15118-2 Annex H.) //FAULT_EVSECANInit
    uint8_t     supportedPaymentOption_PnC:1;   //2     //At least one payment option must be selected. FAULT_EVSECANInit
    uint8_t     supportedPaymentOption_EIM:1;   //3     //At least one payment option must be selected. FAULT_EVSECANInit
    uint8_t     environmentConfiguration:1;     //4          
    uint8_t     Reserved:3;                     //567
};

typedef union
{
    uint8_t                               all;
    struct      __Id_Payment_Option       bit;
}_Id_Payment_Option;

struct  __SupportedEnergyTransfer
{
    uint8_t     AC1:1;          //0     //Reserved for AC
    uint8_t     AC3:1;          //1     //Reserved for AC
    uint8_t     DCcore:1;       //2     //At least one energy transfer mode must be selected. FAULT_EVSECANInit
    uint8_t     DCextd_CCS:1;   //3     //At least one energy transfer mode must be selected. FAULT_EVSECANInit
    uint8_t     DCcc:1;         //4     //At least one energy transfer mode must be selected. FAULT_EVSECANInit
    uint8_t     DCunique:1;     //5     //At least one energy transfer mode must be selected. FAULT_EVSECANInit
    uint8_t     Reserved:2;     //67
};

typedef union
{
    uint8_t                                   all;
    struct      __SupportedEnergyTransfer     bit;
}_SupportedEnergyTransfer;

struct  __ServiceList
{
    uint8_t     Internet:1;         //0     //Reserved for PnC //ServiceDetails of 'Internet' is provided by the SECC.
    uint8_t     ContractCert:1;     //1     //Reserved for PnC //ServiceDetails of 'ContractCertificate' is provided by the SECC.
    uint8_t     OtherCustom:1;      //2     //Reserved for PnC //Additional services are customizable upon request.
    uint8_t     Reserved:5;         //34567
};

typedef union
{
    uint8_t                       all;
    struct      __ServiceList     bit;
}_ServiceList;

struct  __freeService
{
    uint8_t     EVCharging:1;       //0
    uint8_t     Internet:1;         //1     //Reserved for PnC
    uint8_t     ContractCert:1;     //2     //Reserved for PnC
    uint8_t     OtherCustom:1;      //3     //Reserved for PnC
    uint8_t     Reserved:4;         //4567
};

typedef union
{
    uint8_t                       all;
    struct      __freeService     bit;
}_freeService;

typedef enum
{
    /*Note
    //00 Default          //The SECC uses EVSEMaximumPowerLimit as PMax and provides 24h schedule to the EV.
                          //It also utilizes DepartureTime and number of schedule entries provided by the EV.
    //01 Custom DB / OCPP //The EVSE Controller uses a custom DB for static/dynamic configuration. e.g.
                          //Gridwiz OCPP client receives the charging schedule.
    //Not used in DIN. Refer to the PEPPERMINT 3.0 Application Guide for custom DB usage.
    */
  
    Default = 0x00,
    CustomDB_OCPP = 0x01
}_saScheduleManagement;

typedef struct
{
//    ECharger_Data               eCharger_Data;
    _Id_Payment_Option          Id_Payment_Option_0;    
    uint8_t                     evseIdLength_DIN_1;     //byte //1~32 //FAULT_EVSECANInit
    uint8_t                     evseIdLength_ISO_2;     //byte //7~37 //FAULT_EVSECANInit
    _SupportedEnergyTransfer    SupportedEnergyTransfer_3;
    _ServiceList                ServiceList_4;            
    _freeService                freeService_5;            
    _saScheduleManagement       saScheduleManagement_6;   
    uint8_t                     Reserved_7;             
//    uint32_t                    CNT;
}_E_01;

//EVSE Status
typedef enum
{
    //"Start charging" could only be triggered at "Idle" state. N-stop, E-stop can only be triggered after "Ready" state.
    NONE                          = 0,
    Initialize_parameters         = 1, //Initializes charging parameters and triggers seccStatusCode = Ready
    Start_charging                = 2, //Changes CP state from E/F to A/B and wait for plug-in
    EVSE_initiated_normal_stop    = 3, //Signals the EV to stop charging and proceeds to stop sequence. Forces emergency stop after notification max delay.
    EVSE_initiated_emergency_stop = 4  //Immediately turns off CP oscillator and halts V2G communication.
}_chargingControl;

typedef enum
{
    //Initialize to "Invalid" after each charge session.
    //After CableCheck (IMD), return the result. For further details, refer to IEC 61851-23. FAULT_EVSECANInit
  
    //The EVSE should consistently perform the isolation monitoring during "CableCheck", "PreCharge", and "Charging".
    //The SECC will initiated an E-stop upon receiving "Fault".
  
    //For further details, refer to IEC 61851-23 and CharIN documents.
  
    //FAULT_EVSECANInit
    Invalid     = 0, //An isolation test has not been carried out.
    Valid       = 1, //The isolation test has been carried out successfully and did not result in an isolation warning or fault.
    Warning     = 2, //The measured isolation resistance is below the warning level.
    Fault       = 3, //E-Stop due to isolation fault, The measured isolation resistance is below the fault level.
    No_IMD_NONE = 4  //ISO : No_IMD, DIN : NONE / The EVSE does not contain an IMD and therefore cannot carry out an isolation test.
}_evseIsolationStatus;

struct  __Processing_trgger
{
    uint8_t     evseProcessing_AuthEIM:1;       //0  //User authorization(RFID, APP,WEB) is finished. In case of PnC(Contract), the SECC will automatically authorize the contract and proceed to the next sequence. Initialize to "Ongoing" after each charge session.         //FAULT_EVSECANInit
    uint8_t     evseProcessing_CPD:1;           //1  //Charging parameter configuration finished by the EVSE or the Charge Point Operator. This also indicates that saScheduleManagement by the EVSE Controller is complete. Initialize to "Ongoing" after each charge session. //FAULT_EVSECANInit
    uint8_t     evseProcessing_CableCheck:1;    //2  //IMD finished and result can be found in evseIsolationStatus. Initialize to "Ongoing" after each charge session. //FAULT_EVSECANInit 
    uint8_t     isolationMonitoringActive:1;    //3  //Initialize to "Not active" after each charge session. //Isolation monitoring should be started with CableCheck. Refer to IEC 61851-23
    uint8_t     triggerReNegotiation:1;         //4  //Initialize to "None" after each charge session. //FAULT_EVSECANInit
    uint8_t     Reserved1:1;                    //5
    uint8_t     triggerStateF:1;                //6  //0 (Disabled), 1(Enabled) / Only available during "Idle Wait" and "Idle" states.
    uint8_t     Reserved2:1;                    //7
};

typedef union
{
    uint8_t                              all;
    struct      __Processing_trgger      bit;
}_Processing_trgger;

struct  __LimitAchieved
{
    uint8_t     evseCurrentLimitAchieved:1;     //0     //Initialize to FALSE after each charge session. //FAULT_EVSECANInit
    uint8_t     evseVoltageLimitAchieved:1;     //1     //Initialize to FALSE after each charge session. //FAULT_EVSECANInit
    uint8_t     evsePowerLimitAchieved:1;       //2     //Initialize to FALSE after each charge session. //FAULT_EVSECANInit
    uint8_t     Reserved:5;                     //34567
};

typedef union
{
    uint8_t                       all;
    struct      __LimitAchieved   bit;
}_LimitAchieved;

typedef struct
{   
    /*Note
    //23
    //The SECC will proceed to emergency stop sequence if the EV doesn't respond to "EVSE initiated normal stop" or renegotiation within the defined timeout. Added to the default margin of 5 sec.
    //unsignedShort 0~65535 s // e.g. Set value as "10" -> 5 + "10" = 15s
  
    //23
    //The SECC will proceed to emergency stop sequence if the EV doesn't respond to "EVSE initiated normal stop" or renegotiation within the defined timeout. Default value is 60  seconds.
    //unsignedShort 0~65535 s // e.g. Set value as "0" --> 60s, Set value as "10" --> 10s
    */
  
    uint8_t                     evseHeartbeat_0;        //binary //EVSE heartbeat. Cycles through 0 to 255. Fail to update it for 500ms will result in Heartbeat warning and fault. Unable to charge. //FAULT_HeartbeatError
    _chargingControl            chargingControl_1;        
    uint16_t                    notificationMaxDelay_23;        
    _evseIsolationStatus        evseIsolationStatus_4;    
    uint8_t                     Reserved_5;             
    _Processing_trgger          Processing_trgger_6;      
    _LimitAchieved              LimitAchieved_7;          
}_E_02;

//EVSE Charge Parameters 1
typedef struct
{
    uint16_t            evseMaximumCurrentLimit_01;        //short 0~4000  0.1 A //Maximum current the EVSE can deliver
    uint16_t            evseMaximumPowerLimit_23;          //short 0~20000 10  W //Maximum power the EVSE can deliver //Although this value is optional in the DIN standard, it MUST be sent to the SECC.
    uint16_t            evseMaximumVoltageLimit_45;        //short 0~10000 0.1 V //Maximum voltage the EVSE can deliver
    uint16_t            evsePeakCurrentRipple_67;          //short 0~4000  0.1 A //peak-to-peak magnitude of the current ripple of the EVSE
}_E_03;

//EVSE Charge Parameters 2
typedef struct
{
    uint16_t            evseMinimumCurrentLimit_01;        //short 0~4000  0.1 A //Minimum current the EVSE can deliver with the expected accuracy
    uint16_t            evseMinimumVoltageLimit_23;        //short 0~10000 0.1 V //Minimum voltage the EVSE can deliver with the expected accuracy
    uint16_t            evseCurrentRegulationTolerance_45; //short 0~4000  0.1 A //Absolute magnitude of the regulation tolerance of the EVSE //If 0, this field is ignored
    uint16_t            evseEnergyToBeDelivered_67;        //short 0~20000 10 Wh //Amount of energy to be delivered by the EVSE               //If 0, this field is ignored
//    uint32_t            CNT;
}_E_04;

//EVSE Present Voltage/Current
typedef struct
{
    uint16_t            evsePresentVoltage;     //short 0~10000 0.1 V //Output voltage of the EVSE
    uint16_t            evsePresentCurrent;     //short 0~4000  0.1 A //Output current of the EVSE
    uint16_t            Reserved_45;            
    uint16_t            Reserved_67;            
}_E_05;

//EVSE DIN ID 1
typedef struct
{
    //EVSE ID 1~8 must conform to DIN SPEC 91286.
    //hexBinary //FAULT_EVSECANInit
//    ECharger_Data       eCharger_Data_0;
    uint32_t            evseDinId1_High;
    uint32_t            evseDinId1_Low;
//    uint32_t            evseDinId;
//    uint32_t            CNT;
}_E_06;

//EVSE DIN ID 2
typedef struct
{
    //EVSE ID 9~16
    //hexBinary //FAULT_EVSECANInit
//   ECharger_Data       eCharger_Data_0;
    uint32_t            evseDinId2_High;
    uint32_t            evseDinId2_Low;
//    uint32_t            CNT;
}_E_07;

//EVSE DIN ID 3
typedef struct
{
    //EVSE ID 17~24
    //hexBinary //FAULT_EVSECANInit
//    ECharger_Data       eCharger_Data_0;
    uint32_t            evseDinId3_High;
    uint32_t            evseDinId3_Low;
//    uint32_t            CNT;
}_E_08;

//EVSE DIN ID 4
typedef struct
{
    //EVSE ID 25~32
    //hexBinary //FAULT_EVSECANInit
//    ECharger_Data       eCharger_Data_0;
    uint32_t            evseDinId4_High;
    uint32_t            evseDinId4_Low;
//    uint32_t            CNT;
}_E_09;

//EVSE ISO ID 1
typedef struct
{
    //EVSE ID 1~8 must conform to IETF RFC 5234 and ISO 15118-2 Annex H.
    //char //FAULT_EVSECANInit
//    ECharger_Data       eCharger_Data_0;
    uint32_t            evseIsoId1_High;
    uint32_t            evseIsoId1_Low;
//    uint32_t            CNT;
}_E_0A;

//EVSE ISO ID 2
typedef struct
{
    //EVSE ID 9~16 
    //char //FAULT_EVSECANInit
//    ECharger_Data       eCharger_Data_0;
    uint32_t            evseIsoId2_High;
    uint32_t            evseIsoId2_Low;
//    uint32_t            CNT;
}_E_0B;

//EVSE ISO ID 3
typedef struct
{
    //EVSE ID 17~24 
    //char //FAULT_EVSECANInit
//    ECharger_Data       eCharger_Data_0;
    uint32_t            evseIsoId3_High;
    uint32_t            evseIsoId3_Low;
//    uint32_t            CNT;
}_E_0C;

//EVSE ISO ID 4
typedef struct
{
    //EVSE ID 25~32 
    //char //FAULT_EVSECANInit
//    ECharger_Data       eCharger_Data_0;
    uint32_t            evseIsoId4_High;
    uint32_t            evseIsoId4_Low;
//    uint32_t            CNT;
}_E_0D;

//EVSE ISO ID 5
typedef struct
{
    //EVSE ID 33~37 
    //char //FAULT_EVSECANInit
//    ECharger_Data       eCharger_Data_0;
    uint32_t            evseIsoId5_High;
    uint32_t            evseIsoId5_Low;
//    uint32_t            CNT;
}_E_0E;

//EVSE Timestamp
typedef struct
{
    //Unix Time Stamp format
    //Time set available only during seccStatusCode = "Idle Wait"
//    ECharger_Data       eCharger_Data_0;
    uint32_t            evseTimestamp_High;
    uint32_t            evseTimestamp_Low;
//    uint32_t            CNT;
}_E_0F;

//EVSE SECC Reboot
typedef struct
{
    //Kernel reboot, hard reset.
    //Only abailable during "Idle Wait"
    //and "Idle" states.
//    ECharger_Data       eCharger_Data_0;
    uint8_t             R_0;    //char //0x52
    uint8_t             E_1;    //char //0x45
    uint8_t             B_2;    //char //0x42
    uint8_t             O_3;    //char //0x4F
    uint8_t             O_4;    //char //0x4F
    uint8_t             T_5;    //char //0x54
    uint16_t            Reserved_67;
//    uint32_t            CNT;
}_E_FF;

typedef struct
{
    CanTxMsg            Message;
//    ECharger_Data       eCharger_Data;
}STxCan2;

typedef struct
{
    _E_01       EVSE_Configurations;
    _E_02       EVSE_Status;
    _E_03       EVSE_Charge_Parameters1;
    _E_04       EVSE_Charge_Parameters2;
    _E_05       EVSE_Present_Voltage_Current;
    _E_06       EVSE_DIN_ID1;
    _E_07       EVSE_DIN_ID2;
    _E_08       EVSE_DIN_ID3;
    _E_09       EVSE_DIN_ID4;
    _E_0A       EVSE_ISO_ID1;
    _E_0B       EVSE_ISO_ID2;
    _E_0C       EVSE_ISO_ID3;
    _E_0D       EVSE_ISO_ID4;
    _E_0E       EVSE_ISO_ID5;
    //_E_0F       E_0F;
    //_E_FF       E_FF;
//    STxCan2     sTx_Can[17];
}EVSE_MSG;



#ifdef _PLC3_0_C


EVSE_MSG  EVSE_Msg[3]; 
SECC_MSG  SECC_Msg[3];

void Variable_Initialize_1CH(void),Variable_Initialize_2CH(void);
void PLC_3_0_Msg_Send_1CH(void), PLC_3_0_Msg_Send_2CH(void);
void SECC_Msg_Receive_1CH(void);
// SECC_Msg_Receive_2CH(void);

_seccStatusCode global_variable_check_seccStatusCode;
_seccStatusCode     seccStatusCode_old;
_evErrorCode        evErrorCode_old;
_seccErrorCode      seccErrorCode_old; 
uint8_t cpOscillator_old;
uint8_t SOC__old;

uint8_t EVSE_SECC_cnt_1CH_reboot = 0,EVSE_SECC_cnt_2CH_reboot = 0;
uint8_t EVSE_SECC_cnt_1CH_50ms_1 = 0, EVSE_SECC_cnt_2CH_50ms_1=0;
uint8_t EVSE_SECC_cnt_1CH_50ms_2 = 0, EVSE_SECC_cnt_2CH_50ms_2=0;
uint8_t EVSE_SECC_cnt_1CH_200ms_1 = 0,EVSE_SECC_cnt_2CH_200ms_1 = 0;
uint8_t EVSE_SECC_cnt_1CH_500ms_1 = 0,EVSE_SECC_cnt_2CH_500ms_1 = 0;
uint8_t EVSE_SECC_cnt_1CH_500ms_2 = 0,EVSE_SECC_cnt_2CH_500ms_2 = 0;
uint8_t EVSE_SECC_cnt_1CH_500ms_3 = 0,EVSE_SECC_cnt_2CH_500ms_3 = 0;
uint16_t EVSE_SECC_Period_1CH = 0, EVSE_SECC_Period_2CH = 0;
uint8_t PLC_Msg_toggle = 0;

#else
 

extern EVSE_MSG  EVSE_Msg[3]; 
extern SECC_MSG  SECC_Msg[3];

extern void Variable_Initialize_1CH(void),Variable_Initialize_2CH(void);
extern void PLC_3_0_Msg_Send_1CH(void), PLC_3_0_Msg_Send_2CH(void);
extern void SECC_Msg_Receive_1CH(void);
//extern void SECC_Msg_Receive_2CH(void);
extern uint8_t EVSE_SECC_cnt_1CH_reboot, EVSE_SECC_cnt_2CH_reboot;

extern _seccStatusCode global_variable_check_seccStatusCode;
extern _seccStatusCode     seccStatusCode_old;
extern _evErrorCode        evErrorCode_old;
extern _seccErrorCode      seccErrorCode_old; 
extern uint8_t cpOscillator_old;
extern uint8_t SOC__old;
extern uint8_t PLC_Msg_toggle;

#endif

