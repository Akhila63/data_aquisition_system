//UART.C

#include <LPC21xx.h>
#include "types.h"
#include "uart_defines.h"
#include "pin_connect_block.h"

#if (UART_INTS_ENABLE!=0)
void U0_ISR(void) __irq;
extern u32 flag;
#endif 

void Init_UART0(void)
{
	//cfg p0.0 as TXD0 pin
	CfgPortPinFunc(0,0,TXD0_PIN_0_0);
	//Cfg p0.1 as RXD0 pin
	CfgPortPinFunc(0,1,RXD0_PIN_0_1);
	//Cfg UART for 8N1,with Divisor Latch Enabled
	U0LCR=(1<<DLAB_BIT)|WORD_LEN;
	//Cfg for required Baudrate
	U0DLM=DIVISOR>>8;
	U0DLL=DIVISOR;
	//Clear/reset Divisor Latch
	U0LCR&=~(1<<DLAB_BIT);
	
	#if (UART_INTS_ENABLE!=0)
	IODIR0|=((1<<TXD0_STATUS_LED)|
	         (1<<RXD0_STATUS_LED));
	U0IER |=((1<<TX0_INT_EN_BIT)|
	        (1<<RX0_INT_EN_BIT));
	//VICIntSelect=0;
	VICIntEnable=1<<UART0_VIC_CHNO;
	//let uart0 isr be at priority 0
	VICVectCntl0=(1<<5)|UART0_VIC_CHNO;
	//load uart0 isr starting addr
	VICVectAddr0=(u32 )U0_ISR; 
	#endif
}

void U0_TxByte(u8 byte)
{
	//place byte into tx buffer 
	//& start transmission
	U0THR=byte;
	//wait until transmission completion status
	//while(((U0LSR>>TEMT_BIT)&1)==0);
	while(((U0LSR>>THRE_BIT)&1)==0);
}

u8  U0_RxByte(void)
{
	//wait until reception completion status
	while(((U0LSR>>DR_BIT)&1)==0);
	//read & return recvd byte
	return U0RBR;	
}
#if (UART_INTS_ENABLE!=0)
void U0_ISR(void) __irq
{
	u8 dummy;
	
	dummy=(U0IIR&0x0F);
	if(dummy==TXD0_STATUS)
	{	
	 flag=1;
	 IOPIN0^=1<<TXD0_STATUS_LED;
	}
	else if(dummy==RXD0_STATUS)
	{
		IOPIN0^=1<<RXD0_STATUS_LED;
		dummy=U0RBR;
	}
	VICVectAddr=0;
}
#endif

void U0_TxStr(s8 *s)
{
	while(*s)
		U0_TxByte(*s++);	
}

void U0_TxU32(u32 n)
{
	s32 i=0;
	u8 digit[10];
	if(n==0)
	{
		U0_TxByte('0');
	}
  else
	{
		while(n>0)
		{
			digit[i]=(n%10)+48;
			n/=10;
			i++;
		}
		for(--i;i>=0;i--)
		   U0_TxByte(digit[i]);
	}
}

void U0_TxS32(s32 n)
{
	if(n<0)
	{
		U0_TxByte('-');
		n=-n;
	}
  U0_TxU32(n);	
}

void U0_TxF32(f32 fNum,u8 nDP )
{
	s32 i;
	u32 n;
	if(fNum<0)
	{
		U0_TxByte('-');
		fNum=-fNum;
	}
	n=fNum;
	U0_TxU32(n);
	U0_TxByte('.');
	for(i=0;i<nDP;i++)
	{
		fNum=(fNum-n)*10;
		n=fNum;
		U0_TxByte(n+48);
	}
}

s8 * U0_ReadStr(void)
{
	static s8 rbuf[100];
	u32 i=0;
	while(1)
	{
		rbuf[i]=U0_RxByte();
		U0_TxByte(rbuf[i]);
		if((rbuf[i]=='\n')||(rbuf[i]=='\r'))
		{
			rbuf[i]='\0';
			break;
		}	
    i++;		
	}
	return rbuf;
}

