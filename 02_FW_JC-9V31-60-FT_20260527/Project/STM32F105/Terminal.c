
#define __TERMINAL_C

#include "stdint.h"
#include "stm32f10x.h"
#include "HMI_modbus.h"
#include "main.h"
#include "stm32f10x_it.h"
#include "FirmwareUpdate.h"
//#include "io.h"
#include "INFY_V2G.h"
#include "PLC_3_0.h"
#include "ctype.h"
#include "pt1000.h"
#include "digital_io.h"
#include "Terminal.h"
#include "fault_check.h"



/* Type declarations ---------------------------------------------------------*/
typedef struct{
	
	u16 RxInCnt,RxOutCnt;
	u16 TxInCnt,TxOutCnt;
	u8 RxBuf[RBUF_SIZE];
	
}StructUart;  

extern StructUart Uart1,Uart2;


/*******************************************************************************
* Function Name : char HexToAscii(u8 hex)
* Description   : 16진수를 아스키코드로 변환
* Parameters    : 1byte
* Return        : 아스키코드
*******************************************************************************/
char HexToAscii(u8 hex)
{	
	return hex<10 ? hex+'0' : hex-10+'A';
}

/*******************************************************************************
* Function Name : void UART1_PutChar(u8 data)
* Description   : UART1 1문자 출력, Transmit one byte
* Parameters    : 아스키코드
* Return        : None
*******************************************************************************/
void UART1_PutChar(u8 data)
{
	USART_SendData(USART1,data);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);  
}

/*******************************************************************************
* Function Name : u8 UART1_GetChar(u8 *pData)
* Description   : UART1로 수신버퍼에서 데이타(1byte) 가져오기
* Parameters    : none
* Return        : 데이타 수신 여부(0: 수신버퍼가 비었음, 1: 수신버퍼에 데이타가 있음)
*******************************************************************************/
u8 UART1_GetChar(u8 *pData)
{       
	if(Uart1.RxInCnt == Uart1.RxOutCnt) return 0;
	else *pData = Uart1.RxBuf[Uart1.RxOutCnt];        
	if(Uart1.RxOutCnt<RBUF_SIZE-1) Uart1.RxOutCnt++;
	else Uart1.RxOutCnt = 0;
	
	return 1;
}

/*******************************************************************************
* Function Name : void UART1_PutStr(char *pStr)
* Description   : UART1 믄자열 출력
* Parameters    : 문자열
* Return        : 1byte
*******************************************************************************/
void UART1_PutStr(char *pStr)
{
	while(*pStr != '\0') UART1_PutChar(*(pStr++));
}

/*******************************************************************************
* Function Name : void UART1_PutHex(u8 hex)
* Description   : UART1 16진수값 1byte출력
* Parameters    : 1byte
* Return        : none
*******************************************************************************/
void UART1_PutHex(u8 hex)
{
	UART1_PutChar(HexToAscii(hex>>4));
	UART1_PutChar(HexToAscii(hex&0x0F));
}

/*******************************************************************************
* Function Name : void UART1_Put2Hex(u16 hex)
* Description   : UART1 16진수값 2byte 출력
* Parameters    : 2byte
* Return        : none
*******************************************************************************/
void UART1_Put2Hex(u16 hex)
{
	UART1_PutHex(hex/0x100);
	UART1_PutHex(hex%0x100);     
}

/*******************************************************************************
* Function Name : void UART1_Put4Hex(u32 hex)
* Description   : UART1 16진수값 4byte 출력
* Parameters    : 4byte
* Return        : none
*******************************************************************************/
void UART1_Put4Hex(u32 hex)
{
	UART1_Put2Hex(hex/0x10000);
	UART1_Put2Hex(hex%0x10000);
}

/*******************************************************************************
* Function Name : UART1_PutDec(u8 cnt,u16 dec)
* Description   : UART1 10진수값 출력(1~5자리)
* Parameters    : 출력 자리수, 2byte
* Return        : none
*******************************************************************************/
void UART1_PutDec(u8 cnt,u16 dec)
{      
	if(cnt == 5) UART1_PutChar(HexToAscii(dec/10000)),cnt--;
	if(cnt == 4) UART1_PutChar(HexToAscii(dec%10000/1000)),cnt--;     
	if(cnt == 3) UART1_PutChar(HexToAscii(dec%1000/100)),cnt--;     
	if(cnt == 2) UART1_PutChar(HexToAscii(dec%100/10)),cnt--;     
	if(cnt == 1) UART1_PutChar(HexToAscii(dec%10));
} 


/*******************************************************************************
* Function Name : u8 AsciiToHex(char ascii)
* Description   : 아스키코드를 16진수로 변환
* Parameters    : 아스키코드
* Return        : 16진수
*******************************************************************************/
u8 AsciiToHex(char ascii)
{	
	ascii = toupper(ascii);
	return ascii>='A' ? ascii-'A'+ 10 : ascii-'0';
}


UINT8 Term_Send_RAMStr( UINT8 * str )
{
	UINT8 count = 0;            // Initialize byte counter
	
	while( *str != 0 )                  // Reached zero byte ?
	{
		UART1_PutChar( *str );
		str++;                          // Increment ptr
		count++;                        // Increment byte counter
	}
	
	return count;                       // Return byte count
}

void Term_Sand_sprintf (float val)
{
	char pBuf[100];
	
	sprintf(pBuf, "value = %f  ", val);
	//    UART1_PutStr(pBuf);
	Term_Send_RAMStr((UINT8*)pBuf);
}


void send_uart_term_system_info(void)
{
	//ANSI Escape Code, Terminal color change, ANSI art
	
	//ESC[ 38;2;?r?;?g?;?b? m Select RGB foreground color
	//ESC[ 48;2;?r?;?g?;?b? m Select RGB background color
	//	UART1_PutStr("\033[38;5;206m TEST \r\n"); // Using these above, you can make pink text like so:
	//	UART1_PutStr("\033[48;5;57m  TEST \r\n"); // And make an early-morning blue background using
	//	UART1_PutStr("\033[38;5;206;48;5;57m TEST \r\n"); // And, of course, you can combine these:
	//	UART1_PutStr("\033[48;5;9m  TEST \r\n"); // And make an early-morning blue background using
	
	//	UART1_PutStr(T_BOLD);
	UART1_PutStr(T_BrRED);     UART1_PutStr("\r\n\033[37m   ####     ####     ####     ####      ######## ##     ##  ######  ####  ######  ");
	UART1_PutStr(T_BrGREEN);   UART1_PutStr("\r\n\033[37m  ##  ##   ##  ##   ##  ##   ##  ##     ##       ##     ## ##    ##  ##  ##    ## ");
	UART1_PutStr(T_BrYELLOW);  UART1_PutStr("\r\n\033[37m      ##   ## ###       ##       ##     ##       ##     ## ##        ##  ##       ");
	UART1_PutStr(T_BrBLUE);    UART1_PutStr("\r\n\033[37m    ###    ######     ###      ###      ######   ##     ##  ######   ##   ######  ");
	UART1_PutStr(T_BrMAGENTA); UART1_PutStr("\r\n\033[37m   ##      ### ##    ##          ##     ##        ##   ##        ##  ##        ## ");
	UART1_PutStr(T_BrCYAN);    UART1_PutStr("\r\n\033[37m  ##  ##   ##  ##   ##  ##   ##  ##     ##         ## ##   ##    ##  ##  ##    ## ");
	UART1_PutStr(T_BrWHITE);   UART1_PutStr("\r\n\033[37m  ######    ####    ######    ####      ########    ###     ######  ####  ######   \033[0m\r\n");
	
	
	
	UART1_PutStr("\r\n\n [터미널 명령어 정보]");
	UART1_PutStr("\r\n  test   + Enter : 자가진단 테스트 시작 ");
	UART1_PutStr("\r\n  cinfo  + Enter : charging_info_start_cmd");
	UART1_PutStr("\r\n  stop   + Enter : charging_info_stop_cmd");
	UART1_PutStr("\r\n  reset  + Enter : system  reset ");
	UART1_PutStr("\r\n  update + Enter : f/w update mode(x-modem 1k), 반드시 UI 종료 후 실행할 것 ");
	UART1_PutStr("\r\n  version+ Enter : f/w ver, plc ver, power module info, device id info ");
	UART1_PutStr("\r\n  status + Enter : charging_status");
	UART1_PutStr("\r\n  his    + Enter : f/w ver, plc ver, power module info, device id info \r\n");
	UART1_PutStr("\r\n  xxx    + Enter : cancel command, xxx = anything");
	
	UART1_PutStr("\r\n\n [테라텀 로그 저장 방법]");
	UART1_PutStr("\r\n 1. Tera Term - Setup - Additional settings - Log - Log Rotate check, Size: 10MB, Rotate: 30으로 설정.");
	UART1_PutStr("\r\n 2. File - log.. - Timestamp check and save. ");
	
	UART1_PutStr("\r\n\n [테라텀 한글 안 깨지게 보는 방법]");
	UART1_PutStr("\r\n Tera Term - Setup - Terminal setup - Coding (receive) ");UART1_PutStr(T_BrGREEN); UART1_PutStr("KS5601 "); UART1_PutStr(T_BrWHITE);
	
	UART1_PutStr("\r\n\n [테라텀 파일전송 방법]");
	UART1_PutStr("\r\n File - Trasfer - XMODEM - Send.. 파일 선택 후 하단 Option 에서 1K 체크 후 열기 \n");
	
	UART1_PutStr("\r\n [충전기 프로그램 정보]");
	UART1_PutStr("\r\n 파워 모듈                : 윈라인(WINLINE)");
	UART1_PutStr("\r\n 펌웨어 버전              : "); UART1_PutDec(1, (u16)(fw_version*0.01F)); UART1_PutChar('.'); UART1_PutDec(1, (u16)(fw_version*0.1F)); UART1_PutChar('.'); UART1_PutDec(1, (u16)fw_version);
	UART1_PutStr("\r\n PLC_1CH 모뎀 펌웨어 버전 : "); UART1_PutDec(1, (u16)SECC_Msg[1].SECC_Status_1.seccVersionMajor); UART1_PutChar('.'); UART1_PutDec(1, (u16)SECC_Msg[1].SECC_Status_1.seccVersionMinor); UART1_PutChar('.'); UART1_PutDec(1, (u16)SECC_Msg[1].SECC_Status_1.seccVersionPatch);
//	UART1_PutStr("\r\n PLC_2CH 모뎀 펌웨어 버전 : "); UART1_PutDec(1, (u16)SECC_Msg[2].SECC_Status_1.seccVersionMajor); UART1_PutChar('.'); UART1_PutDec(1, (u16)SECC_Msg[2].SECC_Status_1.seccVersionMinor); UART1_PutChar('.'); UART1_PutDec(1, (u16)SECC_Msg[2].SECC_Status_1.seccVersionPatch);
	UART1_PutStr(" (정보가 나오지 않으면 모뎀과 통신이 안 된 것입니다. 모뎀 부팅시간 확보 후 다시 확인하세요.)\r\n");
	UART1_PutStr("\r\n 마지막 업데이트 일자     : " ); //UART1_PutDec(4, (u16)last_update_year); UART1_PutChar('.'); UART1_PutDec(2, (u16)last_update_month); UART1_PutChar('.'); UART1_PutDec(2, (u16)last_update_day);	
	UART1_PutStr(T_BOLD); printMonth(last_update_year, last_update_month); UART1_PutStr(T_RESET);
	UART1_PutStr("\r\n");
	
}


void Send_Uart_Terminal(void)
{
	
	elapsed_time++; // 100ms timer
	elapsed_time_s = elapsed_time * 0.1F; // sec timer
	
	minute = (uint16_t)(elapsed_time_s * 0.0166666667);
	hour   = (uint16_t)(minute * 0.0166666667);
	minute = (uint16_t)(minute % 60);
	second = (uint16_t)((u16)elapsed_time_s % 60);
	
	//    UART1_PutDec(1, (u16)hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)second); UART1_PutStr("."); UART1_PutDec(1, elapsed_time);	
	
	UART1_PutStr("\r\n Seq:"); 
	if     ((s16)sequence_step_1CH >= 1000) {                    UART1_PutDec(4, sequence_step_1CH);}
	else if((s16)sequence_step_1CH >=  100) {UART1_PutStr(" ");  UART1_PutDec(3, sequence_step_1CH);}
	else if((s16)sequence_step_1CH >=   10) {UART1_PutStr("  "); UART1_PutDec(2, sequence_step_1CH);}
	else if((s16)sequence_step_1CH >=    0) {UART1_PutStr("   ");UART1_PutDec(1, sequence_step_1CH);}
	
	UART1_PutStr(" MODULE:");
	if(module_on_off_G1 == 1) UART1_PutStr(" ON");
	else                   UART1_PutStr("OFF"); 
	
	UART1_PutStr(" Vref:"); 
	if     (Vout_ref_temp_G1 >= 100) {                   UART1_PutDec(3, (u16)Vout_ref_temp_G1);}
	else if(Vout_ref_temp_G1 >=  10) {UART1_PutStr(" "); UART1_PutDec(2, (u16)Vout_ref_temp_G1);}
	else if(Vout_ref_temp_G1 >=   0) {UART1_PutStr("  ");UART1_PutDec(1, (u16)Vout_ref_temp_G1);}
	
	UART1_PutStr(" Vout:"); 
	if     (Vout_G1 >= 100) {                   UART1_PutDec(3, (u16)Vout_G1);}
	else if(Vout_G1 >=  10) {UART1_PutStr(" "); UART1_PutDec(2, (u16)Vout_G1);}
	else if(Vout_G1 >=   0) {UART1_PutStr("  ");UART1_PutDec(1, (u16)Vout_G1);}
	
	UART1_PutStr(" Iref:"); 
	if     (Iout_ref_temp_G1 >= 100) {                   UART1_PutDec(3, (u16)Iout_ref_temp_G1);}
	else if(Iout_ref_temp_G1 >=  10) {UART1_PutStr(" "); UART1_PutDec(2, (u16)Iout_ref_temp_G1);}
	else if(Iout_ref_temp_G1 >=   0) {UART1_PutStr("  ");UART1_PutDec(1, (u16)Iout_ref_temp_G1);}
	
	UART1_PutStr(" Iout:");
	if     (Iout_Module_G1 >= 100) {                   UART1_PutDec(3, (u16)Iout_Module_G1);}
	else if(Iout_Module_G1 >=  10) {UART1_PutStr(" "); UART1_PutDec(2, (u16)Iout_Module_G1);}
	else if(Iout_Module_G1 >=   1) {UART1_PutStr("  ");UART1_PutDec(1, (u16)Iout_Module_G1);}
	
	UART1_PutStr(" M1:"); 
	if     (module_info[0].ufVout.fValue >= 100) {                   UART1_PutDec(3, (u16)module_info[0].ufVout.fValue);}
	else if(module_info[0].ufVout.fValue >=  10) {UART1_PutStr(" "); UART1_PutDec(2, (u16)module_info[0].ufVout.fValue);}
	else if(module_info[0].ufVout.fValue >=   0) {UART1_PutStr("  ");UART1_PutDec(1, (u16)module_info[0].ufVout.fValue);}
	UART1_PutStr("[V]");
	
//	UART1_PutStr(" M2:");
//	if     (module_info[1].ufVout.fValue >= 100) {                   UART1_PutDec(3, (u16)module_info[1].ufVout.fValue);}
//	else if(module_info[1].ufVout.fValue >=  10) {UART1_PutStr(" "); UART1_PutDec(2, (u16)module_info[1].ufVout.fValue);}
//	else if(module_info[1].ufVout.fValue >=   0) {UART1_PutStr("  ");UART1_PutDec(1, (u16)module_info[1].ufVout.fValue);}
//	UART1_PutStr("[V]");
//	
//	UART1_PutStr(" M3:");
//	if     (module_info[2].ufVout.fValue >= 100) {                   UART1_PutDec(3, (u16)module_info[2].ufVout.fValue);}
//	else if(module_info[2].ufVout.fValue >=  10) {UART1_PutStr(" "); UART1_PutDec(2, (u16)module_info[2].ufVout.fValue);}
//	else if(module_info[2].ufVout.fValue >=   0) {UART1_PutStr("  ");UART1_PutDec(1, (u16)module_info[2].ufVout.fValue);}
//	UART1_PutStr("[V]");
//	
//	UART1_PutStr(" M4:");
//	if     (module_info[3].ufVout.fValue >= 100) {                   UART1_PutDec(3, (u16)module_info[3].ufVout.fValue);}
//	else if(module_info[3].ufVout.fValue >=  10) {UART1_PutStr(" "); UART1_PutDec(2, (u16)module_info[3].ufVout.fValue);}
//	else if(module_info[3].ufVout.fValue >=   0) {UART1_PutStr("  ");UART1_PutDec(1, (u16)module_info[3].ufVout.fValue);}
//	UART1_PutStr("[V]");
//	
//	UART1_PutStr(" M5:");
//	if     (module_info[4].ufVout.fValue >= 100) {                   UART1_PutDec(3, (u16)module_info[4].ufVout.fValue);}
//	else if(module_info[4].ufVout.fValue >=  10) {UART1_PutStr(" "); UART1_PutDec(2, (u16)module_info[4].ufVout.fValue);}
//	else if(module_info[4].ufVout.fValue >=   0) {UART1_PutStr("  ");UART1_PutDec(1, (u16)module_info[4].ufVout.fValue);}
//	UART1_PutStr("[V]");
	
	UART1_PutStr(" "); // 구분
	
	UART1_PutStr(" M1:");
	if     (module_info[0].ufIout.fValue >=  10) {                  UART1_PutDec(2, (u16)module_info[0].ufIout.fValue);}
	else if(module_info[0].ufIout.fValue >=   0) {UART1_PutStr(" ");UART1_PutDec(1, (u16)module_info[0].ufIout.fValue);}
	UART1_PutStr("[A]");
	
//	UART1_PutStr(" M2:");
//	if     (module_info[1].ufIout.fValue >=  10) {                  UART1_PutDec(2, (u16)module_info[1].ufIout.fValue);}
//	else if(module_info[1].ufIout.fValue >=   0) {UART1_PutStr(" ");UART1_PutDec(1, (u16)module_info[1].ufIout.fValue);}	
//	UART1_PutStr("[A]");
//	
//	UART1_PutStr(" M3:");
//	if     (module_info[2].ufIout.fValue >=  10) {                  UART1_PutDec(2, (u16)module_info[2].ufIout.fValue);}
//	else if(module_info[2].ufIout.fValue >=   0) {UART1_PutStr(" ");UART1_PutDec(1, (u16)module_info[2].ufIout.fValue);}
//	UART1_PutStr("[A]");
//	
//	UART1_PutStr(" M4:");
//	if     (module_info[3].ufIout.fValue >=  10) {                  UART1_PutDec(2, (u16)module_info[3].ufIout.fValue);}
//	else if(module_info[3].ufIout.fValue >=   0) {UART1_PutStr(" ");UART1_PutDec(1, (u16)module_info[3].ufIout.fValue);}	
//	UART1_PutStr("[A]");
//	
//	UART1_PutStr(" M5:");
//	if     (module_info[4].ufIout.fValue >=  10) {                  UART1_PutDec(2, (u16)module_info[4].ufIout.fValue);}
//	else if(module_info[4].ufIout.fValue >=   0) {UART1_PutStr(" ");UART1_PutDec(1, (u16)module_info[4].ufIout.fValue);}	
//	UART1_PutStr("[A] ");
	
	UART1_PutStr(" CP:"); 
	if(SECC_Msg[1].SECC_Status_2.cpVoltage_12 < 0) {UART1_PutStr("-");UART1_PutDec(3, ~(SECC_Msg[1].SECC_Status_2.cpVoltage_12)+1);} // short형 
	else                                        {UART1_PutStr(" ");UART1_PutDec(3, (u16)SECC_Msg[1].SECC_Status_2.cpVoltage_12);}
	
	UART1_PutStr(" ");
	
	if     (SECC_Msg[1].SECC_Status_2.CP_Status_0.bit.cpOscillator == TRUE  && cpOscillator_old != TRUE) {UART1_PutStr(" PWM:");  UART1_PutStr("  ON"); }
	else if(SECC_Msg[1].SECC_Status_2.CP_Status_0.bit.cpOscillator == FALSE && cpOscillator_old != FALSE){UART1_PutStr(" PWM:");  UART1_PutStr(" OFF"); }
	
	cpOscillator_old = SECC_Msg[1].SECC_Status_2.CP_Status_0.bit.cpOscillator;
	
	
	if     (SECC_Msg[1].SECC_Status_1.seccStatusCode == Idle_Wait)                             
	{
		UART1_PutStr(" Idle_Wait              ");
	} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Idle                                 && seccStatusCode_old != Idle)                                  
	{
		UART1_PutStr(T_BrGREEN);
		UART1_PutStr(" Idle                   ");
		UART1_PutStr(T_RESET);
		UART1_PutStr(" elapsed_time Init 0:00:00.0"); elapsed_time = 0;
	} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Ready                                && seccStatusCode_old != Ready)                                 {UART1_PutStr(T_BrGREEN); UART1_PutStr(" Ready                  "); UART1_PutStr(T_RESET); UART1_PutStr(" elapsed_time "); UART1_PutDec(1, (u16)hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)second); UART1_PutStr("."); UART1_PutDec(1, elapsed_time); elapsed_time = 0;}
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Low_Level_Comm                       && seccStatusCode_old != Low_Level_Comm)                        
	{
		UART1_PutStr(T_BrGREEN);
		UART1_PutStr(" Low_Level_Comm         ");
		UART1_PutStr(T_RESET);
		UART1_PutStr(" elapsed_time "); UART1_PutDec(1, (u16)hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)second); UART1_PutStr("."); UART1_PutDec(1, elapsed_time); elapsed_time = 0;
		UART1_PutStr(" evccAttn: "); UART1_PutDec(2, SECC_Msg[1].SECC_EVCC_Info.evccAttn_6);
		UART1_PutStr(" evccMAC: "); //0~5
		
		UART1_PutHex( SECC_Msg[1].SECC_EVCC_Info.evccMAC_0); UART1_PutStr("-");
		UART1_PutHex( SECC_Msg[1].SECC_EVCC_Info.evccMAC_1); UART1_PutStr("-");
		UART1_PutHex( SECC_Msg[1].SECC_EVCC_Info.evccMAC_2); UART1_PutStr("-");
		UART1_PutHex( SECC_Msg[1].SECC_EVCC_Info.evccMAC_3); UART1_PutStr("-");
		UART1_PutHex( SECC_Msg[1].SECC_EVCC_Info.evccMAC_4); UART1_PutStr("-");
		UART1_PutHex( SECC_Msg[1].SECC_EVCC_Info.evccMAC_5);
		
		UART1_PutStr(" sessionID: "); UART1_Put4Hex(SECC_Msg[1].SECC_Session_ID.sessionID_High);
		UART1_Put4Hex(SECC_Msg[1].SECC_Session_ID.sessionID_Low);
	} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == High_Level_Comm                      && seccStatusCode_old != High_Level_Comm)                       
	{
		UART1_PutStr(T_BrGREEN);
		UART1_PutStr(" High_Level_Comm        ");
		UART1_PutStr(T_RESET);
		UART1_PutStr(" elapsed_time "); UART1_PutDec(1, (u16)hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)second); UART1_PutStr("."); UART1_PutDec(1, elapsed_time); elapsed_time = 0; 
		UART1_PutStr(" evccAttn: "); UART1_PutDec(2, SECC_Msg[1].SECC_EVCC_Info.evccAttn_6);
		UART1_PutStr(" evccMAC: "); //0~5
		
		UART1_PutHex( SECC_Msg[1].SECC_EVCC_Info.evccMAC_0); UART1_PutStr("-");
		UART1_PutHex( SECC_Msg[1].SECC_EVCC_Info.evccMAC_1); UART1_PutStr("-");
		UART1_PutHex( SECC_Msg[1].SECC_EVCC_Info.evccMAC_2); UART1_PutStr("-");
		UART1_PutHex( SECC_Msg[1].SECC_EVCC_Info.evccMAC_3); UART1_PutStr("-");
		UART1_PutHex( SECC_Msg[1].SECC_EVCC_Info.evccMAC_4); UART1_PutStr("-");
		UART1_PutHex( SECC_Msg[1].SECC_EVCC_Info.evccMAC_5);
		
		UART1_PutStr(" sessionID: "); UART1_Put4Hex(SECC_Msg[1].SECC_Session_ID.sessionID_High);
		UART1_Put4Hex(SECC_Msg[1].SECC_Session_ID.sessionID_Low);
	} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Authorization_ContractAuthentication && seccStatusCode_old != Authorization_ContractAuthentication)  {UART1_PutStr(T_BrGREEN); UART1_PutStr(" Authorization          "); UART1_PutStr(T_RESET); UART1_PutStr(" elapsed_time "); UART1_PutDec(1, (u16)hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)second); UART1_PutStr("."); UART1_PutDec(1, elapsed_time); elapsed_time = 0;} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == ChargeParameterDiscovery             && seccStatusCode_old != ChargeParameterDiscovery)              {UART1_PutStr(T_BrGREEN); UART1_PutStr(" ParameterDiscovery     "); UART1_PutStr(T_RESET); UART1_PutStr(" elapsed_time "); UART1_PutDec(1, (u16)hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)second); UART1_PutStr("."); UART1_PutDec(1, elapsed_time); elapsed_time = 0;} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == CableCheck                           && seccStatusCode_old != CableCheck)                            {UART1_PutStr(T_BrGREEN); UART1_PutStr(" CableCheck             "); UART1_PutStr(T_RESET); UART1_PutStr(" elapsed_time "); UART1_PutDec(1, (u16)hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)second); UART1_PutStr("."); UART1_PutDec(1, elapsed_time); elapsed_time = 0;} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == PreCharge                            && seccStatusCode_old != PreCharge)                             {UART1_PutStr(T_BrGREEN); UART1_PutStr(" PreCharge              "); UART1_PutStr(T_RESET); UART1_PutStr(" elapsed_time "); UART1_PutDec(1, (u16)hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)second); UART1_PutStr("."); UART1_PutDec(1, elapsed_time); elapsed_time = 0;} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Charging)                              
	{
		if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Charging                              && seccStatusCode_old != Charging )                             {UART1_PutStr(T_BrGREEN); UART1_PutStr(" Charging               "); UART1_PutStr(T_RESET); UART1_PutStr(" elapsed_time "); UART1_PutDec(1, (u16)hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)second); UART1_PutStr("."); UART1_PutDec(1, elapsed_time); elapsed_time = 0;}
		
		if(view_count++ >= 100) // 약 10초마다
		{
			view_count = 0;
			
			UART1_PutStr(" SOC:");
			if     ((s16)SECC_Msg[1].SECC_EV_SoC_Related_Parameters.evSOC_1 >= 100) {                   UART1_PutDec(3, (u16)SECC_Msg[1].SECC_EV_SoC_Related_Parameters.evSOC_1);}
			else if((s16)SECC_Msg[1].SECC_EV_SoC_Related_Parameters.evSOC_1 >=  10) {UART1_PutStr(" "); UART1_PutDec(2, (u16)SECC_Msg[1].SECC_EV_SoC_Related_Parameters.evSOC_1);}
			else if((s16)SECC_Msg[1].SECC_EV_SoC_Related_Parameters.evSOC_1 >=   0) {UART1_PutStr("  ");UART1_PutDec(1, (u16)SECC_Msg[1].SECC_EV_SoC_Related_Parameters.evSOC_1);}
			UART1_PutStr("[%]");
			
			if(SECC_Msg[1].SECC_EV_SoC_Related_Parameters.remainingTimeToBulkSOC_67 != 0) {UART1_PutStr(" Bulk_Time: "); UART1_PutDec(3, (u16)(SECC_Msg[1].SECC_EV_SoC_Related_Parameters.remainingTimeToBulkSOC_67 * 0.166666666F)); UART1_PutStr(" Min");} // 1/6 = 0.166666666F
			else                                                                       {UART1_PutStr(" Full_Time: "); UART1_PutDec(3, (u16)(SECC_Msg[1].SECC_EV_SoC_Related_Parameters.remainingTimeToFullSOC_45 * 0.166666666F)); UART1_PutStr(" Min");} // ex) 198 -> 198/6 = 33min	
			
			
			if     (Vout_G1 * Iout_Module_G1 * 0.001F >= 100) {                   UART1_PutDec(3, (u16)(Vout_G1 * Iout_Module_G1 * 0.001F));}
			else if(Vout_G1 * Iout_Module_G1 * 0.001F >=  10) {UART1_PutStr(" "); UART1_PutDec(2, (u16)(Vout_G1 * Iout_Module_G1 * 0.001F));}
			else if(Vout_G1 * Iout_Module_G1 * 0.001F >=   0) {UART1_PutStr("  ");UART1_PutDec(1, (u16)(Vout_G1 * Iout_Module_G1 * 0.001F));}
			UART1_PutStr("[kW] ");
			
			// 	UART1_PutDec(1, (u16)(dc_electric_energy * 100 * 0.0001F));
			//	UART1_PutDec(1, (u16)(dc_electric_energy * 100 * 0.001F));
			//	UART1_PutDec(1, (u16)(dc_electric_energy * 100 * 0.01F));
			if     (dc_electric_energy >= 100) {                    UART1_PutDec(3, (u16)(dc_electric_energy));}
			else if(dc_electric_energy >=  10) {UART1_PutStr(" ");  UART1_PutDec(2, (u16)(dc_electric_energy));}
			else if(dc_electric_energy >=   0) {UART1_PutStr("  "); UART1_PutDec(1, (u16)(dc_electric_energy));}
			UART1_PutChar('.'); 
			UART1_PutDec(1, (u16)(dc_electric_energy * 100 * 0.1F));
			UART1_PutDec(1, (u16)(dc_electric_energy * 100));
			UART1_PutStr("[kWh] ");
			
			//            UART1_PutStr("EV current_demand: "); UART1_PutDec(3, (u16)(SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetCurrent_23 * 0.1F)); UART1_PutStr(" ");
			
			UART1_PutDec(1, (u16)hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)second); UART1_PutStr("."); UART1_PutDec(1, elapsed_time);
			
			//      UART1_PutStr(" DC_METER Iout: "); UART1_PutDec(3, DCIout_1CH); // 2022.09.30
		}
		
	} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Renegotiation                        && seccStatusCode_old != Renegotiation)                         {UART1_PutStr(" Renegotiation          ");} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == EV_Init_Stop_Charging                && seccStatusCode_old != EV_Init_Stop_Charging)                 {UART1_PutStr(T_BrRED); UART1_PutStr(" EV_Init_Stop_Charging  ");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == EVSE_Init_Stop_Charging              && seccStatusCode_old != EVSE_Init_Stop_Charging)               {UART1_PutStr(T_BrGREEN); UART1_PutStr(" EVSE_Init_Stop_Charging"); UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Terminate                            && seccStatusCode_old != Terminate)                             
	{
		UART1_PutStr(T_BrRED); 
		UART1_PutStr("Terminate              "); 
		UART1_PutStr(T_RESET);
		
		UART1_PutStr(" DC electric energy: ");
		if     (dc_electric_energy >= 100) {                    UART1_PutDec(3, (u16)(dc_electric_energy));}
		else if(dc_electric_energy >=  10) {UART1_PutStr(" ");  UART1_PutDec(2, (u16)(dc_electric_energy));}
		else if(dc_electric_energy >=   0) {UART1_PutStr("  "); UART1_PutDec(1, (u16)(dc_electric_energy));}
		UART1_PutChar('.'); 
		UART1_PutDec(1, (u16)(dc_electric_energy * 100 * 0.1F));
		UART1_PutDec(1, (u16)(dc_electric_energy * 100));
		UART1_PutStr("[kWh] ");
		
		UART1_PutDec(1, (u16)hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)second); UART1_PutStr("."); UART1_PutDec(1, elapsed_time);
	} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == CertificateInstallation              && seccStatusCode_old != CertificateInstallation)               {UART1_PutStr(" CertificateInstallation");} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == CertificatedUpdate                   && seccStatusCode_old != CertificatedUpdate)                    {UART1_PutStr(" CertificatedUpdate     ");} 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == SECC_ERROR                           && seccStatusCode_old != SECC_ERROR)                            {UART1_PutStr(" SECC_ERROR             ");} 
	
	SOC__old = SECC_Msg[1].SECC_EV_SoC_Related_Parameters.evSOC_1;
	
	seccStatusCode_old = SECC_Msg[1].SECC_Status_1.seccStatusCode;
	
	if     (SECC_Msg[1].SECC_Status_1.seccErrorCode == SECC_NO_ERROR                          && seccErrorCode_old != SECC_NO_ERROR)                          {UART1_PutStr(T_RESET); UART1_PutStr(" seccError:"); UART1_PutStr("                  SECC_NO_ERROR");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED                                 && seccErrorCode_old != FAILED)                                 {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("                         FAILED");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_SequenceError                   && seccErrorCode_old != FAILED_SequenceError)                   {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("                  SequenceError");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ServiceIDInvalid                && seccErrorCode_old != FAILED_ServiceIDInvalid)                {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("               ServiceIDInvalid");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_UnknownSession                  && seccErrorCode_old != FAILED_UnknownSession)                  {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("                 UnknownSession");UART1_PutStr(T_RESET);}  
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ServiceSelectionInvalid         && seccErrorCode_old != FAILED_ServiceSelectionInvalid)         {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("        ServiceSelectionInvalid");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_PaymentSelectionInvalid         && seccErrorCode_old != FAILED_PaymentSelectionInvalid)         {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("        PaymentSelectionInvalid");UART1_PutStr(T_RESET);}  
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CertificateExpired              && seccErrorCode_old != FAILED_CertificateExpired)              {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("             CertificateExpired");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_SignatureError                  && seccErrorCode_old != FAILED_SignatureError)                  {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("                 SignatureError");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_NoCertificateAvailable          && seccErrorCode_old != FAILED_NoCertificateAvailable)          {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("         NoCertificateAvailable");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CerChainError                   && seccErrorCode_old != FAILED_CerChainError)                   {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("                  CerChainError");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ChallengeInvalid                && seccErrorCode_old != FAILED_ChallengeInvalid)                {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("               ChallengeInvalid");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ContractCanceled                && seccErrorCode_old != FAILED_ContractCanceled)                {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("               ContractCanceled");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_WrongChargeParameter            && seccErrorCode_old != FAILED_WrongChargeParameter)            {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("           WrongChargeParameter");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_PowerDeliveryNotApplied         && seccErrorCode_old != FAILED_PowerDeliveryNotApplied)         {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("        PowerDeliveryNotApplied");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_TariffSelectionInvalid          && seccErrorCode_old != FAILED_TariffSelectionInvalid)          {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("         TariffSelectionInvalid");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ChargingProfileInvalid          && seccErrorCode_old != FAILED_ChargingProfileInvalid)          {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("         ChargingProfileInvalid");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_MeteringSignatureNotValid       && seccErrorCode_old != FAILED_MeteringSignatureNotValid)       {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("      MeteringSignatureNotValid");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_NoChargeServiceSelected         && seccErrorCode_old != FAILED_NoChargeServiceSelected)         {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("        NoChargeServiceSelected");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_WrongEnergyTransferMode         && seccErrorCode_old != FAILED_WrongEnergyTransferMode)         {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("        WrongEnergyTransferMode");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ContactorError                  && seccErrorCode_old != FAILED_ContactorError)                  {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("                 ContactorError");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CertificateNotAllowedAtThisEVSE && seccErrorCode_old != FAILED_CertificateNotAllowedAtThisEVSE) {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("CertificateNotAllowedAtThisEVSE");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CertificateRevoked              && seccErrorCode_old != FAILED_CertificateRevoked)              {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("             CertificateRevoked");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_NoNegotiation                   && seccErrorCode_old != FAILED_NoNegotiation)                   {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("                  NoNegotiation");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_CommunicationSetup             && seccErrorCode_old != TIMEOUT_CommunicationSetup)             {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("     TIMEOUT_CommunicationSetup");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_Sequence                       && seccErrorCode_old != TIMEOUT_Sequence)                       {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("               TIMEOUT_Sequence");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_NotificationMaxDelay           && seccErrorCode_old != TIMEOUT_NotificationMaxDelay)           {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("   TIMEOUT_NotificationMaxDelay");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_WeldingDetection               && seccErrorCode_old != TIMEOUT_WeldingDetection)               {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("       TIMEOUT_WeldingDetection");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_WrongCPLevel                    && seccErrorCode_old != FAILED_WrongCPLevel)                    {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("            FAILED_WrongCPLevel");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ProximityError                  && seccErrorCode_old != FAILED_ProximityError)                  {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("          FAILED_ProximityError");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_HLCError                        && seccErrorCode_old != FAILED_HLCError)                        {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("                FAILED_HLCError");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_HeartbeatError                  && seccErrorCode_old != FAILED_HeartbeatError)                  {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("          FAILED_HeartbeatError");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_EVSECANInit                     && seccErrorCode_old != FAILED_EVSECANInit)                     {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("             FAILED_EVSECANInit");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAULT_HPGPLinkDown                     && seccErrorCode_old != FAULT_HPGPLinkDown)                     {UART1_PutStr(T_BrRED); UART1_PutStr(" seccError:"); UART1_PutStr("             FAULT_HPGPLinkDown");UART1_PutStr(T_RESET);} 
	
	
	seccErrorCode_old = SECC_Msg[1].SECC_Status_1.seccErrorCode;
	
	if     (SECC_Msg[1].SECC_Status_1.evErrorCode == EV_NO_ERROR                          && evErrorCode_old != EV_NO_ERROR)                              {UART1_PutStr(T_RESET); UART1_PutStr(" evError:"); UART1_PutStr("                  EV_NO_ERROR");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_RESSTemperatureInhibit        && evErrorCode_old != FAILED_RESSTemperatureInhibit)            {UART1_PutStr(T_BrRED); UART1_PutStr(" evError:"); UART1_PutStr("       RESSTemperatureInhibit");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_EVShiftPosition               && evErrorCode_old != FAILED_EVShiftPosition)                   {UART1_PutStr(T_BrRED); UART1_PutStr(" evError:"); UART1_PutStr("              EVShiftPosition");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargerConnectorLockFault     && evErrorCode_old != FAILED_ChargerConnectorLockFault)         {UART1_PutStr(T_BrRED); UART1_PutStr(" evError:"); UART1_PutStr("           ConnectorLockFault");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_EVRESSMalfunction             && evErrorCode_old != FAILED_EVRESSMalfunction)                 {UART1_PutStr(T_BrRED); UART1_PutStr(" evError:"); UART1_PutStr("            EVRESSMalfunction");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargingCurrentdifferential   && evErrorCode_old != FAILED_ChargingCurrentdifferential)       {UART1_PutStr(T_BrRED); UART1_PutStr(" evError:"); UART1_PutStr("  ChargingCurrentdifferential");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargingVoltageOutOfRange     && evErrorCode_old != FAILED_ChargingVoltageOutOfRange)         {UART1_PutStr(T_BrRED); UART1_PutStr(" evError:"); UART1_PutStr("    ChargingVoltageOutOfRange");UART1_PutStr(T_RESET);} 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargingSystemIncompatibility && evErrorCode_old != FAILED_ChargingSystemIncompatibility)     {UART1_PutStr(T_BrRED); UART1_PutStr(" evError:"); UART1_PutStr("ChargingSystemIncompatibility");UART1_PutStr(T_RESET);} 
	
	
	evErrorCode_old = SECC_Msg[1].SECC_Status_1.evErrorCode;
	if(coupler_temperature_P_1CH >= 75)             {UART1_PutStr(" COUPLER:"); UART1_PutDec(2, (uint16_t)coupler_temperature_P_1CH); UART1_PutStr("[℃]");}
	
	if(module1_can_communication_rx_fault == _rx_fault) UART1_PutStr(" M1: CAN_RX_failure "); // 2022.09.30
	if(module2_can_communication_rx_fault == _rx_fault) UART1_PutStr(" M2: CAN_RX_failure ");
	if(module3_can_communication_rx_fault == _rx_fault) UART1_PutStr(" M3: CAN_RX_failure ");
	if(module4_can_communication_rx_fault == _rx_fault) UART1_PutStr(" M4: CAN_RX_failure ");
	if(module5_can_communication_rx_fault == _rx_fault) UART1_PutStr(" M5: CAN_RX_failure "); //모듈이 4개면 현재 라인 주석처리
	
}


void Send_Uart_Terminal_default(void)
{
	/*1CH status*/
	UART1_PutStr(" \r CP_1ch:");
	if(SECC_Msg[1].SECC_Status_2.cpVoltage_12 < 0) {UART1_PutStr("-");UART1_PutDec(3, ~(SECC_Msg[1].SECC_Status_2.cpVoltage_12)+1);} // short형 
	else                                           {UART1_PutStr(" ");UART1_PutDec(3, (u16)SECC_Msg[1].SECC_Status_2.cpVoltage_12);}
	
	
	UART1_PutStr(" COUPLER_1ch:"); 
	if   (coupler_temperature_P_1CH == 1) UART1_PutStr("OPEN"); 
	else                                  UART1_PutDec(4, (uint16_t)coupler_temperature_P_1CH);
	UART1_PutStr(" [℃] ");
	
	if(Digital_input.bit.I_RY1) UART1_PutStr(" Relay1: ON"); 
	else                        UART1_PutStr(" Relay1:OFF"); 
	if(Digital_input.bit.I_RY2) UART1_PutStr(" Relay2: ON");
	else                        UART1_PutStr(" Relay2:OFF");
	if(Digital_input.bit.I_FUSE_8 == 1) UART1_PutStr(" FUSE8:OK"); 
	else                                  UART1_PutStr(" FUSE8: OPEN!"); 
	if(Digital_input.bit.I_FUSE_9 == 1) UART1_PutStr(" FUSE9:OK"); 
	else                                  UART1_PutStr(" FUSE9: OPEN!"); 
	
	UART1_PutStr(" MODULE:");
	if(module_on_off_G1 == 1) UART1_PutStr(" ON");
	else                      UART1_PutStr("OFF"); 
	
	UART1_PutStr(" Vref_G1:");    UART1_PutDec(3, (u16)Vout_ref_temp_G1);
	UART1_PutStr(" Vout_G1:");    UART1_PutDec(3, (u16)Vout_G1);
	UART1_PutStr(" Iref:");       UART1_PutDec(3, (u16)Iout_ref_temp_G1);
	UART1_PutStr(" Iout:");       UART1_PutDec(3, (u16)Iout_Module_G1);
	//    UART1_PutStr(" "); // 구분
	UART1_PutStr(" M1:"); UART1_PutDec(3, (u16)module_info[0].ufVout.fValue); UART1_PutStr("[V]");
//	UART1_PutStr(" M2:"); UART1_PutDec(3, (u16)module_info[2].ufVout.fValue); UART1_PutStr("[V]");
//	UART1_PutStr(" M3:"); UART1_PutDec(3, (u16)module_info[3].ufVout.fValue); UART1_PutStr("[V]");
//	UART1_PutStr(" M4:"); UART1_PutDec(3, (u16)module_info[4].ufVout.fValue); UART1_PutStr("[V]");
	
	//	UART1_PutStr(" "); // 구분
	UART1_PutStr(" M1:"); UART1_PutDec(2, (u16)module_info[0].ufIout.fValue); UART1_PutStr("[A]");
//	UART1_PutStr(" M2:"); UART1_PutDec(2, (u16)module_info[2].ufIout.fValue); UART1_PutStr("[A]");
//	UART1_PutStr(" M3:"); UART1_PutDec(2, (u16)module_info[3].ufIout.fValue); UART1_PutStr("[A]");
//	UART1_PutStr(" M4:"); UART1_PutDec(2, (u16)module_info[4].ufIout.fValue); UART1_PutStr("[A]");
	
	
	
	//	UART1_PutStr(" "); // 구분
	if     (SECC_Msg[1].SECC_Status_1.seccStatusCode == Idle_Wait                             ) UART1_PutStr(" Idle_Wait              ");                              
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Idle                                  ) UART1_PutStr(" Idle                   ");                           
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Ready                                 ) UART1_PutStr(" Ready                  "); 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Low_Level_Comm                        ) UART1_PutStr(" Low_Level_Comm         ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == High_Level_Comm                       ) UART1_PutStr(" High_Level_Comm        "); 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Authorization_ContractAuthentication  ) UART1_PutStr(" Authorization          ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == ChargeParameterDiscovery              ) UART1_PutStr(" ParameterDiscovery     ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == CableCheck                            ) UART1_PutStr(" CableCheck             ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == PreCharge                             ) UART1_PutStr(" PreCharge              ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Charging                              ) UART1_PutStr(" Charging               ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Renegotiation                         ) UART1_PutStr(" Renegotiation          ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == EV_Init_Stop_Charging                 ) UART1_PutStr(" EV_Init_Stop_Charging  ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == EVSE_Init_Stop_Charging               ) UART1_PutStr(" EVSE_Init_Stop_Charging"); 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Terminate                             ) UART1_PutStr(" Terminate              ");    
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == CertificateInstallation               ) UART1_PutStr(" CertificateInstallation");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == CertificatedUpdate                    ) UART1_PutStr(" CertificatedUpdate     ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == SECC_ERROR                            ) UART1_PutStr(" SECC_ERROR             ");
	//	UART1_PutStr(" "); // 구분
	if     (SECC_Msg[1].SECC_Status_1.seccErrorCode == SECC_NO_ERROR                          ) UART1_PutStr(" SECC_NO_ERROR                  ");
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED                                 ) UART1_PutStr(" FAILED                         ");
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_SequenceError                   ) UART1_PutStr(" SequenceError                  ");
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ServiceIDInvalid                ) UART1_PutStr(" ServiceIDInvalid               ");
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_UnknownSession                  ) UART1_PutStr(" UnknownSession                 ");  
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ServiceSelectionInvalid         ) UART1_PutStr(" ServiceSelectionInvalid        "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_PaymentSelectionInvalid         ) UART1_PutStr(" PaymentSelectionInvalid        ");  
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CertificateExpired              ) UART1_PutStr(" CertificateExpired             "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_SignatureError                  ) UART1_PutStr(" SignatureError                 "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_NoCertificateAvailable          ) UART1_PutStr(" NoCertificateAvailable         "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CerChainError                   ) UART1_PutStr(" CerChainError                  "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ChallengeInvalid                ) UART1_PutStr(" ChallengeInvalid               "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ContractCanceled                ) UART1_PutStr(" ContractCanceled               "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_WrongChargeParameter            ) UART1_PutStr(" WrongChargeParameter           "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_PowerDeliveryNotApplied         ) UART1_PutStr(" PowerDeliveryNotApplied        "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_TariffSelectionInvalid          ) UART1_PutStr(" TariffSelectionInvalid         "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ChargingProfileInvalid          ) UART1_PutStr(" ChargingProfileInvalid         "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_MeteringSignatureNotValid       ) UART1_PutStr(" MeteringSignatureNotValid      "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_NoChargeServiceSelected         ) UART1_PutStr(" NoChargeServiceSelected        "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_WrongEnergyTransferMode         ) UART1_PutStr(" WrongEnergyTransferMode        "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ContactorError                  ) UART1_PutStr(" ContactorError                 "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CertificateNotAllowedAtThisEVSE ) UART1_PutStr(" CertificateNotAllowedAtThisEVSE"); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CertificateRevoked              ) UART1_PutStr(" CertificateRevoked             "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_NoNegotiation                   ) UART1_PutStr(" NoNegotiation                  "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_CommunicationSetup             ) UART1_PutStr(" TIMEOUT_CommunicationSetup     "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_Sequence                       ) UART1_PutStr(" TIMEOUT_Sequence               "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_NotificationMaxDelay           ) UART1_PutStr(" TIMEOUT_NotificationMaxDelay   "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_WeldingDetection               ) UART1_PutStr(" TIMEOUT_WeldingDetection       "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_WrongCPLevel                    ) UART1_PutStr(" FAILED_WrongCPLevel            "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ProximityError                  ) UART1_PutStr(" FAILED_ProximityError          "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_HLCError                        ) UART1_PutStr(" FAILED_HLCError                "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_HeartbeatError                  ) UART1_PutStr(" FAILED_HeartbeatError          "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_EVSECANInit                     ) UART1_PutStr(" FAILED_EVSECANInit             "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAULT_HPGPLinkDown                     ) UART1_PutStr(" FAULT_HPGPLinkDown             "); 
	//    UART1_PutStr(" "); // 구분
	if     (SECC_Msg[1].SECC_Status_1.evErrorCode == EV_NO_ERROR                              ) UART1_PutStr(" EV_NO_ERROR                  "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_RESSTemperatureInhibit            ) UART1_PutStr(" RESSTemperatureInhibit       "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_EVShiftPosition                   ) UART1_PutStr(" EVShiftPosition              "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargerConnectorLockFault         ) UART1_PutStr(" ConnectorLockFault           "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_EVRESSMalfunction                 ) UART1_PutStr(" EVRESSMalfunction            "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargingCurrentdifferential       ) UART1_PutStr(" ChargingCurrentdifferential  "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargingVoltageOutOfRange         ) UART1_PutStr(" ChargingVoltageOutOfRange    "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargingSystemIncompatibility     ) UART1_PutStr(" ChargingSystemIncompatibility"); 
	
	
}


void term_get_cmd_excute(void)
{
	
	
	if(send_uart_term_start)       {Send_Uart_Terminal();                                           }
	if(finish_newline_flag)        
	{
		UART1_PutStr(evsis_text1);
		UART1_PutStr(evsis_text2);
		UART1_PutStr(evsis_text3);
		UART1_PutStr(evsis_text4);
		UART1_PutStr(evsis_text5);
		UART1_PutStr(evsis_text6);
		UART1_PutStr(evsis_text7);
		UART1_PutStr("\r 시스템 전체 동작 시간     : "); UART1_PutDec(3, (u16)g_day); UART1_PutStr(" [Day] ");UART1_PutDec(2, (u16)g_hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)g_minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)g_second);
		
		UART1_PutStr("\r\n\n\n\n\n\n\n\n\n\n"); 
		finish_newline_flag = 0;
	}
	
	
	if     (system_cmd == charging_info_start_cmd                     ){system_cmd = none; UART1_PutStr("\r\n\n 충전 정보 표시를 시작합니다. \r\n\n "); send_uart_term_start_flag = 1;}
	else if(system_cmd == charging_info_stop_cmd                      ){system_cmd = none; UART1_PutStr("\r\n\n 충전 정보 표시를 종료합니다. \r\n\n "); send_uart_term_start_flag = 0;}
	else if(system_cmd == system_reset_cmd && sequence_step_1CH == 0)
	{
		system_cmd = none; 
		UART1_PutStr("\r\n\n 사용자 시스템 리셋!!  \r\n\n "); 
		NVIC_SystemReset();            
	}
	else if(system_cmd == fw_update_cmd && sequence_step_1CH == 0   )
	{
		system_cmd = none; 
		UART1_PutStr(evsis_text1);
		UART1_PutStr(evsis_text2);
		UART1_PutStr(evsis_text3);
		UART1_PutStr(evsis_text4);
		UART1_PutStr(evsis_text5);
		UART1_PutStr(evsis_text6);
		UART1_PutStr(evsis_text7);		
		UART1_PutStr("\r\n ********************************************************************************************************** \r\n");				
		UART1_PutStr("\r\n   펌웨어 업데이트 모드입니다. 파일 전송을 기다리는 중입니다. \r\n");
		UART1_PutStr("\r\n   하이퍼 터미널 또는 응용프로그램에서 x-modem(1k)로 60초 이내에 bin(바이너리) 파일 전송을 시작해 주세요. 60초가 지나면 자동 복구됩니다. baud rate는 38400입니다. \n ");
		UART1_PutStr("\r\n   펌웨어 업데이트 모드 실행 전에 HMI의 충전기(응용) 프로그램을 종료하지 않았다면 응용 프로그램 종료 후 반드시 업데이트 파일을 전송해야합니다. \r\n");
		UART1_PutStr("\r\n   업데이트 파일을 전송하지 않으면 메인 프로그램으로 복구되지 않을 수 있습니다. \r\n"); 
		UART1_PutStr("\r\n   1분 이상 이 상태가 지속되면 컨트롤보드의 리셋 버튼을 눌러서 부트 프로그램이 종료될 때까지(1분) 기다립니다. 또는 부트 프로그램과 메인 프로그램을 다시 다운로드 하세요. \n ");
		UART1_PutStr("\r\n ********************************************************************************************************** \r\n");
		FW_update();
	}
	else if(system_cmd == version_cmd)
	{
		system_cmd = none; 				
		UART1_PutStr(evsis_text1);
		UART1_PutStr(evsis_text2);
		UART1_PutStr(evsis_text3);
		UART1_PutStr(evsis_text4);
		UART1_PutStr(evsis_text5);
		UART1_PutStr(evsis_text6);
		UART1_PutStr(evsis_text7);	
		UART1_PutStr("\r\n [충전기 프로그램 정보]");
		UART1_PutStr("\r\n 펌웨어 버전               : "); UART1_PutDec(1, (u16)(fw_version*0.01F)); UART1_PutChar('.'); UART1_PutDec(1, (u16)(fw_version*0.1F)); UART1_PutChar('.'); UART1_PutDec(1, (u16)fw_version);
		UART1_PutStr("\r\n 마지막 업데이트 일자      : "); UART1_PutDec(2, (u16)last_update_year); UART1_PutChar('.'); UART1_PutDec(2, (u16)last_update_month); UART1_PutChar('.'); UART1_PutDec(2, (u16)last_update_day);	
		UART1_PutStr("\r\n PLC_1CH 모뎀 펌웨어 버전  : "); UART1_PutDec(1, (u16)SECC_Msg[1].SECC_Status_1.seccVersionMajor); UART1_PutChar('.'); UART1_PutDec(1, (u16)SECC_Msg[1].SECC_Status_1.seccVersionMinor); UART1_PutChar('.'); UART1_PutDec(1, (u16)SECC_Msg[1].SECC_Status_1.seccVersionPatch);
		UART1_PutStr("\r\n PLC_2CH 모뎀 펌웨어 버전  : "); UART1_PutDec(1, (u16)SECC_Msg[2].SECC_Status_1.seccVersionMajor); UART1_PutChar('.'); UART1_PutDec(1, (u16)SECC_Msg[2].SECC_Status_1.seccVersionMinor); UART1_PutChar('.'); UART1_PutDec(1, (u16)SECC_Msg[2].SECC_Status_1.seccVersionPatch);
		UART1_PutStr(" (정보가 안 나오면 모뎀과 통신이 안 된 것입니다. 모뎀 부팅시간 확보 후 다시 확인하세요.)");
		UART1_PutStr("\r\n 파워 모듈                 : 윈라인(WINLINE");
		
		//UI로부터 받은 각종 파라메타 출력, 채널정보, 채널수 용량, 전류설정값, 등등
		
		UART1_PutStr("\r\n 시스템 전체 동작 시간     : "); UART1_PutDec(3, (u16)g_day); UART1_PutStr(" [Day] ");UART1_PutDec(2, (u16)g_hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)g_minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)g_second);
	}
	else if(system_cmd == charging_status) Send_Uart_Terminal_default();
	else if(system_cmd == wrong_cmd) 
	{
		UART1_PutStr("\r\n\n [터미널 명령어 정보]");
		UART1_PutStr("\r\n  test   + Enter : 자가진단 테스트 시작 ");
		UART1_PutStr("\r\n  cinfo  + Enter : charging_info_start_cmd");
		UART1_PutStr("\r\n  stop   + Enter : charging_info_stop_cmd");
		UART1_PutStr("\r\n  reset  + Enter : system  reset ");
		UART1_PutStr("\r\n  update + Enter : f/w update mode(x-modem 1k), 반드시 UI 종료 후 실행할 것 ");
		UART1_PutStr("\r\n  version+ Enter : f/w ver, plc ver, power module info, device id info ");
		UART1_PutStr("\r\n  status + Enter : charging_status");
		UART1_PutStr("\r\n  his    + Enter : f/w ver, plc ver, power module info, device id info \r\n");
		UART1_PutStr("\r\n 잘못된 명령입니다. 명령을 취소합니다. \r\n\r\n"); system_cmd = none;
	}  
	else if(system_cmd == history_cmd) {system_cmd = none; history_flag = 1;}
	else                                                                                      
	{
		system_cmd = none;
	}
	
	
}



uint32_t UTF8_Type_Check(uint8_t num)
{
	uint32_t UTF8toUnicode = 0;
	uint8_t UTF8_byte = UTF8_Byte_table[buff[num]]; //Buffer에 앞자리만 확인을 하여 UTF-8이 몇 Byte인지 확인을 해주고, Switch안에서는 Byte의 자리수에 따라 유니코드로 변환을 해줍니다.
	
	switch (UTF8_byte) {
		case 4:    
		UTF8toUnicode += (buff[num++] & 0x07); UTF8toUnicode <<= 6;
		UTF8toUnicode += (buff[num++] & 0x3f); UTF8toUnicode <<= 6;
		UTF8toUnicode += (buff[num++] & 0x3f); UTF8toUnicode <<= 6;
		UTF8toUnicode += (buff[num] & 0x3f);
		UTF8toUnicode = 4;
		break;
		
		case 3:    
		UTF8toUnicode += (buff[num++] & 0x0f);     UTF8toUnicode <<= 6;
		UTF8toUnicode += (buff[num++] & 0x3f);     UTF8toUnicode <<= 6;
		UTF8toUnicode += (buff[num] & 0x3f);
		UTF8_Byte_val = 3;
		break;
		
		case 2:    
		UTF8toUnicode += (buff[num++] & 0x1f); UTF8toUnicode <<= 6;
		UTF8toUnicode += (buff[num] & 0x3f);
		UTF8_Byte_val = 2;
		break;
		
		case 1:    
		UTF8toUnicode += buff[num];
		UTF8_Byte_val = 1;
		break;
	}
	
	return UTF8toUnicode;
}

UINT8 rand_num_make_count;

void make_random_number(void)
{
	uint8_t rand_number;
	
	rand_number = rand() % 7 + 1;
	if(rand_num_make_count++ > 7) rand_num_make_count = 1;
	if     (rand_num_make_count == 1) evsis_text1[5] = 0x30 + rand_number;
	else if(rand_num_make_count == 2) evsis_text2[5] = 0x30 + rand_number;
	else if(rand_num_make_count == 3) evsis_text3[5] = 0x30 + rand_number;
	else if(rand_num_make_count == 4) evsis_text4[5] = 0x30 + rand_number;
	else if(rand_num_make_count == 5) evsis_text5[5] = 0x30 + rand_number;
	else if(rand_num_make_count == 6) evsis_text6[5] = 0x30 + rand_number;
	else if(rand_num_make_count == 7) evsis_text7[5] = 0x30 + rand_number;
}

void total_operating_time(void)
{
	if(g_elapsed_time++ >= 864000) // 1day = 24hour = 1440min = 86400sec, 100ms timer 
	{
		g_day++;
		g_elapsed_time = 0; 
	}
	g_elapsed_time_s = g_elapsed_time * 0.1F; // sec timer
	
	g_minute = (uint16_t)(g_elapsed_time_s * 0.0166666667);
	g_hour   = (uint16_t)(g_minute * 0.0166666667);
	g_minute = (uint16_t)(g_minute % 60);
	g_second = (uint16_t)((u16)g_elapsed_time_s % 60);
	
}

uint8_t test_timer, test_timer1, soc_blink;
UINT16 targetvoltage, current_demand;
void charging_infomation (void) //log1, 50ms
{
	if     (SECC_Msg[1].SECC_Status_1.seccStatusCode == Idle_Wait                           ) UART1_PutStr(" Idle_Wait              ");                              
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Idle                                ) UART1_PutStr(" Idle                   ");                           
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Ready                               ) UART1_PutStr(" Ready                  "); 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Low_Level_Comm                      ) UART1_PutStr(" Low_Level_Comm         ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == High_Level_Comm                     ) UART1_PutStr(" High_Level_Comm        "); 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Authorization_ContractAuthentication) UART1_PutStr(" Authorization          ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == ChargeParameterDiscovery            ) UART1_PutStr(" ParameterDiscovery     ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == CableCheck                          ) UART1_PutStr(" CableCheck             ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == PreCharge                           ) UART1_PutStr(" PreCharge              ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Charging                            ) UART1_PutStr(" Charging               ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Renegotiation                       ) UART1_PutStr(" Renegotiation          ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == EV_Init_Stop_Charging               ) UART1_PutStr(" EV_Init_Stop_Charging  ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == EVSE_Init_Stop_Charging             ) UART1_PutStr(" EVSE_Init_Stop_Charging"); 
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Terminate                           ) UART1_PutStr(" Terminate              ");    
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == CertificateInstallation             ) UART1_PutStr(" CertificateInstallation");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == CertificatedUpdate                  ) UART1_PutStr(" CertificatedUpdate     ");
	else if(SECC_Msg[1].SECC_Status_1.seccStatusCode == SECC_ERROR                          ) UART1_PutStr(" SECC_ERROR             ");
	//	UART1_PutStr(" "); // 구분
	if     (SECC_Msg[1].SECC_Status_1.seccErrorCode == SECC_NO_ERROR                          ) UART1_PutStr(" SECC_NO_ERROR                  ");
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED                                 ) UART1_PutStr(" FAILED                         ");
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_SequenceError                   ) UART1_PutStr(" SequenceError                  ");
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ServiceIDInvalid                ) UART1_PutStr(" ServiceIDInvalid               ");
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_UnknownSession                  ) UART1_PutStr(" UnknownSession                 ");  
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ServiceSelectionInvalid         ) UART1_PutStr(" ServiceSelectionInvalid        "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_PaymentSelectionInvalid         ) UART1_PutStr(" PaymentSelectionInvalid        ");  
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CertificateExpired              ) UART1_PutStr(" CertificateExpired             "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_SignatureError                  ) UART1_PutStr(" SignatureError                 "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_NoCertificateAvailable          ) UART1_PutStr(" NoCertificateAvailable         "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CerChainError                   ) UART1_PutStr(" CerChainError                  "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ChallengeInvalid                ) UART1_PutStr(" ChallengeInvalid               "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ContractCanceled                ) UART1_PutStr(" ContractCanceled               "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_WrongChargeParameter            ) UART1_PutStr(" WrongChargeParameter           "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_PowerDeliveryNotApplied         ) UART1_PutStr(" PowerDeliveryNotApplied        "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_TariffSelectionInvalid          ) UART1_PutStr(" TariffSelectionInvalid         "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ChargingProfileInvalid          ) UART1_PutStr(" ChargingProfileInvalid         "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_MeteringSignatureNotValid       ) UART1_PutStr(" MeteringSignatureNotValid      "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_NoChargeServiceSelected         ) UART1_PutStr(" NoChargeServiceSelected        "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_WrongEnergyTransferMode         ) UART1_PutStr(" WrongEnergyTransferMode        "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ContactorError                  ) UART1_PutStr(" ContactorError                 "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CertificateNotAllowedAtThisEVSE ) UART1_PutStr(" CertificateNotAllowedAtThisEVSE"); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_CertificateRevoked              ) UART1_PutStr(" CertificateRevoked             "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_NoNegotiation                   ) UART1_PutStr(" NoNegotiation                  "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_CommunicationSetup             ) UART1_PutStr(" TIMEOUT_CommunicationSetup     "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_Sequence                       ) UART1_PutStr(" TIMEOUT_Sequence               "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_NotificationMaxDelay           ) UART1_PutStr(" TIMEOUT_NotificationMaxDelay   "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == TIMEOUT_WeldingDetection               ) UART1_PutStr(" TIMEOUT_WeldingDetection       "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_WrongCPLevel                    ) UART1_PutStr(" FAILED_WrongCPLevel            "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_ProximityError                  ) UART1_PutStr(" FAILED_ProximityError          "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_HLCError                        ) UART1_PutStr(" FAILED_HLCError                "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_HeartbeatError                  ) UART1_PutStr(" FAILED_HeartbeatError          "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAILED_EVSECANInit                     ) UART1_PutStr(" FAILED_EVSECANInit             "); 
	else if(SECC_Msg[1].SECC_Status_1.seccErrorCode == FAULT_HPGPLinkDown                     ) UART1_PutStr(" FAULT_HPGPLinkDown             "); 
	//    UART1_PutStr(" "); // 구분
	if     (SECC_Msg[1].SECC_Status_1.evErrorCode == EV_NO_ERROR                          ) UART1_PutStr(" EV_NO_ERROR                  "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_RESSTemperatureInhibit        ) UART1_PutStr(" RESSTemperatureInhibit       "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_EVShiftPosition               ) UART1_PutStr(" EVShiftPosition              "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargerConnectorLockFault     ) UART1_PutStr(" ConnectorLockFault           "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_EVRESSMalfunction             ) UART1_PutStr(" EVRESSMalfunction            "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargingCurrentdifferential   ) UART1_PutStr(" ChargingCurrentdifferential  "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargingVoltageOutOfRange     ) UART1_PutStr(" ChargingVoltageOutOfRange    "); 
	else if(SECC_Msg[1].SECC_Status_1.evErrorCode == FAILED_ChargingSystemIncompatibility ) UART1_PutStr(" ChargingSystemIncompatibility"); 	
	
	UART1_PutStr(" CP:");
	if(SECC_Msg[1].SECC_Status_2.cpVoltage_12 < 0) {UART1_PutStr("-");UART1_PutDec(3, ~(SECC_Msg[1].SECC_Status_2.cpVoltage_12-1));} // short형 
	else                                        {UART1_PutStr(" ");UART1_PutDec(3, (u16)SECC_Msg[1].SECC_Status_2.cpVoltage_12);}
	
	if     (SECC_Msg[1].SECC_Status_2.CP_Status_0.bit.cpOscillator == TRUE) {UART1_PutStr(" PWM:");  UART1_PutStr("  ON"); }
	else if(SECC_Msg[1].SECC_Status_2.CP_Status_0.bit.cpOscillator == FALSE){UART1_PutStr(" PWM:");  UART1_PutStr(" OFF"); }
	
	UART1_PutStr("  80%까지 남은시간: "); UART1_PutDec(3, (u16)(SECC_Msg[1].SECC_EV_SoC_Related_Parameters.remainingTimeToBulkSOC_67 * 0.166666666F)); UART1_PutStr("분"); // 1/6 = 0.166666666F
	UART1_PutStr(" 100%까지 남은시간: "); UART1_PutDec(3, (u16)(SECC_Msg[1].SECC_EV_SoC_Related_Parameters.remainingTimeToFullSOC_45 * 0.166666666F)); UART1_PutStr("분"); // ex) 198 -> 198/6 = 33min	
	
	targetvoltage  = (UINT16)((float)SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetVoltage_01 * 0.1F);
	current_demand = (UINT16)((float)SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetCurrent_23 * 0.1F);
	
	UART1_PutStr(" 목표전압: "); UART1_PutDec(3, targetvoltage);  UART1_PutStr(" 현재전압: "); UART1_PutDec(3, (u16)Vout_G1);
	UART1_PutStr(" 요청전류: "); UART1_PutDec(3, current_demand); UART1_PutStr(" 현재전류: "); UART1_PutDec(3, (u16)Iout_Module_G1);
	
	
	UART1_PutStr(" SOC: "); UART1_PutDec(3, (u16)SOC_1CH); UART1_PutStr("[%] ");	
	
	
	UART1_PutStr("\r\n ");
}


void charging_infomation2 (void)
{
	UART1_PutStr(" CP:");
	if(SECC_Msg[1].SECC_Status_2.cpVoltage_12 < 0) {UART1_PutStr("-");UART1_PutDec(3, ~(SECC_Msg[1].SECC_Status_2.cpVoltage_12-1));} // short형 
	else                                        {UART1_PutStr(" ");UART1_PutDec(3, (u16)SECC_Msg[1].SECC_Status_2.cpVoltage_12);}
	
	if     (SECC_Msg[1].SECC_Status_2.CP_Status_0.bit.cpOscillator == TRUE) {UART1_PutStr(" PWM:");  UART1_PutStr("  ON"); }
	else if(SECC_Msg[1].SECC_Status_2.CP_Status_0.bit.cpOscillator == FALSE){UART1_PutStr(" PWM:");  UART1_PutStr(" OFF"); }
	
	UART1_PutStr("  80%까지 남은시간: "); UART1_PutDec(3, (u16)(SECC_Msg[1].SECC_EV_SoC_Related_Parameters.remainingTimeToBulkSOC_67 * 0.166666666F)); UART1_PutStr("분"); // 1/6 = 0.166666666F
	UART1_PutStr(" 100%까지 남은시간: "); UART1_PutDec(3, (u16)(SECC_Msg[1].SECC_EV_SoC_Related_Parameters.remainingTimeToFullSOC_45 * 0.166666666F)); UART1_PutStr("분"); // ex) 198 -> 198/6 = 33min	
	
	targetvoltage  = (UINT16)((float)SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetVoltage_01 * 0.1F);
	current_demand = (UINT16)((float)SECC_Msg[1].SECC_EV_TargetVoltage_Current.targetCurrent_23 * 0.1F);
	
	UART1_PutStr(" 목표전압: "); UART1_PutDec(3, targetvoltage);  UART1_PutStr(" 현재전압: "); UART1_PutDec(3, (u16)Vout_G1);
	UART1_PutStr(" 요청전류: "); UART1_PutDec(3, current_demand); UART1_PutStr(" 현재전류: "); UART1_PutDec(3, (u16)Iout_Module_G1);
	
	UART1_PutStr("충전 전력 : ");  
	if     (Vout_G1 * Iout_Module_G1 * 0.001F >= 100) {                   UART1_PutDec(3, (u16)(Vout_G1 * Iout_Module_G1 * 0.001F));}
	else if(Vout_G1 * Iout_Module_G1 * 0.001F >=  10) {UART1_PutStr(" "); UART1_PutDec(2, (u16)(Vout_G1 * Iout_Module_G1 * 0.001F));}
	else if(Vout_G1 * Iout_Module_G1 * 0.001F >=   0) {UART1_PutStr("  ");UART1_PutDec(1, (u16)(Vout_G1 * Iout_Module_G1 * 0.001F));}
	UART1_PutStr(" [kW], ");
	
	UART1_PutStr("충전 전력량 : ");
	if     (dc_electric_energy >= 100) {                    UART1_PutDec(3, (u16)(dc_electric_energy));}
	else if(dc_electric_energy >=  10) {UART1_PutStr(" ");  UART1_PutDec(2, (u16)(dc_electric_energy));}
	else if(dc_electric_energy >=   0) {UART1_PutStr("  "); UART1_PutDec(1, (u16)(dc_electric_energy));}
	UART1_PutChar('.'); 
	UART1_PutDec(1, (u16)(dc_electric_energy * 100 * 0.1F));
	UART1_PutDec(1, (u16)(dc_electric_energy * 100));
	UART1_PutStr(" [kWh] ");
	
	if(SECC_Msg[1].SECC_Status_1.seccStatusCode == Charging)
	{
		elapsed_time++; // 100ms timer
		elapsed_time_s = elapsed_time * 0.1F; // sec timer
		
		minute = (uint16_t)(elapsed_time_s * 0.0166666667);
		hour   = (uint16_t)(minute * 0.0166666667);
		minute = (uint16_t)(minute % 60);
		second = (uint16_t)((u16)elapsed_time_s % 60);
	}
	UART1_PutStr("충전 시간 : "); UART1_PutDec(1, (u16)hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)second); UART1_PutStr("."); UART1_PutDec(1, elapsed_time);
	
	UART1_PutStr(" 시작 배터리 : "); UART1_PutDec(2, start_SOC); UART1_PutStr("[%], ");
	UART1_PutStr(" 현재 배터리 : "); UART1_PutDec(3, (u16)SOC_1CH); UART1_PutStr("[%] ");	
	
	if(test_timer1++ >= 2) 	{test_timer1 = 0; soc_blink ^= 1;}
	if(test_timer++ >= 5){SOC_1CH++; test_timer = 0;}
	if(SOC_1CH >= 110) SOC_1CH = 0;
	UART1_PutStr("[");
	if     (SOC_1CH >   0 && SOC_1CH <  10) {UART1_PutStr("\033[30;41m");if(soc_blink){UART1_PutStr("□");UART1_PutStr("\033[0m"); UART1_PutStr("                  ");}else {UART1_PutStr("\033[0m");UART1_PutStr("                    "); }}
	else if(SOC_1CH >= 10 && SOC_1CH <  20) {UART1_PutStr("\033[30;41m");if(soc_blink){UART1_PutStr("□□");UART1_PutStr("\033[0m"); UART1_PutStr("                ");}else {UART1_PutStr("\033[30;41m");UART1_PutStr("□");UART1_PutStr("                  ");UART1_PutStr("\033[0m"); }}
	else if(SOC_1CH >= 20 && SOC_1CH <  30) {UART1_PutStr("\033[30;43m");if(soc_blink){UART1_PutStr("□□□");UART1_PutStr("\033[0m"); UART1_PutStr("              ");}else {UART1_PutStr("□□");UART1_PutStr("\033[0m");UART1_PutStr("                "); }}
	else if(SOC_1CH >= 30 && SOC_1CH <  40) {UART1_PutStr("\033[30;43m");if(soc_blink){UART1_PutStr("□□□□");UART1_PutStr("\033[0m"); UART1_PutStr("            ");}else {UART1_PutStr("□□□");UART1_PutStr("\033[0m");UART1_PutStr("              "); }}
	else if(SOC_1CH >= 40 && SOC_1CH <  50) {UART1_PutStr("\033[30;43m");if(soc_blink){UART1_PutStr("□□□□□");UART1_PutStr("\033[0m"); UART1_PutStr("          ");}else {UART1_PutStr("□□□□");UART1_PutStr("\033[0m");UART1_PutStr("            "); }}
	else if(SOC_1CH >= 50 && SOC_1CH <  60) {UART1_PutStr("\033[30;43m");if(soc_blink){UART1_PutStr("□□□□□□");UART1_PutStr("\033[0m"); UART1_PutStr("        ");}else {UART1_PutStr("□□□□□");UART1_PutStr("\033[0m");UART1_PutStr("          "); }}
	else if(SOC_1CH >= 60 && SOC_1CH <  70) {UART1_PutStr("\033[30;43m");if(soc_blink){UART1_PutStr("□□□□□□□");UART1_PutStr("\033[0m"); UART1_PutStr("      ");}else {UART1_PutStr("□□□□□□");UART1_PutStr("\033[0m");UART1_PutStr("        "); }}
	else if(SOC_1CH >= 70 && SOC_1CH <  80) {UART1_PutStr("\033[30;42m");if(soc_blink){UART1_PutStr("□□□□□□□□");UART1_PutStr("\033[0m"); UART1_PutStr("    ");}else {UART1_PutStr("□□□□□□□");UART1_PutStr("\033[0m");UART1_PutStr("      "); }}
	else if(SOC_1CH >= 80 && SOC_1CH <  90) {UART1_PutStr("\033[30;42m");if(soc_blink){UART1_PutStr("□□□□□□□□□");UART1_PutStr("\033[0m"); UART1_PutStr("  ");}else {UART1_PutStr("□□□□□□□□");UART1_PutStr("\033[0m");UART1_PutStr("    "); }}
	else if(SOC_1CH >= 90 && SOC_1CH < 100) {UART1_PutStr("\033[30;42m");if(soc_blink){UART1_PutStr("□□□□□□□□□□");UART1_PutStr("\033[0m"); UART1_PutStr("");}else {UART1_PutStr("□□□□□□□□□");UART1_PutStr("\033[0m");UART1_PutStr("  "); }}
	else if(SOC_1CH >= 100)             {UART1_PutStr("\033[30;42m");              UART1_PutStr("□□□□□□□□□□"); }
	
	//	\033[30;41m빨강 -> \033[30;42m초록 -> \033[30;44m파랑 -> \033[30;43m노랑
	
	UART1_PutStr("\033[0m");
	UART1_PutStr("]");
	
	UART1_PutStr("\r ");	
	
}

static UINT8 i = 0;

void fault_history_function (void)
{
	if(i == 0) 
	{
		UART1_PutStr(T_BrGREEN); 
		UART1_PutStr("현재 시간:"); UART1_PutDec(3, (u16)g_day); UART1_PutStr("[일] ");UART1_PutDec(2, (u16)g_hour); UART1_PutStr(":"); UART1_PutDec(2, (u16)g_minute); UART1_PutStr(":"); UART1_PutDec(2, (u16)g_second); UART1_PutStr("  충전 중 CP 에러(충전 중단) 이력 조회  "); UART1_PutStr(T_RESET);UART1_PutStr(" \r\n\n");
		UART1_PutStr("발생 횟수|\t발생 시간\t|\tEVCC MAC ADDRESS\t|CP\t|PWM   |시작  |종료  |타겟전압|출력전압|요청전류|현재전류|충전전력량");UART1_PutStr(" \r\n");
	}
	
	if((cp_error_chager_status[i].occur_day != 0) || (cp_error_chager_status[i].occur_hour != 0) || (cp_error_chager_status[i].occur_min != 0) || (cp_error_chager_status[i].occur_sec != 0))
	{
		if((fault_memory_cnt-1) == i)
		{
			UART1_PutStr(T_BG_BrRED);
			UART1_PutDec(3, i);
			
			UART1_PutStr("\t");
			before_day  = g_day    - cp_error_chager_status[i].occur_day;
			before_hour = g_hour   - cp_error_chager_status[i].occur_hour;
			before_min  = g_minute - cp_error_chager_status[i].occur_min;
			before_sec  = g_second - cp_error_chager_status[i].occur_sec;
			UART1_PutDec(3, before_day); UART1_PutStr("[일] "); UART1_PutDec(2, (u16)before_hour);UART1_PutStr(":");UART1_PutDec(2, (u16)before_min);UART1_PutStr(":");UART1_PutDec(2, (u16)before_sec);
			UART1_PutStr("전 발생\t");
			UART1_PutStr("\t"); UART1_PutDec(2, (u16)cp_error_chager_status[i].evccmac0); UART1_PutStr("-"); UART1_PutDec(2, (u16)cp_error_chager_status[i].evccmac1); UART1_PutStr("-"); UART1_PutDec(2, (u16)cp_error_chager_status[i].evccmac2); UART1_PutStr("-"); UART1_PutDec(2, (u16)cp_error_chager_status[i].evccmac3); UART1_PutStr("-"); UART1_PutDec(2, (u16)cp_error_chager_status[i].evccmac4); UART1_PutStr("-"); UART1_PutDec(2, (u16)cp_error_chager_status[i].evccmac5);
			UART1_PutStr("\t"); 
			if(cp_error_chager_status[i].cpVoltage < 0) {UART1_PutStr("-");UART1_PutDec(3, ~(cp_error_chager_status[i].cpVoltage-1));} // short형 
			else                                        {UART1_PutStr(" ");UART1_PutDec(3, (u16)cp_error_chager_status[i].cpVoltage);}
			UART1_PutStr("\t"); if     (cp_error_chager_status[i].cpOscillator == TRUE) {UART1_PutStr(" ON"); } else if(cp_error_chager_status[i].cpOscillator == FALSE){UART1_PutStr("OFF"); }
			UART1_PutStr("\t");UART1_PutDec(3, cp_error_chager_status[i].started_soc); 
			UART1_PutStr("\t");UART1_PutDec(3, cp_error_chager_status[i].finished_soc); 
			UART1_PutStr("\t");UART1_PutDec(4, (u16)cp_error_chager_status[i].targetV); 
			UART1_PutStr("\t");UART1_PutDec(4, (u16)cp_error_chager_status[i].present_V);  //(uint16_t)(Vout_G1 * 10);
			UART1_PutStr("\t");UART1_PutDec(4, (u16)cp_error_chager_status[i].demendI); 
			UART1_PutStr("\t");UART1_PutDec(4, (u16)cp_error_chager_status[i].present_I); //(uint16_t)(Iout_Module * 10);
			UART1_PutStr("\t");
			if     (cp_error_chager_status[i].dc_electric_energy_cpy >= 100) {                    UART1_PutDec(3, (u16)(cp_error_chager_status[i].dc_electric_energy_cpy));}
			else if(cp_error_chager_status[i].dc_electric_energy_cpy >=  10) {UART1_PutStr(" ");  UART1_PutDec(2, (u16)(cp_error_chager_status[i].dc_electric_energy_cpy));}
			else if(cp_error_chager_status[i].dc_electric_energy_cpy >=   0) {UART1_PutStr("  "); UART1_PutDec(1, (u16)(cp_error_chager_status[i].dc_electric_energy_cpy));}
			UART1_PutChar('.'); 
			UART1_PutDec(1, (u16)(cp_error_chager_status[i].dc_electric_energy_cpy * 100 * 0.1F));
			UART1_PutDec(1, (u16)(cp_error_chager_status[i].dc_electric_energy_cpy * 100));//				UART1_PutStr(" [kWh] ");
			UART1_PutStr(" [가장 최근 발생 기록]");
			UART1_PutStr(T_RESET);
			UART1_PutStr(" \r\n");
		}
		else
		{
			UART1_PutDec(3, i);  
			
			UART1_PutStr("\t");
			before_day  = g_day    - cp_error_chager_status[i].occur_day;
			before_hour = g_hour   - cp_error_chager_status[i].occur_hour;
			before_min  = g_minute - cp_error_chager_status[i].occur_min;
			before_sec  = g_second - cp_error_chager_status[i].occur_sec;
			UART1_PutDec(3, before_day); UART1_PutStr("[일] "); UART1_PutDec(2, (u16)before_hour);UART1_PutStr(":");UART1_PutDec(2, (u16)before_min);UART1_PutStr(":");UART1_PutDec(2, (u16)before_sec);
			UART1_PutStr("전 발생\t");
			UART1_PutStr("\t"); UART1_PutDec(2, (u16)cp_error_chager_status[i].evccmac0); UART1_PutStr("-"); UART1_PutDec(2, (u16)cp_error_chager_status[i].evccmac1); UART1_PutStr("-"); UART1_PutDec(2, (u16)cp_error_chager_status[i].evccmac2); UART1_PutStr("-"); UART1_PutDec(2, (u16)cp_error_chager_status[i].evccmac3); UART1_PutStr("-"); UART1_PutDec(2, (u16)cp_error_chager_status[i].evccmac4); UART1_PutStr("-"); UART1_PutDec(2, (u16)cp_error_chager_status[i].evccmac5);
			UART1_PutStr("\t"); 
			if(cp_error_chager_status[i].cpVoltage < 0) {UART1_PutStr("-");UART1_PutDec(3, ~(cp_error_chager_status[i].cpVoltage-1));} // short형 
			else                                        {UART1_PutStr(" ");UART1_PutDec(3, (u16)cp_error_chager_status[i].cpVoltage);}
			UART1_PutStr("\t"); if     (cp_error_chager_status[i].cpOscillator == TRUE) {UART1_PutStr(" ON"); } else if(cp_error_chager_status[i].cpOscillator == FALSE){UART1_PutStr("OFF"); }
			UART1_PutStr("\t");UART1_PutDec(3, cp_error_chager_status[i].started_soc); 
			UART1_PutStr("\t");UART1_PutDec(3, cp_error_chager_status[i].finished_soc); 
			UART1_PutStr("\t");UART1_PutDec(4, (u16)cp_error_chager_status[i].targetV); 
			UART1_PutStr("\t");UART1_PutDec(4, (u16)cp_error_chager_status[i].present_V);  //(uint16_t)(Vout_G1 * 10);
			UART1_PutStr("\t");UART1_PutDec(4, (u16)cp_error_chager_status[i].demendI); 
			UART1_PutStr("\t");UART1_PutDec(4, (u16)cp_error_chager_status[i].present_I); //(uint16_t)(Iout_Module * 10);
			UART1_PutStr("\t");
			if     (cp_error_chager_status[i].dc_electric_energy_cpy >= 100) {                    UART1_PutDec(3, (u16)(cp_error_chager_status[i].dc_electric_energy_cpy));}
			else if(cp_error_chager_status[i].dc_electric_energy_cpy >=  10) {UART1_PutStr(" ");  UART1_PutDec(2, (u16)(cp_error_chager_status[i].dc_electric_energy_cpy));}
			else if(cp_error_chager_status[i].dc_electric_energy_cpy >=   0) {UART1_PutStr("  "); UART1_PutDec(1, (u16)(cp_error_chager_status[i].dc_electric_energy_cpy));}
			UART1_PutChar('.'); 
			UART1_PutDec(1, (u16)(cp_error_chager_status[i].dc_electric_energy_cpy * 100 * 0.1F));
			UART1_PutDec(1, (u16)(cp_error_chager_status[i].dc_electric_energy_cpy * 100));//				UART1_PutStr(" [kWh] ");
			
			if(fault_memory_cnt == i) UART1_PutStr(" [가장 오래된 발생 기록]");
			UART1_PutStr(" \r\n");
		}
		
		if(i++ >= 255) {i = 0; history_flag = 0;}
	}
	else {i = 0; history_flag = 0; UART1_PutStr("더 이상 CP 에러 발생 이력이 없습니다. \n\r");}	
}


void printMonth(int year, int month) //year와 month를 전달 받아 해당 월을 출력 -> title, body
{
	printMonthTitle(year, month);
	printMonthBody(year, month);
}

void printMonthTitle(int year, int month) //월과 년도를 출력
{
	UART1_PutStr("\r\n\n     ");
	
	UART1_PutDec(4, (u16)year);UART1_PutStr("년");
	printMonthName(month);     UART1_PutStr("월 ");
	
	if     (last_update_day >=  10) {                   UART1_PutDec(2, last_update_day);}
	else if(last_update_day >=   1) {UART1_PutStr(" "); UART1_PutDec(1, last_update_day);}
	UART1_PutStr("일");
	
	UART1_PutStr("\r\n-----------------------------");
	UART1_PutStr("\r\n  일  월  화  수  목  금  토     ");
	UART1_PutStr("\r\n-----------------------------");
	UART1_PutStr("\r\n");
}

void printMonthName(int month) //month를 전달 받아 해당 달의 영어 이름을 출력
{
	switch (month)
	{
		case 1: UART1_PutStr(" 1"); break;
		case 2: UART1_PutStr(" 2"); break;
		case 3: UART1_PutStr(" 3"); break;
		case 4: UART1_PutStr(" 4"); break;
		case 5: UART1_PutStr(" 5"); break;
		case 6: UART1_PutStr(" 6"); break;
		case 7: UART1_PutStr(" 7"); break;
		case 8: UART1_PutStr(" 8"); break;
		case 9: UART1_PutStr(" 9"); break;
		case 10: UART1_PutStr(" 10"); break;
		case 11: UART1_PutStr(" 11"); break;
		case 12: UART1_PutStr(" 12"); break;
	}
}

void printMonthBody(int year, int month) //year와 month를 전달 받아 요일과 일을 출력
{
	int startday = getStartDay(year, month);
	int numberofdaysinmonth = getNumberOfDaysInMonth(year, month);
	
	for (int i = 0; i < startday; i++) UART1_PutStr("    ");
	for (int i = 1; i <= numberofdaysinmonth; i++) 
	{
		if     (i >=  10) 
		{
			UART1_PutStr("  ");
			if(i == last_update_day) UART1_PutStr(T_BrRED);
			UART1_PutDec(2, i);      UART1_PutStr(T_RESET); UART1_PutStr(T_BOLD); UART1_PutStr(T_BrWHITE);
		}
		else if(i >=   0) 
		{
			UART1_PutStr("   "); 
			if(i == last_update_day) UART1_PutStr(T_BrRED);
			UART1_PutDec(1, i);      UART1_PutStr(T_RESET); UART1_PutStr(T_BOLD); UART1_PutStr(T_BrWHITE);
		}
		
		if ((i + startday) % 7 == 0) UART1_PutStr("\r\n");
	}
}

int getStartDay(int year, int month) // 해당 월의 시작하는 날이 몇 요일인지 찾는 함수
{
	int startday1800 = 3;
	int totalnumberofdays = getTotalNumberOfDays(year, month);
	return (startday1800 + totalnumberofdays) % 7;
}

int getTotalNumberOfDays(int year, int month) // 1800년 1월 1일 부터 며칠이 되는지 계산
{
	int total = 0;
	for (int i = 1800; i < year; i++) 
	{
		if (isLeapYear(i)) total += 366;
		else total += 365;
	}
	for (int i = 1; i < month; i++) total = total + getNumberOfDaysInMonth(year, i);
	return total;
}

int getNumberOfDaysInMonth(int year, int month) //year와 month를 전달 받아 해당월에 몇일이 있는지
{
	if      (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) return 31;
	else if (month == 4 || month == 6 || month == 9 || month == 11)                                            return 30;
	else if (month == 2) 
	{
		if (isLeapYear(year) == 1)                                                                             return 29;
		else                                                                                                   return 28;
	}
	else                                                                                                       return 0;
}

int isLeapYear(int year) //year를 전달받아 윤년인지 확인
{
	if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)) return 1;
	else return 0;
}
