#include <msp430.h> 

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;
	P1DIR |=BIT0;
	P1OUT |=BIT0;
	return 0;
}
