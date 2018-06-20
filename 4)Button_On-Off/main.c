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
    int sayac=0;
    while(1)
    {
        if((P2IN & BIT1)==0)
        {
            sayac++;
            __delay_cycles(10000);
        }
        if(sayac%2==1)
        {
            P1OUT |=BIT0;
        }
        if(sayac%2==0)
        {
            P1OUT &=~BIT0;
        }
    }
}
