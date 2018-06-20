#include <msp430.h> 
#include "nokia_5110.h"

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;
	LCD_init();
	LCD_clear();
	LCD_write_string(8,2,"Hello World");
	
	return 0;
}
