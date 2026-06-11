
#define __TERMINAL_H

// wchar_t == unsigned short? 리터럴을 2바이트 문자열로 인식하게 해주는 접두어 L이 있다.
// c언어는 기본적으로 1바이트 형식으로 문자열을 인식하고, 리터럴마저도 그런 법칙에서 벗어나진 않기 때문에 이걸 앞에 붙여주지 않으면 문제가 생긴다.
// 로케일이란게 필요하다. 문자열 처리방식을 어떻게 해야 할까를 정하는 것이다.
// C언어의 로케일은 기본적으로 아스키코드만을 사용하기 때문에 이것도 확장을 해줘야 제대로 처리를 한다.
// 유니코드는 국제표준 문자표이고 UTF-8은 인코딩 방식이다. 



/* Defines -------------------------------------------------------------------*/
#define last_update_year  (2024) // 년
#define last_update_month (04) // 월
#define last_update_day   (23)  // 일 , 앞에 08, 09을 입력하면 컴파일 안됨 -> 8진수로 인식

#define fw_version       (108) // 0.0.1 //  1채널 보드용 펌웨어 / 3째자리 정식 펌웨어 / 4째자리 테스트 펌웨어 ex:1061 106에 대한 1번째 테스트 펌웨어
//v108 CN15 imd AD -> I/O, UI 통신부분 수정사항 적용(CRC체크 부분, 30kW UI의 경우 228번지 까지만 보내줌 )

/* 
--------------------------------F/W HISTORY--------------------------------
// Ver1.0.3 - 2024.02.01 누설전류 Fault 기준 값 수정, 결제단말기 ON 시간 딜레이 추가(3초)
// Ver1.0.4 - 
// Ver1.0.5 - 
// Ver1.0.6 - 2024.02.14 누설전류 감지 평균값 + Fault 딜레이(500ms) / 결제단말기 On 시간 딜레이(5초) / KTC 검증용 릴레이 On/off 기능 추가
// Ver1.0.6.1 - 누설전류 감지에 대한 임시 주석처리 20240318

---------------------------------------------------------------------------
*/

//CP에러 또는 malfunction으로 종료 시 충전 재시작, Plug_Check_Disabled 정보를 UI로 부터 수신받아서 초기 CP state 설정

#define UART1           0
#define UART2           1
#define UART3           2
#define RBUF_SIZE       256
#define RBUF_SIZE7      7

/*
Cursor Controls
ESC[H	moves cursor to home position (0, 0)
ESC[{line};{column}H
ESC[{line};{column}f	moves cursor to line #, column #
ESC[#A	moves cursor up # lines
ESC[#B	moves cursor down # lines
ESC[#C	moves cursor right # columns
ESC[#D	moves cursor left # columns
ESC[#E	moves cursor to beginning of next line, # lines down
ESC[#F	moves cursor to beginning of previous line, # lines up
ESC[#G	moves cursor to column #
ESC[6n	request cursor position (reports as ESC[#;#R)
ESC M	moves cursor one line up, scrolling if needed
ESC 7	save cursor position (DEC)
ESC 8	restores the cursor to the last saved position (DEC)
ESC[s	save cursor position (SCO)
ESC[u	restores the cursor to the last saved position (SCO)

//Erase Functions
ESC[J	erase in display (same as ESC[0J)
ESC[0J	erase from cursor until end of screen
ESC[1J	erase from cursor to beginning of screen
ESC[2J	erase entire screen
ESC[3J	erase saved lines
ESC[K	erase in line (same as ESC[0K)
ESC[0K	erase from cursor to end of line
ESC[1K	erase start of line to the cursor
ESC[2K	erase the entire line

//Colors / Graphics Mode
ESC[0m		reset all modes (styles and colors)
ESC[1m	ESC[22m	set bold mode.
ESC[2m	ESC[22m	set dim/faint mode.
ESC[3m	ESC[23m	set italic mode.
ESC[4m	ESC[24m	set underline mode.
ESC[5m	ESC[25m	set blinking mode
ESC[7m	ESC[27m	set inverse/reverse mode
ESC[8m	ESC[28m	set hidden/invisible mode
ESC[9m	ESC[29m	set strikethrough mode.

Reference - https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
*/

#define T_CS1    "\033[1J"  //ESC[1J 은 현재 커서 위치부터 화면을 지운다.
#define T_CS2    "\033[2J"  //ESC[2J 은 전체 화면을 지운다.
#define T_CS3    "\033[3J"  //ESC[3J 은 Scroll-back을 포함한 전체 화면을 지운다.
#define T_CS4    "\033[K"   //ESC[K  은 현재 커서 위치부터 라인을 지운다.
#define T_CS5    "\033[1K"  //ESC[1K 은 라인 처음에서 현재 커서 위치까지 지운다.
#define T_CS6    "\033[2K"  //ESC[2K 은 전체 라인을 지운다.

#define T_RESET        "\033[0m" // All attributes off
#define T_BOLD         "\033[1m" // Bold or increased intensity
#define T_UNDERLINE    "\033[4m" // Underline
#define T_BELL         "\a "

// Foreground color CodeEffect
//Color Name	Foreground Color Code	Background Color Code
//Bright Black	    90	100
//Bright Red	    91	101
//Bright Green	    92	102
//Bright Yellow   	93	103
//Bright Blue	    94	104
//Bright Magenta	95	105
//Bright Cyan	    96	106
//Bright White	    97	107

#define T_BrBLACK   "\033[90m"
#define T_BrRED     "\033[91m"
#define T_BrGREEN   "\033[92m"
#define T_BrYELLOW  "\033[93m"
#define T_BrBLUE    "\033[94m"
#define T_BrMAGENTA "\033[95m"
#define T_BrCYAN    "\033[96m"
#define T_BrWHITE   "\033[97m"

#define T_BG_BrBLACK   "\033[100m"
#define T_BG_BrRED     "\033[101m"
#define T_BG_BrGREEN   "\033[102m"
#define T_BG_BrYELLOW  "\033[103m"
#define T_BG_BrBLUE    "\033[104m"
#define T_BG_BrMAGENTA "\033[105m"
#define T_BG_BrCYAN    "\033[106m"
#define T_BG_BrWHITE   "\033[107m"

#define T_BLACK   "\033[30m"
#define T_RED     "\033[31m"
#define T_GREEN   "\033[32m"
#define T_YELLOW  "\033[33m"
#define T_BLUE    "\033[34m"
#define T_MAGENTA "\033[35m"
#define T_CYAN    "\033[36m"
#define T_WHITE   "\033[37m"




typedef enum
{
  none = 0,
  charging_info_start_cmd, 
  charging_info_stop_cmd, 
  system_reset_cmd, 
  fw_update_cmd,
  version_cmd,
  charging_status,
  manual_mode,
  self_test_start,
  history_cmd,
  wrong_cmd
}_system_command;




#ifdef __TERMINAL_C

uint32_t elapsed_time = 0; // 100ms
float elapsed_time_s = 0;


uint16_t minute;
uint16_t hour;
uint16_t second;


uint32_t g_elapsed_time = 0; // 100ms
float g_elapsed_time_s = 0;

uint16_t g_day;
uint16_t g_minute;
uint16_t g_hour;
uint16_t g_second;

const uint8_t UTF8_Byte_table[0x100] =
{
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x00-0x0F */
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x10-0x1F */
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x20-0x2F */
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x30-0x3F */
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x40-0x4F */
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x50-0x5F */
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x60-0x6F */
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x70-0x7F */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x80-0x8F */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x90-0x9F */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xA0-0xAF */
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xB0-0xBF */
   0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* 0xC0-0xCF */
   2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* 0xD0-0xDF */
   3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, /* 0xE0-0xEF */
   4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0, /* 0xF0-0xFF */
};

uint8_t buff[4] = {0xd7, 0x87};
uint8_t UTF8_Byte_val;

uint32_t UTF8_Type_Check(uint8_t num);

char HexToAscii(u8 hex);
void UART1_PutChar(u8 data);
u8 UART1_GetChar(u8 *pData);
void UART1_PutStr(char *pStr);
void UART1_PutHex(u8 hex);
void UART1_Put2Hex(u16 hex);
void UART1_Put4Hex(u32 hex);
void UART1_PutDec(u8 cnt,u16 dec);
u8 AsciiToHex(char ascii);
void Send_Uart_Terminal(void);
void Send_Uart_Terminal_default(void);



float dc_electric_energy;
uint8_t view_count;



void term_get_cmd_excute(void);
void send_uart_term_system_info(void);
uint8_t send_uart_term_start_flag, send_uart_term_start;


_system_command system_cmd;

char evsis_text1[] ={"\r\n\033[37m  ######## ##     ##  ######  ####  ######  "};
char evsis_text2[] ={"\r\n\033[37m  ##       ##     ## ##    ##  ##  ##    ## "};
char evsis_text3[] ={"\r\n\033[37m  ##       ##     ## ##        ##  ##       "};
char evsis_text4[] ={"\r\n\033[37m  ######   ##     ##  ######   ##   ######  "};
char evsis_text5[] ={"\r\n\033[37m  ##        ##   ##        ##  ##        ## "};
char evsis_text6[] ={"\r\n\033[37m  ##         ## ##   ##    ##  ##  ##    ## "};
char evsis_text7[] ={"\r\n\033[37m  ########    ###     ######  ####  ######   \033[0m\r\n"};

uint8_t finish_newline_flag;
void make_random_number(void);
void total_operating_time(void);
void charging_infomation (void);
void charging_infomation2 (void);
UINT8 history_flag = 0;

uint8_t before_hour, before_min, before_sec;
uint16_t before_day;
void fault_history_function (void);

//함수의 원형
void printMonth(int year, int month);
void printMonthTitle(int year, int month);
void printMonthName(int month);
void printMonthBody(int year, int month);
int getStartDay(int year, int month);
int getTotalNumberOfDays(int year, int month);
int getNumberOfDaysInMonth(int year, int month);
int isLeapYear(int year);

#else 

extern UINT8 history_flag;

extern uint8_t before_hour, before_min, before_sec;
extern uint16_t before_day;

extern char evsis_text1[];
extern char evsis_text2[];
extern char evsis_text3[];
extern char evsis_text4[];
extern char evsis_text5[];
extern char evsis_text6[];
extern char evsis_text7[];

extern uint32_t UTF8_Type_Check(uint8_t num);
extern char HexToAscii(u8 hex);
extern void UART1_PutChar(u8 data);
extern u8 UART1_GetChar(u8 *pData);
extern void UART1_PutStr(char *pStr);
extern void UART1_PutHex(u8 hex);
extern void UART1_Put2Hex(u16 hex);
extern void UART1_Put4Hex(u32 hex);
extern void UART1_PutDec(u8 cnt,u16 dec);
extern u8 AsciiToHex(char ascii);
extern void Send_Uart_Terminal(void);
extern void Send_Uart_Terminal_default(void);



extern float dc_electric_energy;
extern uint8_t view_count;


extern void term_get_cmd_excute(void);
extern void send_uart_term_system_info(void);

extern uint8_t send_uart_term_start_flag, send_uart_term_start;


extern _system_command system_cmd;
extern uint8_t finish_newline_flag;
extern void make_random_number(void);
extern void total_operating_time(void);

extern float g_elapsed_time_s;

extern uint16_t g_day;
extern uint16_t g_minute;
extern uint16_t g_hour;
extern uint16_t g_second;

extern void charging_infomation (void);
extern void charging_infomation2 (void);

extern uint32_t g_elapsed_time;
extern void fault_history_function (void);


extern void printMonth(int year, int month);
extern void printMonthTitle(int year, int month);
extern void printMonthName(int month);
extern void printMonthBody(int year, int month);
extern int getStartDay(int year, int month);
extern int getTotalNumberOfDays(int year, int month);
extern int getNumberOfDaysInMonth(int year, int month);
extern int isLeapYear(int year);

#endif

