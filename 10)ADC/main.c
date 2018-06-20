#include <msp430.h>

unsigned int ADCValue;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  ADC12CTL0 = ADC12SHT02 + ADC12ON;         // Sampling time, ADC12 on
  ADC12CTL1 = ADC12SHP;                     // Use sampling timer
  ADC12CTL0 |= ADC12ENC;
  P6SEL |= 0x01;                            // P6.0 ADC option select
  P1DIR |= 0x01;                            // P1.0 output

  while (1)
  {
    ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    ADCValue =ADC12MEM0;
    if(ADCValue<=0x7FF)
    {
        P1OUT |= BIT0;
    }
    else
    {
        P1OUT &= ~BIT0;
    }
  }
}

