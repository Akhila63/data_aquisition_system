//kpm_3.c
#include "types.h"
#include <LPC21xx.h>
#include "kpm_defines.h"
//#include "defines.h"

u8 kpmLUT[4][4]=
{
	{7,8,9,'/'},{4,5,6,'X'},{1,2,3,'-'},{'c',0,'=','+'}
};
void InitKPM(void)
{
	//ground all rows
	WRITENIBBLE(IODIR1,ROW0,15);
	//default all cols are high any way
}
u32 ColScan(void)
{
	return(READNIBBLE(IOPIN1,COL0)==15);
}
u32 RowCheck(void)
{
	u32 rNo;
	for(rNo=0;rNo<4;rNo++)
	{
		//ground row 0 to 3 iteratively,
		WRITENIBBLE(IOPIN1,ROW0,~(1<<rNo));
		//now check if key was pressed in this row
		if(!ColScan())
		{
			break;
		}
  }	
	//ground all rows
	WRITENIBBLE(IOPIN1,ROW0,0);
	return rNo;
}

u32 ColCheck(void)
{
	u32 cNo;
	for(cNo=0;cNo<4;cNo++)
	{
		if(READBIT(IOPIN1,(COL0+cNo))==0)
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
	
	return keyV;
}

u32 ReadNum(void)
{
	static u32 sum=0,keyV;
	while(1)
	{
		keyV=KeyScan();
		if(keyV<10)
		{
			sum=(sum*10)+keyV;
			while(ColScan()==0);
    }
    else
     break;			
	}
	return sum;
}
