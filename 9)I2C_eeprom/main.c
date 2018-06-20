#include <msp430.h> 

/*
 * main.c
 */
/********************************************************************************************/
/*    I2C Definitions                                                                       */
/********************************************************************************************/
//I2C Port Definitions
#define   i2cAddress     0x50    //Address GPIO Address

//USCI I2C PIN Definition
#define   SDA_PIN     BIT7      //Bit 7 USCI Port 1(SDA)
#define   SCL_PIN     BIT6      //Bit 6 USCI Port 1(SCL)


/********************************************************************************************/
/*    UART Function Definitions                                                                  */
/********************************************************************************************/

void UART_puts(char * s);
void UART_outdec(long data, unsigned char ndigits);

/********************************************************************************************/
/*    I2C Function Definitions                                                                  */
/********************************************************************************************/
void I2C_Write_EEProm(unsigned char slave_address, unsigned int register_address, char * data, unsigned char DataLength );
void I2C_Read_EEProm(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength );


/********************************************************************************************/
/*    Misc Function Definitions                                                                  */
/********************************************************************************************/
void delay_ms(unsigned int delay);          //Delay


/********************************************************************************************/
/*   Main                                                                  */
/********************************************************************************************/
void main(void) {

    WDTCTL = WDTPW + WDTHOLD;           // Stop WDT

    BCSCTL1 = CALBC1_16MHZ;             //set DCO to 16Mhz
    DCOCTL = CALDCO_16MHZ;              // make sure to update the Delay_ms function if the DCO is changed


    //UART Initial
    P1SEL = BIT1 + BIT2;                     // P1.1 = RX pin, P1.2=TX pin
    P1SEL2 = BIT1 + BIT2 ;                    // P1SEL and P1SEL2 = 11--- Secondary peripheral module function is selected.
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 69;                             // 16MHz 230400
    UCA0BR1 = 0;                              // 16MHz 230400
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

    //USCI_I2C_Init
    P1SEL |= SDA_PIN + SCL_PIN;                     // Assign I2C pins to USCI_B0
    P1SEL2|= SDA_PIN + SCL_PIN;
    UCB0CTL1 = UCSWRST;                             // Enable SW reset, HOLD USCB in a reset state
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;           // I2C Master, MODE 3 = I2C, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;                  // Use SMCLK, keep SW reset
    UCB0BR0 = 72;                                   // Set I2C master speed  72 gives approx 200Khz clock at 16Mhz
    UCB0BR1 = 0;                                    // Set I2C master speed
    UCB0CTL1 &= ~UCSWRST;                           // Clear SW reset, resume operation


    UART_puts("Starting...\r\n");       //output to UART to indicate the program is running

    __bis_SR_register(CPUOFF + GIE);                                    // LPM0 with interrupts enable

        while (1){ }
}

 void UART_puts(char * s) {
    while (*s) {
        while (!(IFG2 & UCA0TXIFG));                // USCI_A0 TX buffer ready?
        UCA0TXBUF = *s++;
    }
 }


 void UART_outdec(long data, unsigned char ndigits){    //UART_outdec modified/hacked to properly handle negative numbers.
    unsigned char sign, s[15];          //I copied this from a TI example or the 40oh forum, but I'm not sure of the original author.
    unsigned int i;
    sign = 0x00;

    if(data < 0) {
        sign='-';
        data = -data;
    }
    i = 0;

    do {
        s[i++] = data % 10 + '0'; //adds the value of data least sig digit to ascii value of '0'
        if(i == ndigits) {
            s[i++]='.';
        }
    } while( (data /= 10) > 0);

    if (i < ndigits) //fixes loss of leading 0 in fractional portion when number of digits is less than length of data
    {
        do {
            s[i++]='0';
            } while (ndigits > i) ;

        s[i++]='.';
    }

    if (sign == '-')  //if value is negative then include the '-' sign
    {
        s[i] = sign;
    } else
    {           //if value is positive then reduce 'i' counter by 1 to prevent the do loop from trying to output a sign character.
        i--;
    }

    do {
        while (!(IFG2 & UCA0TXIFG));
        UCA0TXBUF = s[i];
    } while(i--);
 }




 void delay_ms(unsigned int delay)
 {
     while (delay--)
     {
         __delay_cycles(16000);  //1ms = 1000 cycles per 1Mhz clock freq.
     }
 }


void I2C_Read_EEProm(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
{ //Reading from a 24LCxxx series is much easier then writing.  Reading doesn't have to be done in 64 byte pages.
    /*
         * Todo:
         * 1. add checks to make sure write does not exceed maximum length of EEprom
         * 2. check for valid memory_address
         *
         */

    int rLoop = 0;  //loop counter

    while (UCB0STAT & UCBBUSY);         //wait for USCI B0 bus to be inactive
    UCB0I2CSA = slave_address;          //set SLAVE address

    UCB0CTL1 |= UCTR + UCTXSTT;                     //set USCI to be I2C TX,  send start condition
    UCB0TXBUF = (memory_address & 0x7F00) >> 8;     //transfer memory_address MSB

    while (UCB0CTL1 & UCTXSTT);                     // waiting for slave address to transfer
    while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear

    UCB0TXBUF = (memory_address & 0x00FF);          //transfer memory_address LSB
    while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear

    UCB0CTL1 &= ~UCTR;              //set USCI to be RECEIVER
    UCB0CTL1 |= UCTXSTT;            //send restart
    while (UCB0CTL1 & UCTXSTT);     // wait until I2C STT is sent

    for (rLoop=0; rLoop<DataLength; rLoop++)    //receive loop
    {
         while ((IFG2 & UCB0RXIFG) != UCB0RXIFG); //wait for RX buffer to have data
            data[rLoop] = UCB0RXBUF;  //Move rvcd data of or USCI buffer. This also clears the UCB0RXIFG flag

            if (rLoop == DataLength-2)  //NACK and STOP must be send before the last byte is read from the buffer.
                                        //if not the CPU will read an extra byte.
            {
                UCB0CTL1 |= UCTXNACK; //generate a NACK
                UCB0CTL1 |= UCTXSTP;  //generate a stop condition
            }
    }


}

void I2C_Write_EEProm(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
{
    /*
     * Todo:
     * 1. add checks to make sure write does not exceed maximum length of EEprom
     * 2. check for valid memory_address
     *
     */


    int NumPages = (DataLength)/64 ; //Count of full 64 byte pages, 0 means the data is less than a full 64 byte page
    int PartialPageBytes = DataLength % 64;  //this is the number of bytes remaining that do not make up a full page

    int address = 0; //EEprom memory starting address, this is different from the I2C slave address
    int NP =1; //loop counter to iterate though pages of memory
    int offset = 0;
    int offsetlimit = 0;

    if (PartialPageBytes > 0)
    {
        NumPages++; //if we have left over bytes that do not make up a full page, this will add a page to handle those bytes.
    }

    __disable_interrupt(); //prevent interrupts from messing with the I2C functions

    while (UCB0STAT & UCBBUSY);     //wait for USCI B0 bus to be inactive
            UCB0I2CSA = slave_address;          //set SLAVE address

    for (NP=1; NP<=NumPages; NP++)
    {

        address = ((NP-1) * 64) + memory_address; //this is the full page start address

        UCB0CTL1 |= UCTR + UCTXSTT;         //set USCI to be I2C TX,  send start condition
        UCB0TXBUF = (address & 0x7F00) >> 8;        //transferring memory_address MSB

        while (UCB0CTL1 & UCTXSTT);                 // waiting for slave address was transferred
        while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear

        UCB0TXBUF = (address & 0x00FF);     //transferring memory_address LSB
        while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear

        offsetlimit = 63;       //set the offset limit to 63

        if ((NP == NumPages) && (PartialPageBytes > 0))     //if we are preparing to send the last/partial page
        {
            offsetlimit = PartialPageBytes-1;               //set the offset limit to the number of partial page bytes
        }
        for (offset=0; offset <=offsetlimit; offset++)
        {
            UCB0TXBUF = data[((NP-1)*64)+offset];                   //send data.
            //UART_outdec(offset,0);

            while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);    //wait for TX IFG to clear

        }

        UCB0CTL1 |= UCTXSTP;                    // set I2C stop condition
        while ((UCB0STAT & UCSTPIFG) == UCSTPIFG); //wait for Stop condition to be set

                        //delay while the EEPROM completed its write cycle.
                        //It would be better to use NACK polling here as described in the datasheet.
        delay_ms(6);    //24LC256 has a max page write time of 5ms, so we will wait 6ms to be sure
    }

    UART_puts("Done...\r\n");
    __enable_interrupt();

}


// -----------------------------------------------------------------------------
// Interrupt Handlers
// -----------------------------------------------------------------------------


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)

    char RxChar = 0x00; //char used to receive serial data
    RxChar = UCA0RXBUF;                    // copy the RX buffer into RxChar

    char buf[4] = {0};  //i2c received data buffer


    //void I2C_Write_EEProm(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
    I2C_Write_EEProm(i2cAddress,0x00,"ABCDEFGHIJKLMNOP",15);

    //void I2C_Read_EEProm(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
    I2C_Read_EEProm(i2cAddress,0x05,buf,5);  //reads three bytes starting at memory location 0x05 and returns the data into 'buf'
    UART_puts("READING: \r\n");
    UART_puts(buf);
    UART_puts("\r\n");



    __bis_SR_register(CPUOFF + GIE);                                    // LPM0 with interrupts enable

} //end UART RCV ISR
