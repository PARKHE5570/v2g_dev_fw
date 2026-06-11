
#define __SELF_CHECK_H

#define MODULE_ON_OFF_TIMEOUT_SET               (1000)  // 10 [sec]
#define AFTER_5_SECONDS                         (700)   // 5  [sec]
#define DIGITAL_INPUT_OUTPUT_TIMEOUT_SET        (150)   // 1.5[sec]
#define TEST_VOLTAGE_COMMAND                    (750)   //    [V]
#define FAN_CHECK_COUNT                         (1000)  // 10 [sec]



#ifdef __SELF_CHECK_C

uint16_t self_check_timer = 0;

uint8_t test_step = 0;

uint8_t check_list_ok = 0;


uint8_t Automatic_self_check (void);

UINT32 self_test_result0; // 결과값을 언제까지 가지고 있을 것인지? 언제 클리어 할지? HMI가 클리어 명령을 줄 것인지?
UINT32 self_test_result1;

#else 

extern uint8_t Automatic_self_check (void);

extern UINT32 self_test_result0;
extern UINT32 self_test_result1;

#endif

