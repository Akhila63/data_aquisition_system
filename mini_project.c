//MINI_PROJECT.C
#include <LPC21xx.h>
#include "types.h"
#include "delay.h"
#include "pin_connect_block.h"
#include "lcd_defines.h"
#include "lcd.h"
#include "adc.h"
#include "rtc.h"
#include "uart.h"
#include "kpm.h"
#define BUZZ_LED 24

#define PIN_FUNC2 1
#define EINT0_INPUT_PIN PIN_FUNC2 
#define EINT0_PIN 16//p0.16
#define EINT0_VIC_CHNO 14

f32 SetPoint= 45;
u32 edit_flag =0;

s32 hour,min,sec,date,month,year,day;
void setTime(void);
void display_LCD(f32 eAR);
void display_UART(f32 eAR);

void eint0_isr(void) __irq;
void edit_mode(void);
void EditTime(void);
	
int main()
{
	//s32 hour,min,sec,date,month,year,day;
	f32 eAR;
  u32 adcDVal;
	Init_ADC();
	RTC_Init();
	Init_UART0();
	InitLCD();
	InitKPM();
	IODIR1 |= 1<<BUZZ_LED;
	//StrLCD("WELCOME");
	//delay_ms(300);
	CmdLCD(0x01);
	//setTime();
	SetRTCTimeInfo(2,30,59);
	SetRTCDateInfo(21,5,2025);
	SetRTCDay(4);
	
	CfgPortPinFunc(0,16,EINT0_INPUT_PIN);
	
  //cfg VIC for EINT0 as irq types
  //VICIntSelect=0; //def all interrups are irq
  //cfg VIC for enable EINT0 interrupt
  VICIntEnable = 1<<EINT0_VIC_CHNO;
  //cfg VIC for EINT0 as v.irq highest priority(0)
  VICVectCntl0=(1<<5)|EINT0_VIC_CHNO;
  //load eint0_isr address into v.irq reg
  VICVectAddr0=(u32)eint0_isr;
	
	//cfg External Interrupt Peripheral
	//enable EINT0 
	//EXTINT =0;  //default enabled
	//cfg for EINT0 for edge triggering
	EXTMODE = 0<<0;
	//cfg EINT0 for falling edge
	EXTPOLAR= 0X00; //default
	
	while(1)
	{
		if(edit_flag == 1)
		{
			edit_mode();
			edit_flag =0;
		}
		Read_ADC(1,&adcDVal,&eAR);
		
		GetRTCTimeInfo(&hour,&min,&sec);
		GetRTCDay(&day);
		GetRTCDateInfo(&date,&month,&year);
		GetRTCDay(&day);
		
		display_LCD(eAR);
		display_UART(eAR);
		
		delay_ms(1000);
	}
}

void display_LCD(f32 eAR)
{
		static int buzz = 0;
		if((eAR*100) >= SetPoint)
		{
			buzz = 1;
			IOSET1 = (1<<BUZZ_LED);
			CmdLCD(CLEAR_LCD);
			StrLCD("OVER TEMP!..");
			return;
		}
		if(buzz == 1)
		{
			IOCLR1 = (1<<BUZZ_LED);
			buzz = 0;
		}
		CmdLCD(CLEAR_LCD);
		DisplayRTCTime(hour,min,sec);
	
		DisplayRTCDay(day);
		
		DisplayRTCDate(date,month,year);
		
		DisplayRTCDay(day);
		
		CmdLCD(GOTO_LINE2_POS0+10);
		StrLCD("T:");
		U32LCD(eAR*100);
		CharLCD(0xDF);
    StrLCD("C");
}
void display_UART(f32 eAR)
{
		U0_TxStr("temp:");
		U0_TxF32(eAR*100,1);
		U0_TxByte('*');
		U0_TxStr("C @ ");
		
		U0_TxByte((hour/10)+48);	//10's position value
		U0_TxByte((hour%10)+48);	//1's position value
		U0_TxByte(':');
		U0_TxByte((min/10)+48);	//10's position value
		U0_TxByte((min%10)+48);	//1's position value
		U0_TxByte(':');
		U0_TxByte((sec/10)+48);	//10's position value
		U0_TxByte((sec%10)+48);
		U0_TxByte(' ');
		
		
		U0_TxByte((date/10)+48);	//10's position value
		U0_TxByte((date%10)+48);	//1's position value
		U0_TxByte('/');
		U0_TxByte((month/10)+48);	//10's position value
		U0_TxByte((month%10)+48);	//1's position value
		U0_TxByte('/');
		U0_TxU32(year%100);
		if((eAR*100) >= SetPoint)
		{
			U0_TxStr("- OVER TEMP!");			
		}
		U0_TxStr("\r\n");
}
void eint0_isr(void) __irq
{
	u32 temp;
	temp	= EXTINT;
	if(temp&0x01)
	{
		edit_flag =1;
	}
	//clear EINT0 status flag in Ext Int Peripheral
	EXTINT=1<<0;
	//EXTINT = temp;
	//clear EINT0 status in VIC Peripheral
	VICVectAddr=0;
}

void edit_mode(void)
{
	//u32 ch;
	f32 setpt;
	while(1)
	{
		U0_TxStr("*** Time Editing Mode Activated ***\r\n");
		CmdLCD(CLEAR_LCD);
		CmdLCD(GOTO_LINE1_POS0);
		StrLCD("EDIT MODE ACTVTD");
		CmdLCD(GOTO_LINE2_POS0);
		StrLCD("1.T 2.SPt 3.Exit");
		U0_TxStr("1.TIME 2.SET POINT 3.Exit\r\n");
		//ch = KeyScan();
		switch(KeyScan())
		{
			case 1 : EditTime(); break;
			
			case 2 : delay_ms(200);CmdLCD(CLEAR_LCD);CmdLCD(GOTO_LINE1_POS0);
							 StrLCD("ENTER NEW SET Pt");U0_TxStr("ENTER NEW SET POINT: \r\n");
							 CmdLCD(GOTO_LINE2_POS0); setpt = ReadNum(); F32LCD(setpt,2); U0_TxU32(setpt);
							 delay_ms(500); SetPoint = setpt; U0_TxStr("\r\n");break;							
												
			case 3 : return;
			
			default : CmdLCD(CLEAR_LCD);CmdLCD(GOTO_LINE1_POS0);
							  StrLCD("Invalid Chioce..");U0_TxStr("Invalid Chioce..\r\n");
		}
	}
}
void EditTime(void)
{
	u32 ch;
	//s32 temp=0; 
	while(1)
	{
		CmdLCD(CLEAR_LCD);
		CmdLCD(GOTO_LINE1_POS0);
		StrLCD("1.H 2.M 3.S 4.D");
		U0_TxStr("1.HOURS 2.MINS 3.SECS 4.DATE\n\r");
		CmdLCD(GOTO_LINE2_POS0);
		StrLCD("5.M 6.Y 7.Dy 8.E");
		U0_TxStr("5.MONTHS 6.YEAR 7.DAy 8.EXIT \n\r");
		ch = KeyScan();
		switch(ch)
		{
			case 1 : CmdLCD(CLEAR_LCD); CmdLCD(GOTO_LINE1_POS0);U0_TxStr("ENTER HOURS :\n\r");
							 StrLCD("ENTER HOURS : "); HOUR = ReadNum();U0_TxStr("HOURS UPDATED...\n\r");
							 CmdLCD(GOTO_LINE2_POS0);StrLCD("HOURS UPDATED..."); 
							 break;
			
			case 2 : CmdLCD(CLEAR_LCD); CmdLCD(GOTO_LINE1_POS0);U0_TxStr("ENTER MINS: \n\r");
							 StrLCD("ENTER MINS : "); MIN = ReadNum();U0_TxStr("MINS UPDATED...\n\r");
							 CmdLCD(GOTO_LINE2_POS0); StrLCD("MINS UPDATED..."); 
							 break;
			
			case 3 : CmdLCD(CLEAR_LCD); CmdLCD(GOTO_LINE1_POS0);U0_TxStr("ENTER SECONDS :\n\r");
							 StrLCD("ENTER SECS : "); SEC = ReadNum();U0_TxStr("SECONDS UPDAETD...\n\r");
							 CmdLCD(GOTO_LINE2_POS0);StrLCD("SECS UPDATED..."); break;
			
			case 4 : CmdLCD(CLEAR_LCD); CmdLCD(GOTO_LINE1_POS0);U0_TxStr("ENTER DATE :\n\r");
							 StrLCD("ENTER DATE : "); DOM = ReadNum();U0_TxStr("DATE UPDATED..\n\r");
							 CmdLCD(GOTO_LINE2_POS0);StrLCD("DATE UPDATED..."); break;
			
			case 5 : CmdLCD(CLEAR_LCD); CmdLCD(GOTO_LINE1_POS0);U0_TxStr("ENTER MONTHS :\n\r");
							 StrLCD("ENTER MONTH : "); MONTH = ReadNum();U0_TxStr("MONTHS UPDATED...\n\r");
							 CmdLCD(GOTO_LINE2_POS0);StrLCD("MONTHS UPDATED.."); break;
			
			case 6 : CmdLCD(CLEAR_LCD); CmdLCD(GOTO_LINE1_POS0);U0_TxStr("ENTER YEAR :\n\r");
							 StrLCD("ENTER YEAR : "); YEAR = ReadNum();U0_TxStr("YREA UPDATED...\n\r");
							 CmdLCD(GOTO_LINE2_POS0);StrLCD("YEAR UPDATED..."); break;
							 
			case 7 : CmdLCD(CLEAR_LCD); CmdLCD(GOTO_LINE1_POS0);U0_TxStr("ENTER DAY :\n\r");
							 StrLCD("ENTER DAY : "); DOW = KeyScan();U0_TxStr("DAY UPDATED...\n\r");
							 CmdLCD(GOTO_LINE2_POS0);StrLCD("DAY UPDATED..."); break;
							 
			case 8 : return;
			
			default : CmdLCD(CLEAR_LCD);CmdLCD(GOTO_LINE1_POS0);
								StrLCD("Invalid Chioce..");U0_TxStr("Invalid Chioce..\r\n");
		}
	}
}   
