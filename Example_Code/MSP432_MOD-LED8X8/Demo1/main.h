#ifndef MAIN_H
#define MAIN_H

#include "msp430.h"
#include "font.h"
#include "matrix.h"


#define PRESCALER       1                         // prescaler of the TimerA
#define BUTTON_INPUT    P1DIR = P1DIR & (~0x08)   // Button set as input    1.3
#define IS_BUTTON_DOWN  (~P1IN & 0x08)            // check if the button is pressed   1.3
#define LEFT  1
#define RIGHT 0

extern const char TextMessage[];

void Delay (volatile unsigned long int i);

// Here you can configurate behavior of the board: number of matrices, displayed text, slide direction, delay and refresh period.

#define TEXT  "www.olimex.com/dev " // max 835 symbols

#define SLIDE_DIRECTION LEFT        // macro to define whether sliding direction (left or right).

#define MATRIX_COUNT  3             // Count of the connected matrix

#if   MATRIX_COUNT == 0
  #warning  Text can not be display without LED matrix!
  #define PERIOD        0xFFFF
  #define DELAY_TIME    0
#elif MATRIX_COUNT == 1
  #define PERIOD        0x4000
  #define DELAY_TIME    50000
#elif MATRIX_COUNT == 2
  #define PERIOD        0x0600
  #define DELAY_TIME    10000
#elif MATRIX_COUNT == 3
  #define PERIOD        0x0780
  #define DELAY_TIME    1000
#elif MATRIX_COUNT == 4
  #define PERIOD        0x0A00
  #define DELAY_TIME    2000
#else
  #warning  This board is not tested the selected count of LED matrices!
  // Random values for PERIOD and DELAY_TIME macros
  // You should select proper
  #define PERIOD        0x1000
  #define DELAY_TIME    10000
#endif

#endif
