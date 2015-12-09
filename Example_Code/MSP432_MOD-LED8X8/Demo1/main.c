/**************************************************
* Demo software for Olimex Board MOD-LED_8X8.     *
* On the display will be shown a welcome message  *
* Press the button to slide it.                   *
* To change the text, slide direction, number     *
* of displays check the configuration macros      *
* in main.h file.                                 *
**************************************************/

#include "main.h"

const char TextMessage[] = TEXT;

// Simple delay loop with volatile variable to avoid optimizations
void Delay (volatile unsigned long int i)
{
  while (i--);
  return;
}

// Initialize the clock to maximize the input frequency
// See the datasheet "MSP430x2xx Family User Guide.pdf"
// pages 292 and 299 for more details
void Clock_Init (void)
{
  DCOCTL = DCOCTL | 0xE0;   // DCO bits are set to 0x7
  BCSCTL1 = BCSCTL1 | 0x0F; // RSEL bits are set to 0xF
  return;
}

int main (void)
{  
// ++++++++++++++++++ Initializations ++++++++++++++++++
  
  WDTCTL = WDTPW + WDTHOLD;     // Stop watchdog timer
  Clock_Init ();
  LED_Matrix_Init ();
  TimerA_Init (PERIOD, PRESCALER);
  BUTTON_INPUT;
  Message_Length = String_Length (TextMessage);
  
    __enable_interrupt();         // General Interrupt Enable
// ------------------ Initializations ------------------

  while(1)
  {
    if (IS_BUTTON_DOWN)
    {
#if   SLIDE_DIRECTION == LEFT
      // Increasing the first displayed column number and eventually start from zero if the end is reached
      First = (First + 1) % (Message_Length * 6);
#elif SLIDE_DIRECTION == RIGHT
      // Decreasing the first displayed column number and eventually starts from the last if the zero column is reached
      First--;
      if (First == -1)
        First = Message_Length * 6 - 1;
#else
#warning  No sliding! Check SLIDE_DIRECTION macro in "main.h".
#endif
      Delay (DELAY_TIME);       // Simple delay
    }
  }
}
