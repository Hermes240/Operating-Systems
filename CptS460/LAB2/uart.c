/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include <stdint.h>
#define DR   0x00
#define FR   0x18
#include<stdio.h>
#include<stdarg.h>
#define RXFE 0x10
#define TXFF 0x20

typedef unsigned int   u32;
extern char *tab;
typedef struct uart{
  char *base;
  int n;
}UART;

UART uart[4];

int uart_init()
{
  int i; UART *up;

  for (i=0; i<4; i++){
    up = &uart[i];
    up->base = (char *)(0x101F1000 + i*0x1000);
    up->n = i;
  }
  uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}

int ugetc(UART *up)
{
  while (*(up->base + FR) & RXFE);
  return *(up->base + DR);
}

int uputc(UART *up, char c)
{
  while(*(up->base + FR) & TXFF);
  *(up->base + DR) = c;
}

int ugets(UART *up, char *s)
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}


void base(UART *up, u32 x, int b, char *c)
{
  uprints(up, c);

  if(x==0)
  {
    uputc(up,'0');
  }
  else
  {
    myrpu(up,x,b);
  }
  
}

void uprintd(UART *up, int x)
{
  if(x<0)
  {
    uputc(up,'-');
    x = -x;
  }

 myrpu(up,x,10);

}

void myrpu(UART *up, u32 x,int myBASE)
{
  char c;
	if (x) 
  {
		c = tab[x % myBASE];
		myrpu(up,x / myBASE,myBASE);
		uputc(up,c);
	}
}

printu(UART *up, u32 x)
{
  base(up,x,16,"");
}



void uprintf(UART *up, char *fmt, ...) //for formatted print **/
{
   char *cp = fmt;  // set *cp to point to the string ("CPTS %d",360)
   int *ip = (int *)&fmt; //ip points to the arguments past to myprintf

  while (cp[0] != NULL)
  {
    
   if(cp[0] == '%')
    {
      cp++;

      if(cp[0] == 'c')
      {
        uputc(up,*(++cp));
        cp++;
      }

      else if(cp[0] == 's')
      {
        uprints(up,*(++cp));
      }

      else if(cp[0] == 'd')
      {
        uprintd(up,*(++cp));
      }

      else if(cp[0] == 'u')
      {
        printu(up, *(++cp));
      }
    }

    else
    {
      uputc(up,*cp);
      cp++;
    }
    
  }
  

}
