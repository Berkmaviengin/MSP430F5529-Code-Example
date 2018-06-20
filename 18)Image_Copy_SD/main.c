

#include <MSP430F5529.h>
#include "ff.h"
#include "diskio.h"

void initCLK(void);
void SetVcoreUp (unsigned int level);
FRESULT WriteFile(char*, char*, WORD);
void fat_init(void);
void New(void);
void fat_init_1(void);
FIL file;                                               /* Opened file object */
FATFS fatfs;                                            /* File system object */
DIRS dir;                                               /* Directory object   */
FRESULT errCode;                                        /* Error code object  */
FRESULT res;                                            /* Result object      */
UINT bytesRead;                                         /* Bytes read object  */
UINT read;     /* Read bytes object  */
DWORD bytees;
unsigned char deneme[]="BerkMaviengin\n28.12.2017\n";
unsigned char MST_Data,SLV_Data;
BYTE buffer[32];
int result=1;

void main(void){
    unsigned char buffer[50];
    unsigned int size;

    WDTCTL = WDTPW+WDTHOLD;                     // Stop watchdog timer

    initCLK();
    P1DIR |=BIT0;
    P1OUT &=~BIT0;
    __enable_interrupt();

    long i=0;
    long bytes=0;                              //mount, set directory to read from, assign file
    unsigned int bytesWritten;
    fat_init();
    f_lseek(&file, bytes);
    res = f_read(&file, buffer, sizeof(buffer), &size);
    f_close(&file);
    f_mount(0,0);
    New();
    f_lseek(&file, bytes);
    f_write(&file, buffer, sizeof(buffer), &bytesWritten);
    f_close(&file);
    f_mount(0,0);
    for(i=0;i<4920;i++){
        fat_init();
        bytes=bytes+50;
        f_lseek(&file, bytes);
        res = f_read(&file, buffer, sizeof(buffer), &size);
        f_close(&file);
        f_mount(0,0);
        fat_init_1();
        f_lseek(&file, bytes);
        f_write(&file, buffer, sizeof(buffer), &bytesWritten);
        f_close(&file);
        f_mount(0,0);


    }
    f_close(&file);
    f_mount(0,0);

    P1OUT |=BIT0;
    while(1)
    {
    }
}

void fat_init(void){
    errCode = -1;

    while (errCode != FR_OK){                               //go until f_open returns FR_OK (function successful)
        errCode = f_mount(0, &fatfs);                       //mount drive number 0
        errCode = f_opendir(&dir, "/");                     //root directory
        errCode = f_open(&file, "/PiZero1.png", FA_READ);

        if(errCode != FR_OK)
            result=0;                                       //used as a debugging flag
    }

}
void fat_init_1(void){
    errCode = -1;

    while (errCode != FR_OK){                               //go until f_open returns FR_OK (function successful)
        errCode = f_mount(0, &fatfs);                       //mount drive number 0
        errCode = f_opendir(&dir, "/");                     //root directory
        errCode = f_open(&file, "/berkcpy.png", FA_WRITE);

        if(errCode != FR_OK)
            result=0;                                       //used as a debugging flag
    }

}
void New(void){
    errCode = -1;

    while (errCode != FR_OK){
        errCode = f_mount(0, &fatfs);                       //mount drive number 0
        errCode = f_opendir(&dir, "/");  //go until f_open returns FR_OK (function successful)
        errCode = f_open(&file, "/berkcpy.png", FA_CREATE_ALWAYS | FA_WRITE);
        if(errCode != FR_OK)
            result=0;                                       //used as a debugging flag
    }

}



void initCLK(void){
  volatile unsigned int i;

  // Increase Vcore setting to level3 to support fsystem=25MHz
  // NOTE: Change core voltage one level at a time..
  SetVcoreUp (0x01);
  SetVcoreUp (0x02);
  SetVcoreUp (0x03);

  UCSCTL3 = SELREF_2;                       // Set DCO FLL reference = REFO
  UCSCTL4 |= SELA_2;                        // Set ACLK = REFO

  __bis_SR_register(SCG0);                  // Disable the FLL control loop
  UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
  UCSCTL1 = DCORSEL_7;                      // Select DCO range 50MHz operation
  UCSCTL2 = FLLD_1 + 762;                   // Set DCO Multiplier for 25MHz
                                            // (N + 1) * FLLRef = Fdco
                                            // (762 + 1) * 32768 = 25MHz
                                            // Set FLL Div = fDCOCLK/2
  __bic_SR_register(SCG0);                  // Enable the FLL control loop

  // Worst-case settling time for the DCO when the DCO range bits have been
  // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
  // UG for optimization.
  // 32 x 32 x 25 MHz / 32,768 Hz ~ 780k MCLK cycles for DCO to settle
  __delay_cycles(782000);

  // Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
}

void SetVcoreUp (unsigned int level)
{
  // Open PMM registers for write
  PMMCTL0_H = PMMPW_H;
  // Set SVS/SVM high side new level
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
  // Set SVM low side to new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
  // Wait till SVM is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait till new level reached
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set SVS/SVM low side to new level
  SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
}
