#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  P1DIR |= BIT0;                            // Set P1.0 to output direction
  P1REN |= BIT1;                            // Enable P1.4 internal resistance
  P1OUT |= BIT1;                            // Set P1.4 as pull-Up resistance
  P1IES |= BIT1;                            // P1.4 Hi/Lo edge
  P1IFG &= ~BIT1;                           // P1.4 IFG cleared
  P1IE |= BIT1;                             // P1.4 interrupt enabled

  __bis_SR_register(LPM4_bits + GIE);       // Enter LPM4 w/interrupt
  __no_operation();                         // For debugger
}
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
  P1OUT ^= BIT0;                            // P1.0 = toggle
  P1IFG &= ~BIT1;                          // P1.4 IFG cleared
}
