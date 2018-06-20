#include <msp430.h>
void Uart_SendChar(char c);
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;

    P4SEL |= BIT4+BIT5;                       // P3.3,4 = USCI_A0 TXD/RXD
    UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA1CTL1 |= UCSSEL_2;                     // SMCLK
    UCA1BR0 = 9;                              // 1MHz 115200
    UCA1BR1 = 0;                              // 1MHz 115200
    UCA1MCTL |= UCBRS_1 + UCBRF_0;            // Modulation UCBRSx=1, UCBRFx=0
    UCA1CTL1 &= ~UCSWRST;

    while(1)
    {
        Uart_SendChar('k');
        __delay_cycles(3000000); //3sn Bekleme suresi
    }
}

void Uart_SendChar(char c)
{
    UCA1TXBUF=c;
    while(UCA1STAT&UCBUSY);
}
