#ifndef MATRIX_H
#define MATRIX_H

#define DATA_LATCH_PIN		0x10  // P1.4
#define SPI_CLK_OUT_PIN		0x20  // P1.5
#define SPI_DATA_OUT_PIN	0x40  // P1.6

extern int First, Message_Length;

void TimerA_Init(unsigned int Counter, unsigned int prescaler);

void USI_Init (void);

int String_Length (const char Input[]);

void LED_Matrix_Init (void);

__interrupt void Write_Matrix (void);

#endif
