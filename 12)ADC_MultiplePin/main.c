#include <msp430.h>
#include <stdio.h>
int volt_hex1, volt_hex2;
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
    ADC12CTL0 = ADC12ON + ADC12MSC ; // Sampling time, ADC12 on
    ADC12CTL1 = 0x7000+ADC12SHP + ADC12CONSEQ_1; // Sample and Hold Mode
    ADC12IE = 0x80; // Enable interrupt for channel 7
    ADC12MCTL0 |= ADC12INCH_0;
    ADC12MCTL1 |= 0x07 + ADC12EOS;
    P6SEL |= 0x70; // P6.7 ADC option select ==>RTS
    P6DIR &= 0x7F;
    P1DIR |= BIT0;
    P6SEL = 0X01; //P6.0 ADC option select
    P1DIR |= 0x01; // P1.0 output
    P4DIR |=BIT7;
    while(1)
    {
        ADC12CTL0 |= ADC12ENC; //Enable conversion
        ADC12CTL0 |= ADC12SC; // start conversion
        __bis_SR_register(LPM0_bits | GIE); // Enter Low Power Mode 0 + Global Interrupt Enable
    }
}
// ADC ISR
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    switch(__even_in_range(ADC12IV,34))
    {
        case 0: break; // Vector 0: No interrupt
        case 2: break; // Vector 2: ADC overflow
        case 4: break; // Vector 4: ADC timing overflow
        case 6:break; // Vector 6: ADC12IFG0
        case 8: break; // Vector 8: ADC12IFG1
        case 10: break; // Vector 10: ADC12IFG2
        case 12: break; // Vector 12: ADC12IFG3
        case 14: break; // Vector 14: ADC12IFG4
        case 16: break; // Vector 16: ADC12IFG5
        case 18: break; // Vector 18: ADC12IFG6
        case 20:
            volt_hex1 = ADC12MEM1; // Read converted value for channel 7. This clears the interrupt flag
            volt_hex2 = ADC12MEM7;
            if(volt_hex1<=0x7ff)
            {
                P1OUT |=BIT0;
            }
            if(volt_hex1>0x7ff)
            {
                P1OUT &=~BIT0;
            }
            if(volt_hex2<=0x7ff)
            {
                P4OUT |=BIT7;
            }
            if(volt_hex2>0x7ff)
            {
                P4OUT &=~BIT7;
            }
            __bic_SR_register_on_exit(LPM0_bits); // Quit Low Power Mode 0 on exit to enable CPU for next Start Conversion operation
            break; // Vector 20: ADC12IFG7
        case 22: break; // Vector 22: ADC12IFG8
        case 24: break; // Vector 24: ADC12IFG9
        case 26: break; // Vector 26: ADC12IFG10
        case 28: break; // Vector 28: ADC12IFG11
        case 30: break; // Vector 30: ADC12IFG12
        case 32: break; // Vector 32: ADC12IFG13
        case 34: break; // Vector 34: ADC12IFG14
        default: break;
    }
}
