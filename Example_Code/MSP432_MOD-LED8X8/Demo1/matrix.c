#include "main.h"

int First=0, Message_Length;

/*
Name:
  TimerA_Init
Input:
  Counter - Period of the interrupt.
  prescaler - value to be writen in prescaler of the clock
Output:
  nothing
Description:
  Initialize TimerA mode, Capture/compare register and prescaler.
*/
void TimerA_Init(unsigned int Counter, unsigned int prescaler)
{
  // configure TimerA mode, values, prescaler, interrupt modes
  int divisor;
  switch (prescaler)
  {
    case 1: divisor = 0x0000; break;
    case 2: divisor = 0x0040; break;
    case 4: divisor = 0x0080; break;
    case 8: divisor = 0x00C0; break;
    default: divisor = 0x0000; break;
  }
    
  
  TACTL = TACTL & ~ (MC0 | MC1);    // Stop Mode
  TACTL = TACTL | TACLR;            // TimerA Clear
  
  TACCTL0 = TACCTL0 | OUTMOD2 | CCIE; // Output Toggle Mode, Capture/Compare
  TACCR0 = Counter;                 // set Capture/Compare register value

  TACTL = TACTL | 0x0200 | divisor; // Select the Clock and Input Divider and Prescaler
  TACTL = TACTL | MC0;              // Mode: Up
  return;
}

/*
Name:
  USI_Init
Input:
  nothing
Output:
  nothing
Description:
  Configures the serial interface with the proper settings.
*/
void USI_Init (void)
{
  // configure SPI
  USICTL0 |= USISWRST;                      // USI in reset
  USICTL0 = USICTL0 | USILSB;               // Least Significant Bit first
  USICTL0 |= USIPE7 + USIPE6 + USIPE5 + USIMST + USIOE; // Port, SPI Master
  USICTL1 |= USICKPH;                       // flag remains set
  USICKCTL = USIDIV_1 + USISSEL_2;          // /2 SMCLK
  USICTL0 &= ~USISWRST;                     // USI released for operation
  USICNT = USICNT | 0x50;                   // 16 bit mode; 16 bit to be transmitted/received
  return;
}

int String_Length (const char Input[])
{
  int i;
  for (i=0; Input[i]; i++);
  return i;
}

/*
Name:
  LED_Matrix_Init
Input:
  nothing
Output:
  nothing
Description:
  Initialize the matrix pins and latch.
*/
void LED_Matrix_Init (void)
{
  // configure SPI pins and latch
  P1DIR |=  DATA_LATCH_PIN | SPI_DATA_OUT_PIN | SPI_CLK_OUT_PIN;
  
  // set latch pin to inactive
  P1OUT &= ~DATA_LATCH_PIN;

  USI_Init ();
  return;
}

/*
Name:
  Write_Matrix
Input:
  nothing
Output:
  nothing
Description:
  Using TimerA interrupt to display the picture column by column.
  The columns are selected by the <index> in USISRH 
  and using the FontLookup array select the respective bits to be ON.
*/
#pragma vector=TIMERA0_VECTOR
__interrupt void Write_Matrix(void)
{
  static int x, x_div, x_mod;
  static char index=0, offset, Column=1;
  const int COLUMN_COUNT = MATRIX_COUNT * 8, MESSAGE_LENGTH = Message_Length * 6;
  
  // Latch Pulse
  P1OUT = P1OUT ^ DATA_LATCH_PIN;
  P1OUT = P1OUT ^ DATA_LATCH_PIN;
  
  
  // Sending MATRIX_COUNT portions of data via SPI
  // to the differents matrices. They use one latch so
  // we have to send the respective data to each of them before we pulse it.
  // Offset is increased with 8 because we send data to a different matrix
  // Columns (for 3 matrices) are in that order 0, 8, 16, 1, 9, 17, 2, 10, 18 etc.
  for (offset=0; offset<COLUMN_COUNT; offset=offset+8)    
  {
    // calculate the number of the column (IN THE MESSAGE BUFFER) based on:
    // First - offset caused by the slided text
    // offset - offset caused by number of the different matrix
    // index - offset caused by number of the different columns in one matrix
    // MESSAGE_LENGTH - length of the message (in columns) to avoid displaying unexisting columns
    // (ex. we have 120 columns [0..119] and try do display 120 --> we will display the 0 column)
    x = (First + offset + index) % (MESSAGE_LENGTH);  
    x_div = x/6;
    x_mod = x%6;
    
    USISRH = Column;  // number of the currenly active column
    if (x_mod == 5)
      USISRL = 0x00;  // leaving a gap between the symbols
    else
      USISRL = FontLookup[TextMessage[x_div] - 0x20][x_mod];  // send data for actual rows to be turned ON
    USICNT = USICNT | 0x10;       // 16 bit format transmition
    while (USICTL1 & USIIFG);     // Waiting to finish the transmition
  }
  
  index++;    // Increase the number of the column for each of the matrices
  if (index & 7)  
    Column = Column<<1; // Activate next column
  else
  {
    // if index reach value 8 start over from 0, and the column from 1
    Column = 1;
    index = 0;
  }
  
  return;
}
