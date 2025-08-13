//kpm_2.c
#include "types.h"
#include <LPC21xx.h>
#include "kpm_defines.h"
#include "lcd.h"
#include "delay.h"
#include "lcd_defines.h"
u8 kpmLUT[4][4]=
{
	//{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}
	//{7,8,9,'/'},{4,5,6,'X'},{1,2,3,'-'},{'\b',0,'\n','+'}
        {1,2,3,4},
        {5,6,7,8},
        {9,10,11,12},
        {13,14,'=',0}
};
void InitKPM(void)
{
	//ground all rows
	IODIR1|=15<<ROW0;
	//default all cols are high any way
}
u32 ColScan(void)
{
	return(((IOPIN1>>COL0)&15)==15);
}
u32 RowCheck(void)
{
	u32 rNo;
	for(rNo=0;rNo<4;rNo++)
	{
		//ground row 0 to 3 iteratively,
		IOPIN1=((IOPIN1|(15<<ROW0))^(1<<(ROW0+rNo)));
		//now check if key was pressed in this row
		if(!ColScan())
		{
			break;
		}
  }	
	//ground all rows
	IOCLR1=15<<ROW0;
	return rNo;
}

u32 ColCheck(void)
{
	u32 cNo;
	for(cNo=0;cNo<4;cNo++)
	{
		if(((IOPIN1>>(COL0+cNo))&1)==0)
			 break;
	}	
	return cNo;
}

u32 KeyScan()
{
	u32 keyV,rNo,cNo;
	//waiting for any key press
	while(ColScan());
	//if key is pressed
	//identifying row in which key is pressed
	rNo=RowCheck();
	//identifying col in which key is pressed
	cNo=ColCheck();
	//extract key value/label from LUT
	keyV=kpmLUT[rNo][cNo];
	//switch release condition to avoid multiple key press
	while(!ColScan());
	delay_ms(170);
	return keyV;
}
u32 ReadNum(void)
{
	u32 sum=0,key;
	while(1)
	{
		key=KeyScan();
		if(key <= 9)
		{
			sum=(sum*10)+key;
			while(ColScan()==0);
    }
    else
		{
			while(ColScan()==0);
			break;
		}
	}
	return sum;
}
