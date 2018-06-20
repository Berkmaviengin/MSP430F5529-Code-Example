#ifndef __nokia_5110_h_
#define __nokia_5110_h_

#include <msp430.h>

#define LCD_5110_DIR            P3DIR
#define LCD_5110_OUT        P3OUT

#define   LCD_RST    7
#define   LCD_CE    3
#define   LCD_DC    4
#define   LCD_DIN    5
#define   LCD_CLK    6


//extern void LCD_PORT_INIT(void);
extern void LCD_init(void);
extern void LCD_clear(void);
extern void LCD_write_string(unsigned char X,unsigned char Y,char *s);
extern void LCD_write_char(unsigned char c);
extern void LCD_write_byte(unsigned char dat, unsigned char dc);

extern void LCD_ShowChar(unsigned char X,unsigned char Y,unsigned char c);
extern unsigned int LCD_mypow(unsigned char m,unsigned char n);
extern void LCD_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len);
extern void LCD_ShowFloat(unsigned char x, unsigned char y,float num,unsigned int n1,unsigned int n2);
extern void LCD_ShowString(unsigned char X,unsigned char Y,char *s);

#endif
