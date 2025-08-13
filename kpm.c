#include "types.h"
#include <lpc21xx.h>
#include "kpm_defines.h"
const u32 kpmLUT[4][4]=
{
        {1,2,3,4},
        {5,6,7,8},
        {9,10,11,12},
        {13,14,'=',16}
};
//function to initialize Keypad matrix
void InitKPM()
{
//cfg row lines as gpio out
IODIR1|=15<<ROW0;
//default all col lines are anyways high
}
//function to scan the columns
u32 ColScan(void)
{
u32 t;
t=(((IOPIN1>>COL0)&15)<15)?0:1;
return t;
}
//function to check the row
u32 RowCheck(void)
{
u32 rNo;
for(rNo=0;rNo<=3;rNo++)
{
//ground very row at a time,starting with 0th row
IOPIN1=((IOPIN1&~(15<<ROW0))|((~(1<<rNo))<<ROW0));
if(ColScan()==0)
break;
}
IOCLR1=15<<ROW0;
return rNo;
}
//function to check the columns
u32 ColCheck(void)
{
u32 cNo;
for(cNo=0;cNo<=3;cNo++)
{
if(((IOPIN1>>(COL0+cNo))&1)==0)
    break;
}
return cNo;
}
//function to scan the key
u32 KeyScan(void)
{
u32 rNo,cNo,keyV;
//wait for any key press
        while(ColScan());
//proceed to identify row in which key is pressed
    rNo=RowCheck();
//proceed to identify col in which key is pressed
        cNo=ColCheck();
//extract key value from lookup table for pressed key
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
