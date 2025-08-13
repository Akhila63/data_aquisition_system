//eint0_test.c
#include <LPC21xx.h>
#include "types.h"
#include "pin_connect_block.h"
#include "delay.h"

#define PIN_FUNC2 1
#define EINT0_INPUT_PIN PIN_FUNC2 
#define EINT0_PIN 16//p0.16
#define EINT0_VIC_CHNO 14
#define EINT0_STATUS_LED 15//p0.15

void eint0_isr(void) __irq;

u32 x;
int main()
{
	//cfg p0.16 as EINT0 input pin
	CfgPortPinFunc(0,16,EINT0_INPUT_PIN);
	
  //cfg VIC
  //cfg VIC for EINT0 as irq types
  //VICIntSelect=0; //def all interrups are irq
  //cfg VIC for enable EINT0 interrupt
  VICIntEnable=1<<EINT0_VIC_CHNO;
  //cfg VIC for EINT0 as v.irq highest priority(0)
  VICVectCntl0=(1<<5)|EINT0_VIC_CHNO;
  //load eint0_isr address into v.irq reg
  VICVectAddr0=(u32 )eint0_isr;
	
	//cfg External Interrupt Peripheral
	//enable EINT0 
	//EXTINT=0;  //default enabled
	//cfg for EINT0 for edge triggering
	EXTMODE=1<<0;
	//cfg EINT0 for falling edge
	//EXTPOLAR=0; //default
	
	//Cfg p0.15 as gpio out pin for LED
	IODIR0|=1<<EINT0_STATUS_LED;
	
	while(1)
	{
		x++;//some activity for main-line program
		    //until interrupt fired/raised
		delay_ms(1000);
	}	
}

void eint0_isr(void) __irq
{
	//toggle EINT0 status LED for ISR activity
	IOPIN0^=1<<EINT0_STATUS_LED;
	
	//clear EINT0 status flag in Ext Int Peripheral
	EXTINT=1<<0;
	//clear EINT0 status in VIC Peripheral
	VICVectAddr=0;
	
}
