
#define __HMI_MODBUS_H




typedef enum
{
  combo_type = 3,
  load_test = 5
}_charging_type;


/*200번지 Write*/
struct  UI_STATUS_BITS {                        // bit  description //usRegHoldingBuf_2[0]
  uint16_t      Standby                :1;     // 0 기본 상태(초기화면 0: 그 외 1)
  uint16_t      Ready                  :1;     // 1 충전기에서 Ready 받은 후 = 1
  uint16_t      Run                    :1;     // 2 충전중
  uint16_t      Start                  :1;     // 3 충전시작(1인 경우 충전 시작 동작) 
  uint16_t      Stop                   :1;     // 4 충전종료(1인 경우 충전 종료 동작)
  uint16_t      TestFlag               :1;     // 5 EMI, EMC, 생산 테스트, 검수시 등등 사용
  uint16_t      Door_Open              :1;     // 6 도어열림(1인 경우 Cable Type에 따른 커넥터함 도어 오픈함)
  uint16_t      Fault                  :1;     // 7 UI에서 문제 발생시 = 1
  uint16_t      Reserved8              :1;     // 8
  uint16_t      Reserved_Slow          :1;     // 9 완속 빌트인 용
  uint16_t      Load_test              :1;     // 10 부하 테스트모드 실행시 1
  uint16_t      ResetReq               :1;     // 11 리셋요청(1인 경우 충전기 제어보드 Reset)
  uint16_t      Canopy_Light           :1;     // 12
  uint16_t      Charging_reserved      :1;     // 13 순차충전 일 경우 예약여부 체크
  uint16_t      Fan_contl              :1;     // 14 Device 1, 2번중 하나라도 충전중이면 무조건 1로
  uint16_t      FW_Update_Flag         :1;     // 15 F/W 원격 업데이트 flag
};                                       

typedef union UI_STATUS_REG
{
  uint16_t                        all;
  struct      UI_STATUS_BITS      bit;
}_UI_STATUS;

typedef enum  
{                    // bit  description //usRegHoldingBuf_2[1]
  Reserved1_0    = 0,     // 0~15 //AC3 : 1, CHAdeMO : 2, Combo : 3, CHA_Test : 4, Com_Test : 5 , 완속 B타입 : 6, 완속 C타입 : 7 충전 타입은 Reset X 
  AC3            = 1,
  CHAdeMO        = 2,
  COMBO          = 3,   
  CHA_Test       = 4,
  COMBO_Test     = 5,     
  Slow_B         = 6,
  Slow_C         = 7,     
  Reserved1_7    = 8 
}_CABLE_TYPE; //201번지

struct  _CARPAYMENT_BITS {              // bit  description //usRegHoldingBuf_2[4]
  uint16_t      Reserved0_11    :12;    // 0~11 
  uint16_t      Cert_Completed  :1;     // 12 완료:1, 기본:0
  uint16_t      Reserved13      :1;     // 13 
  uint16_t      MAC_CERT_PASS   :1;     // 14 성공:1(충전시작), 대기:0
  uint16_t      MAC_CERT_FAIL   :1;     // 15 실패:1(충전종료), 대기:0
};                                       

typedef union _CARPAYMENT_REG
{
  uint16_t                        all;
  struct      _CARPAYMENT_BITS      bit;
}_CARPAYMENT; //204번지

typedef struct
{
  _UI_STATUS            UI_Status;                      // 200 
  _CABLE_TYPE           Cable_Type;                     // 201 
  uint16_t              Power_Control_Value;            // 202  //왼쪽 채널 단독 100kW : 1, 듀얼 채널 충전 50kW: 2, 오른쪽 채널 단독 100kW : 3, DEFAULT : 4  (넷 중에 하나만 선택할 수 있도록 구성)
  uint16_t              run_count;                      // 203 
  _CARPAYMENT           CarPayment;                     // 204 
  uint16_t              Test_Mode_Vout_Ref_high;        // 205
  uint16_t              Test_Mode_Vout_Ref_low;         // 206 
  uint16_t              Test_Mode_Iout_Ref_high;        // 207 
  uint16_t              Test_Mode_Iout_Ref_low;         // 208 
  uint16_t              Set_Power;                      // 209
  uint16_t              Set_Voltage;                    // 210
  uint16_t              Set_Module_Num;                 // 211
  uint16_t              Reserved212;                    // 212
  uint16_t              Reserved213;                    // 213
  uint16_t              Reserved214;                    // 214
  uint16_t              Reserved215;                    // 215
  uint16_t              Reserved_216;                   // 216
  uint16_t              Reserved217;                    // 217
  uint16_t              Reserved218;                    // 218
  uint16_t              Reserved219;                    // 219
  uint16_t              Reserved220;                    // 220
  uint16_t              Reserved221;                    // 221
  uint16_t              Reserved222;                    // 222
  uint16_t              DC_Output_Voltage_high;         // 223
  uint16_t              DC_Output_Voltage_low;          // 224
  uint16_t              DC_Output_Current_high;         // 225
  uint16_t              DC_Output_Current_low;          // 226
  uint16_t              DC_PowerMeter_high;             // 227
  uint16_t              DC_PowerMeter_low;              // 228
  uint16_t              Carpay_EVSE_ID1;                // 229  //start 0xAB + 충전소 ID
  uint16_t              Carpay_EVSE_ID2;                // 230  //충전소 ID + finish 0xCD
  
}__HMI_Holding_Memory;
// 프로그램 코드상 42 Byte지만 CPU가 자료형을 저장할때 효율을 위해 자료형 크기의 배수에 해당하는 주소를 위치시키기 때문에 빈공간이 생긴다.



/*400번지 Read*/
struct  CHARGER_STATUS_BITS {            // bit  description //usRegInputBuf_2[0]
  uint16_t      Controlboard_Ready              :1;   // 0 기제어보드 준비 완료(1: 준비완료)
  uint16_t      Charger_Ready                   :1;   // 1 충전 가능 상태(1:충전 가능 상태)
  uint16_t      Plug_Check                      :1;   // 2 커넥터 연결 상태(1:연결)
  uint16_t      Door_Check                      :1;   // 3 커넥터 함 상태(1: 닫힘, 0:열림)
  uint16_t      Charging_Run                    :1;   // 4 충전중 상태(1: 충전시퀀스 시작)
  uint16_t      Charging_Finish                 :1;   // 5 충전 완료 상태(1: 충전완료)
  uint16_t      Charger_Fault                   :1;   // 6 충전기 Fault(1:발생, 0:해제)
  uint16_t      mac_address_recevied_complete   :1;   // 7 Reset 상태(1:Reset요청 받음)
  uint16_t      Reserved08                      :1;   // 8 Reserved
  uint16_t      Connector_Lock_Chademo          :1;   // 9 커넥터락 상태(1:잠금, 0:풀림) //차데모용
  uint16_t      Reserved10_11                   :2;   // 10~11 Reserved
  uint16_t      Connector_Lock_COMBO            :1;   // 12 커플러 거치상태, 거치시:1
  uint16_t      Reserved13_14                   :2;   // 13~14 Reserved
  uint16_t      Stand_Lock                      :1;   // 15 외함 센서
};                                    

typedef union
{                                     
  uint16_t                             all;
  struct      CHARGER_STATUS_BITS      bit;
}_CHARGER_STATUS; //400번지

struct  FAULT_LIST_BITS_1 {                          // bit  description //usRegInputBuf_2[23]
  uint16_t      Emergency_switch_on         :1;   // 0 비상정지 버튼 눌림
  uint16_t      MC1_Error                   :1;   // 1 MC1 접점에러 or 융착
  uint16_t      MC2_Error                   :1;   // 2 MC2 접점에러 or 융착
  uint16_t      MC3_Error                   :1;   // 3 MC3 접점에러 or 융착
  uint16_t      RY1_Error                   :1;   // 4 Relay1 접점에러 or 융착
  uint16_t      RY2_Error                   :1;   // 5 Relay2 접점에러 or 융착
  uint16_t      RY3_Error                   :1;   // 6 Relay3 접점에러 or 융착
  uint16_t      RY4_Error                   :1;   // 7 Relay4 접점에러 or 융착
  uint16_t      Fuse_Open                   :1;   // 8 Fuse P OPEN
  uint16_t      Input_OverVoltage           :1;   // 9 입력 과전압
  uint16_t      Inside_OverTemp             :1;   // 10 내부 과온도 (급속:모듈 온도 체크)
  uint16_t      Module_Communication        :1;   // 11  컨트롤보드1 - 컨트롤보드2 통신오류
  uint16_t      con2con_Error               :1;   // 12
  uint16_t      Reserved2313                :1;   // 13
  uint16_t      Reserved2314                :1;   // 14
  uint16_t      Input_UnderVoltage          :1;   // 15  입력 저전압
};

typedef union
{
  uint16_t                             all;
  struct      FAULT_LIST_BITS_1      bit;
}_FAULT_LIST_1; //423번지

struct  FAULT_LIST_BITS_2{
  uint16_t      spd_error                       :1;
  uint16_t      motor_driver_error              :1;
  uint16_t      coupler_unmounted_status        :1;
  uint16_t      chiller_level_error             :1;
  uint16_t      chiller_level_alarm             :1;
  uint16_t      chiller_error                   :1;
  uint16_t      chiller_feed_overtemp           :1;
  uint16_t      chiller_return_p_overtemp       :1;
  uint16_t      chiller_return_m_overtemp       :1;
  uint16_t      cable_bus_p_overtemp            :1;
  uint16_t      cable_bus_m_overtemp            :1;
  uint16_t      cable_inlet_p_overtemp          :1;
  uint16_t      cabel_inlet_m_overtemp          :1;
  uint16_t      fire_detection                  :1;
  uint16_t      tilt_issue                      :1;
  uint16_t      flooding_detection              :1;
};

typedef union{
  uint16_t                              all;
  struct  FAULT_LIST_BITS_2             bit;
}_FAULT_LIST_2; //424번지

struct  CHARGER_Fault1_BITS {                         // bit  description //usRegInputBuf_2[26]
  uint16_t      Coupler_Lock_fault             :1;   // 0  (콤보차량에러)충전 커플러 잠김상태 불량
  uint16_t      Charging_Current_Imbalance     :1;   // 1 (콤보차량에러)충전전류 편차 오류
  uint16_t      Charging_Voltage_Imbalance     :1;   // 2 (콤보차량에러)충전전압 범위/편차 오류
  uint16_t      Charging_System_Incompatibility:1;   // 3 (콤보차량에러)충전시스템 호환성 오류
  uint16_t      EV_Batt_Temp                   :1;   // 4 (콤보차량에러)차량 배터리 온도문제 발생
  uint16_t      EV_Gear_Error                  :1;   // 5 (콤보차량에러)차량 기어상태 확인
  uint16_t      BMS_Error                      :1;   // 6 (콤보차량에러)차량상태/BMS 오류
  uint16_t      Reserved26_07                  :1;   // 7 
  uint16_t      EV_Current_Leakage             :1;   // 8 (콤보에러)출력 누설전류 발생(EV를 점검하세요)
  uint16_t      EV2Con_Communication_Error     :1;   // 9 (콤보에러)차량 - 컨트롤보드 통신 오류
  uint16_t      Coupler_Temp_fault             :1;   // 10  (콤보에러)커넥터 과온도 오류
  uint16_t      Reserved26_11                  :1;   // 11 (콤보에러)커넥터 잠김 오류
  uint16_t      Over_Voltage                   :1;   // 12 (콤보에러)과전압
  uint16_t      Over_Current                   :1;   // 13 (콤보에러)과전류
  uint16_t      PLC_Modem_comm_Error           :1;   // 14 (콤보에러)PLC 모뎀 상태 에러 - 재부팅 or 교체
  uint16_t      EVSE_Current_Leakage           :1;   // 15 (콤보에러)출력 누설전류 발생(충전기를 점검하세요)
};

typedef union CHARGER_Fault1_STATUS_REG
{
  uint16_t                             all;
  struct      CHARGER_Fault1_BITS      bit;
}_CHARGER_Fault1_STATUS; //426번지


struct  CHARGER_Fault2_BITS {                         // bit  description //usRegInputBuf_2[27]
  uint16_t      Reserved_PLC0                   :1;   // 0 
  uint16_t      PLC_Sequence_Error              :1;   // 1 
  uint16_t      PLC_ServiceID_Invalid           :1;   // 2 
  uint16_t      PLC_UnknownSession              :1;   // 3 
  uint16_t      PLC_ServiceSelectionInvalid     :1;   // 4 
  uint16_t      PLC_PaymentSelectionInvalid     :1;   // 5 
  uint16_t      Reserved_PLC6_11                :6;   // 6~11
  uint16_t      PLC_WrongChargeParameter        :1;   // 12
  uint16_t      Reserved_PLC13                  :1;   // 13 Reserved
  uint16_t      PLC_TariffSelectionInvalid      :1;   // 14 
  uint16_t      PLC_ChargingProfileInvalid      :1;   // 15 기타오류 1:발생, 0: 복구
};                                                   

typedef union CHARGER_Fault2_STATUS_REG
{
  uint16_t                             all;        
  struct      CHARGER_Fault2_BITS      bit;
}_CHARGER_Fault2_STATUS; //427번지

struct  CHARGER_Fault3_BITS {                                // bit  description //usRegInputBuf_2[28]
  uint16_t      Reserved_PLC28_01                      :1;  // 0 
  uint16_t      PLC_NoChargeServiceSelected            :1;  // 1 
  uint16_t      PLC_WrongEnergyTransferMode            :1;  // 2 
  uint16_t      Reserved_PLC3_5                        :3;  // 3~5
  uint16_t      PLC_TIMEOUT_CommunicationSetup         :1;  // 6
  uint16_t      PLC_TIMEOUT_Sequence                   :1;  // 7
  uint16_t      PLC_TIMEOUT_NotificationMaxDelay       :1;  // 8
  uint16_t      PLC_WrongCPLevel                       :1;  // 9 
  uint16_t      Reserved_PLC28_10                      :1;  // 10 
  uint16_t      PLC_HLCError                           :1;  // 11 
  uint16_t      PLC_HeartbeatError                     :1;  // 12 
  uint16_t      PLC_EVSECANInit                        :1;  // 13 
  uint16_t      FAILED_NoNegotiation                   :1;  // 14 
  uint16_t      TIMEOUT_WeldingDetection               :1;  // 15기타오류 1:발생, 0: 복구
};

typedef union CHARGER_Fault3_STATUS_REG
{
  uint16_t                             all;
  struct      CHARGER_Fault3_BITS      bit;
}_CHARGER_Fault3_STATUS; //428번지

struct  SEQUENCE_ERROR1_STATUS_BITS {                 // bit  description //usRegInputBuf_2[29]
  uint16_t      PLC_Idle_Error                 :1;   // 0   
  uint16_t      Slac_Req_Error                 :1;   // 1 
  uint16_t      PLC_CP_Error                   :1;   // 2 
  uint16_t      Protocol_Req_Error             :1;   // 3 
  uint16_t      ChargingType_Req_Error         :1;   // 4 
  uint16_t      SessionSetup_Req_Error         :1;   // 5 
  uint16_t      ServiceDiscovery_Req_Error     :1;   // 6 
  uint16_t      Payment_Req_Error              :1;   // 7 
  uint16_t      HLC_Req_Error                  :1;   // 8 
  uint16_t      Discovery_Info_Error           :1;   // 9 
  uint16_t      Discovery_Req_Error            :1;   // 10 
  uint16_t      CableCheck_Req_Error           :1;   // 11 
  uint16_t      CableCheck_CP_Error            :1;   // 12 
  uint16_t      CableCheck1_Error              :1;   // 13 
  uint16_t      CableCheck2_Error              :1;   // 14 
  uint16_t      PreCharge_Req_Error            :1;   // 15 
};

typedef union SEQUENCE_ERROR1_STATUS_REG
{
  uint16_t                             all;
  struct      SEQUENCE_ERROR1_STATUS_BITS      bit;
}_SEQUENCE_ERROR1_STATUS; //429번지

struct  SEQUENCE_ERROR2_STATUS_BITS {           // bit  description //usRegInputBuf_2[30] 
  uint16_t      PreCharge_Error        :1;     // 0   
  uint16_t      FAULT_HPGPLinkDown     :1;     // 1 
  uint16_t      CarPayment_Fail        :1;     // 2 
  uint16_t      Reserved3_15           :12;    // 3 ~15 
};

typedef union SEQUENCE_ERROR2_STATUS_REG
{
  uint16_t                             all;
  struct      SEQUENCE_ERROR2_STATUS_BITS      bit;
}_SEQUENCE_ERROR2_STATUS; //430번지

struct  MODULE_WARNING_BITS {            // bit  description //usRegInputBuf_2[30]
  uint16_t      Module01_warning :1;    // 0   
  uint16_t      Module02_warning :1;    // 1 
  uint16_t      Module03_warning :1;    // 2 
  uint16_t      Module04_warning :1;    // 3 
  uint16_t      Module05_warning :1;    // 4 
  uint16_t      Module06_warning :1;    // 5 
  uint16_t      Module07_warning :1;    // 6 
  uint16_t      Module08_warning :1;    // 7 
  uint16_t      Module09_warning :1;    // 8 
  uint16_t      Module10_warning :1;    // 9 
  uint16_t      Module11_warning :1;    // 10 
  uint16_t      Module12_warning :1;    // 11 
  uint16_t      Module13_warning :1;    // 12 
  uint16_t      Module14_warning :1;    // 13 
  uint16_t      Module15_warning :1;    // 14 
  uint16_t      Module16_warning :1;    // 15
};

typedef union MODULE_WARNING_REG
{
  uint16_t                             all;
  struct      MODULE_WARNING_BITS      bit;
}_MODULE_WARNING; //434번지

typedef struct
{
  _CHARGER_STATUS                   Charger_Status;             // 400    
  uint16_t                          Run_Count;                  // 401    
  uint16_t                          Stop_Code;                  // 402     
  uint16_t                          CP_Voltage;                 // 403     
  uint16_t                          Fw_Version;                 // 404  //십진수 111 -> 1.1.1로 표기
  uint16_t                          BulkTime_SOC;               // 405  //remainingTimeToBulkSOC 
  uint16_t                          EV_SOC;                     // 406
  uint16_t                          Reserved407;                // 407
  uint16_t                          out_voltage;                // 408  //출력 전압
  uint16_t                          out_current;                // 409  //출력 전류
  uint8_t                           SECC_Status_Code;           // 410H   
  uint8_t                           SECC_Error_Code;            // 410L   
  uint16_t                          Reserved_411;               // 411   
  uint16_t                          Reserved_412;               // 412
  uint16_t                          DC_PowerMeter_high;         // 413 //출력 DC_PowerMeter_high
  uint16_t                          DC_PowerMeter_low;          // 414 //출력 DC_PowerMeter_low
  uint16_t                          AC_PowerMeter_high;         // 415 //입력 A4C_PowerMeter_high
  uint16_t                          AC_PowerMeter_low;          // 416 //입력 AC_PowerMeter_low
  int16_t                           Coupler_TempP;              // 417  //커플러온도P('C)
  int16_t                           Coupler_TempN;              // 418  //커플러온도N('C)
  uint16_t                          Target_Voltage;             // 419  //타겟전압
  uint16_t                          Demand_Current;             // 420  //요청전류
  uint16_t                          Module_Temp;                // 421  //파워모듈 온도('C)
  uint8_t                           EV_Error_Code;              // 422H
  uint8_t                           EV_Atten_Value;             // 422L 
  _FAULT_LIST_1                     Fault_List_1;               // 423  //EVSE 폴트 리스트1 (기본 폴트)
  _FAULT_LIST_2                     Fault_List_2;               // 424  //EVSE 폴트 리스트2 (대부분 수랭식 관련)
  uint16_t                          Reserved_425;               // 425 
  _CHARGER_Fault1_STATUS            Fault1;                     // 426  //EV+EVSE 폴트 리스트
  _CHARGER_Fault2_STATUS            Fault2;                     // 427  //SECC 폴트 리스트1
  _CHARGER_Fault3_STATUS            Fault3;                     // 428  //SECC 폴트 리스트2
  _SEQUENCE_ERROR1_STATUS           ERROR1;                     // 429  //충전 시퀀스 에러1
  _SEQUENCE_ERROR2_STATUS           ERROR2;                     // 430  //충전 시퀀스 에러2
  uint16_t                          Reserved_CHAdeMO1;          // 431  //CHAdeMO 차량 에러1
  uint16_t                          Reserved_CHAdeMO2;          // 432  //CHAdeMO 차량 에러2
  uint16_t                          Reserved_CHAdeMO3;          // 433  //CHAdeMO 차량 에러3
  _MODULE_WARNING                   Module_Warning;             // 434  //모듈 오류 경고
  uint16_t                          evccMAC_1;                  // 435 //EVCCID(현재는 0~5까지만 사용)
  uint16_t                          evccMAC_2;                  // 436 //EVCCID(현재는 0~5까지만 사용)
  uint16_t                          evccMAC_3;                  // 437 //EVCCID(현재는 0~5까지만 사용)
  uint16_t                          evccMAC_4;                  // 438 //EVCCID(현재는 0~5까지만 사용)
  uint16_t                          FullTime_SOC;               // 439 
  
}__HMI_Input_Memory;

#ifdef __HMI_MODBUS_C



void HMI_tx_data_make(void);
void HMI_rx_data_parse(void);
void HMI_memory_reset(void);

uint16_t CRC16 (const uint8_t *nData, uint8_t wLength); // modbus
u8 make8(u16 var, u8 offset);
uint8_t DM_CS_u8(uint8_t *nData, int n);

void Rxd_SCI_2(void);
void Txd_SCI_2(void);


uint8_t         txnew_Test = 0;

uint8_t         bRxSio_1[29], bTxSio_1[39];
uint8_t         cRxd_1 = 0, cTxd_1 = 0, rxok_1 = 0, txnew_1 = 0, cRxdMax_1 = 100, cTxdMax_1 = 0;
uint8_t         uart1_cnt = 0, uart1_cnt_chk = 0;
uint8_t         bRxSio_2[80], bTxSio_2[100];
uint8_t         cRxd_2 = 0, cTxd_2 = 0, rxok_2 = 0, txnew_2 = 0, cRxdMax_2 = 100, cTxdMax_2 = 0;
uint8_t         usart2_tx_ok = 0, counter_ustart2_tx;
uint8_t         ReadCnt=0;


_charging_type charging_type_select_1CH;


volatile __HMI_Holding_Memory Holding_Memory[3];
volatile __HMI_Input_Memory Input_Memory[3];

uint16_t user_set_charger_max_current, evse_max_set_current;
uint8_t BOARD_ID_RX = 0, BOARD_ID_TX = 0;

uint8_t carpayment_authentication_complated = 0;
uint8_t mac_address_authentication_complated = 0;
uint8_t mac_address_authentication_failed = 0;
uint8_t mac_address_recevied_complete = 0;

uint8_t number_of_module;
uint16_t set_power;
uint8_t EVSE_ID_buf[9];

uint8_t memset_flag_1CH = 0;
uint16_t load_test_display_value_419;
uint16_t load_test_display_value_420;
uint16_t load_test_display_value_421;
uint16_t load_test_display_value_408;
uint16_t load_test_display_value_409;
uint16_t load_test_display_value_410;
uint16_t load_test_display_value_415;
uint16_t load_test_display_value_416;
uint16_t load_test_display_value_401;
uint16_t load_test_display_value_402;
uint16_t load_test_display_value_411;
uint16_t load_test_display_value_412;
uint16_t load_test_display_value_417;
uint16_t load_test_display_value_403;
uint16_t load_test_display_value_404;
uint16_t load_test_display_value_405;
uint16_t load_test_display_value_406;
uint16_t load_test_display_value_413;
uint16_t load_test_display_value_414;
uint16_t load_test_display_value_418;


#else 

extern _charging_type charging_type_select_1CH;


extern void HMI_tx_data_make(void);
extern void HMI_rx_data_parse(void);
extern void HMI_memory_reset(void);

extern void Rxd_SCI_2(void);
extern void Txd_SCI_2(void);

extern u8 make8(u16 var, u8 offset);
extern uint16_t CRC16 (const uint8_t *nData, uint8_t wLength);

extern uint8_t         txnew_Test;

extern uint8_t          bRxSio_1[29], bTxSio_1[39];
extern uint8_t          bRxSio_2[80], bTxSio_2[100];
extern uint8_t          bRxSio_3[29], bTxSio_3[39];
extern uint8_t          cRxd_1, cTxd_1, rxok_1, cRxdMax_1, cTxdMax_1;
extern uint8_t          cRxd_2, cTxd_2, rxok_2, cRxdMax_2, cTxdMax_2;
extern uint8_t          cRxd_3, cTxd_3, rxok_3, cRxdMax_3, cTxdMax_3;
extern uint8_t          uart1_cnt;
extern uint8_t          uart1_cnt_chk;
extern  uint8_t         ReadCnt,txnew_2;


extern volatile __HMI_Holding_Memory Holding_Memory[3];
extern volatile __HMI_Input_Memory Input_Memory[3];


extern uint8_t BOARD_ID_RX, BOARD_ID_TX;

extern uint8_t carpayment_authentication_complated;
extern uint8_t mac_address_authentication_complated;
extern uint8_t mac_address_authentication_failed;
extern uint8_t mac_address_recevied_complete;

extern uint8_t number_of_module;
extern uint16_t set_power;
extern uint8_t EVSE_ID_buf[9];

extern uint8_t memset_flag_1CH;

extern uint16_t load_test_display_value_419;
extern uint16_t load_test_display_value_420;
extern uint16_t load_test_display_value_421;
extern uint16_t load_test_display_value_408;
extern uint16_t load_test_display_value_409;
extern uint16_t load_test_display_value_410;
extern uint16_t load_test_display_value_415;
extern uint16_t load_test_display_value_416;
extern uint16_t load_test_display_value_401;
extern uint16_t load_test_display_value_402;
extern uint16_t load_test_display_value_411;
extern uint16_t load_test_display_value_412;
extern uint16_t load_test_display_value_417;
extern uint16_t load_test_display_value_403;
extern uint16_t load_test_display_value_404;
extern uint16_t load_test_display_value_405;
extern uint16_t load_test_display_value_406;
extern uint16_t load_test_display_value_413;
extern uint16_t load_test_display_value_414;
extern uint16_t load_test_display_value_418;

#endif

