#include "nokia_5110.h"
#include "english_6x8_pixel.h"



void delay_1us(void)
{
   unsigned int i;
  for(i=0;i<100;i++);

}

  void delay_1ms(void)
  {
   unsigned int i;
   for (i=0;i<1140;i++);
  }

void delay_nms(unsigned int n)
  {
   unsigned int i=0;
   for (i=0;i<n;i++)
   delay_1ms();
  }

void LCD_init(void)
  {


    LCD_5110_DIR |= (0x01 << LCD_RST) + (0x01 << LCD_CE) + (0x01 << LCD_DC)
                    + (0x01 << LCD_DIN) + (0x01<< LCD_CLK);

   LCD_5110_OUT &= ~(0x01 << LCD_RST);
    delay_1us();

   LCD_5110_OUT |= (0x01 << LCD_RST);


    LCD_5110_OUT &= ~(0x01 << LCD_CE);
    delay_1us();

   LCD_5110_OUT |= (0x01 << LCD_CE);
    delay_1us();

    LCD_write_byte(0x21, 0);
    LCD_write_byte(0xc8, 0);
    LCD_write_byte(0x06, 0);
    LCD_write_byte(0x13, 0);
    LCD_write_byte(0x20, 0);
    LCD_clear();
    LCD_write_byte(0x0c, 0);


    LCD_5110_OUT &= ~(0x01 << LCD_CE);
  }


void LCD_clear(void)
  {
    unsigned int i;

    LCD_write_byte(0x0c, 0);            
    LCD_write_byte(0x80, 0);            

    for (i=0; i<504; i++)
      LCD_write_byte(0, 1);         
  }


void LCD_set_XY(unsigned char X, unsigned char Y)
  {
    LCD_write_byte(0x40 | Y, 0);        // column
    LCD_write_byte(0x80 | X, 0);            // row
  }


void LCD_write_char(unsigned char c)
  {
    unsigned char line;

    c -= 32;

    for (line=0; line<6; line++)
      LCD_write_byte(font6x8[c][line], 1);
  }


void LCD_write_string(unsigned char X,unsigned char Y,char *s)
  {
    LCD_set_XY(X,Y);
    while (*s) 
      {
     LCD_write_char(*s);
     s++;
      }
  }

void LCD_write_byte(unsigned char dat, unsigned char command)
  {
    unsigned char i;
    LCD_5110_OUT &= ~(0x01 << LCD_CE); // msp430

    if (command == 0)
      LCD_5110_OUT &= ~(0x01 << LCD_DC);
    else
     LCD_5110_OUT |= (0x01 << LCD_DC);
        for(i=0;i<8;i++)
        {
            if(dat&0x80)

               LCD_5110_OUT |= (0x01 << LCD_DIN);
            else

               LCD_5110_OUT &= ~(0x01 << LCD_DIN);
            //SCLK = 0;
            LCD_5110_OUT &= ~(0x01 << LCD_CLK);
            dat = dat << 1;
            //SCLK = 1;
            LCD_5110_OUT |= (0x01 << LCD_CLK);
        }

     LCD_5110_OUT |= (0x01 << LCD_CE);
  }


void LCD_ShowChar(unsigned char X,unsigned char Y,unsigned char c)
  {
    LCD_set_XY(X,Y);
    LCD_write_char(c);
  }

unsigned int LCD_mypow(unsigned char m,unsigned char n)
{
    unsigned int result=1;
    while(n--)result*=m;
    return result;
}

void LCD_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len)
{
    unsigned char t,temp;
    unsigned char enshow=0;
    for(t=0;t<len;t++)
    {
        temp=(num/LCD_mypow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                LCD_ShowChar(x+6*t,y,'0');
                continue;
            }else enshow=1;
        }
        LCD_ShowChar(x+6*t,y,temp+'0');
    }
}

void LCD_ShowFloat(unsigned char x, unsigned char y,float num,unsigned int n1,unsigned int n2)
{
    int num0,num1,i,p=1;
    num0=num;
    for(i=0;i<n2;i++)   p=p*10;
    num1=(int)(num*p)%p;
    LCD_ShowNum(x,y,num0,n1);
    LCD_ShowChar(x+n1*6+1,y,'.');
    LCD_ShowNum(x+(n1+1)*6,y,num1,n2);
}

void LCD_ShowString(unsigned char X,unsigned char Y,char *s)
  {
    LCD_set_XY(X,Y);
    while (*s)
      {
     LCD_write_char(*s);
     s++;
      }
  }
