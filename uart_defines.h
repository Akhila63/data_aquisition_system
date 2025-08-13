//uart_defines.h
#define FOSC 12000000
#define CCLK (FOSC*5)
#define PCLK (CCLK/4)

#define BAUD  9600
#define DIVISOR (PCLK/(16 * BAUD))

#define PIN_FUN2 1
#define TXD0_PIN_0_0 PIN_FUN2
#define RXD0_PIN_0_1 PIN_FUN2

//UxLCR defines
#define _8BIT     3
#define WORD_LEN  _8BIT
#define DLAB_BIT  7

//UxLSR defines
#define DR_BIT    0
#define TEMT_BIT  6
#define THRE_BIT  5

#define UART_INTS_ENABLE 0

//UxIER defines
#define RX0_INT_EN_BIT 0
#define TX0_INT_EN_BIT 1

//UxIIR defines
#define TXD0_STATUS    2
#define RXD0_STATUS    4

#define TXD0_STATUS_LED 5
#define RXD0_STATUS_LED 6

#define UART0_VIC_CHNO  6
