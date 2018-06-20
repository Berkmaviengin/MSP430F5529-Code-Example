#include <msp430.h> 

/*
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    P1DIR |=BIT0;
    P2REN |=BIT1;
    P2OUT |=BIT1;
    while(1)
    {
        if((P2IN & BIT1)==0)
        {
            P1OUT |=BIT0;
        }
        else
        {
            P1OUT &=~BIT0;
        }
    }
}
