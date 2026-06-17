/*
 * Name:         Miguel Diaz-Alvarez
 * Date:         6/3/2025
 * Assignment:   Lab 1
 * YouTube:      https://www.youtube.com/watch?v=Wyqa_8PNKbA
*/

#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  P1DIR |= 0x01;                            // Set P1.0 to output direction

  for (;;)
  {
    volatile unsigned int i;

    P1OUT ^= 0x01;                          // Toggle P1.0 using exclusive-OR

    i = 50000;                              // Delay
    do (i--);
    while (i != 0);
  }
}
