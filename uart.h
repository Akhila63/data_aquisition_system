//uart.h

#include "types.h"
void Init_UART0(void);
void U0_TxByte(u8 byte);
u8   U0_RxByte(void);

void U0_TxStr(s8 *);
void U0_TxU32(u32 );
void U0_TxS32(s32 );
void U0_TxF32(f32,u8);
s8 * U0_ReadStr(void);
