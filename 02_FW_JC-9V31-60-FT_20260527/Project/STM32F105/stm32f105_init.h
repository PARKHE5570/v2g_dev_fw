
#define __STM32F105_INIT_H



#ifdef _STM32F105_INIT_C



CanRxMsg        RxMessage2;
CanTxMsg        TxMessage2;


void DMA1_Config();
void WWDG_Configuration();
void IWDG_Configuration();

void Init_RxMes(CanRxMsg *RxMessage, uint8_t CAN_IDE);
void Init_TxMes(CanTxMsg *TxMessage, uint8_t CAN_IDE);
void Init_CAN_buf(uint8_t *CAN_buf);

void UART_Init(u8 uart,u32 baud);

void NVIC_Configuration(void);
void CAN_Config(void);
void GPIO_Config(void);
void TIM1_Configuration(void);
void TIM4_Configuration(void);
void TIM3_Configuration(void);
void System_Init(void);

void ADC1_Configuration(void);

#else

extern CanRxMsg        RxMessage2;
extern CanTxMsg        TxMessage2;


extern void DMA1_Config();
extern void WWDG_Configuration();
extern void IWDG_Configuration();

extern void Init_RxMes(CanRxMsg *RxMessage, uint8_t CAN_IDE);
extern void Init_TxMes(CanTxMsg *TxMessage, uint8_t CAN_IDE);
extern void Init_CAN_buf(uint8_t *CAN_buf);

extern void UART_Init(u8 uart,u32 baud);

extern void NVIC_Configuration(void);
extern void CAN_Config(void);
extern void GPIO_Config(void);
extern void TIM1_Configuration(void);
extern void TIM4_Configuration(void);
extern void TIM3_Configuration(void);
extern void System_Init(void);

extern void ADC1_Configuration(void);

#endif