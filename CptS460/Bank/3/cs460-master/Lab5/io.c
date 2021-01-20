#include "type.h"

#define BASE_10 10
#define BASE_16 16

char *table = "0123456789ABCDEF";
int getbp();
int atoi(char *s)
{
    int neg, x;
    neg = 0;
    x = 0;
    if(*s == '-') {neg = 1; s++;}
    while (*s)
    {
        x *= 10;
        x += (int)((*s++) -'0');
    }
    if(neg) {x *= -1;}
    return x;
}

int gets(char s[])
{
  while((*s = getc()) != '\r') { putc(*s++); }
  *s = 0;
  prints("\n\r");
  return 1;
}

int strcmp(char *s1, char *s2)
{
    while(*s1 != 0 && *s2 != 0)
    {
        if(*s1++ != *s2++) {return --*s1 - --*s2;}
    }
    return 0;
}

int strncmp(char *s1, char *s2, uint32_t n)
{
    int i = 0;
    while(*s1 != 0 && *s2 != 0 && i < n)
    {
        if(*s1++ != *s2++) {return --*s1 - --*s2;}
        i++;
    }
    return 0;
}

//stringcopy(source, dest)
int strcpy(char *s1, char *s2)
{
    while(*s1 != 0)
    {
        *s1++ = *s2++;
    }
    *s2 = 0;
    return 1;
}

//stringcopy(source, dest, length)
int strncpy(char *s1, char *s2, uint32_t n)
{
    int i = 0;
    while(*s1 != 0 && i < n)
    {
        *s1++ = *s2++;
        i++;
    }
    *s2 = 0;
    return 1;
}

void prints(char *s)
{
	char *current = s;
	while(*current)
	{
		putc(*current++);
	}
}

void rpu(uint16_t x, int base)
{
	char c;
	if(x)
	{
		c = table[x%base];
		rpu(x/base, base);
		putc(c);
	}
}

void printu(uint16_t x)
{
    if(x==0)
    {
        putc('0');
    }
    else
    {
        rpu(x, BASE_10);
    }
}

void printd(int x)
{
    if (x < 0)
    {
        putc('-');
        x *= -1;
    }
    if (x == 0)
    {
        putc('0');
    }
    else
    {
        rpu(x, BASE_10);
    }
}

void printx(uint16_t x)
{
    if(x == 0)
    {
        putc('0');
    }
    else
    {
        rpu((uint16_t)x, BASE_16);
    }
}

void printl(uint32_t x)
{
    if(x == 0)
    {
        putc('0');
    }
    else
    {
        rpu((uint32_t)x, BASE_10);
    }
}

void printf(char *fmt, ...)
{
    unsigned short bp;
    char **cp;
    char *fmtStr;
    int *ip;
    char tmp;
    
    bp = getbp();
    //printf("bp = %x\n", bp); // For testing
    cp = (char**)(bp+4);
    //printf("cp = %x, *cp = %c\n", cp, **cp); // For testing
    fmtStr = *cp;
    //printf("fmtStr = %s", fmtStr);
    ip = (uint16_t *)(bp+6);
    //printf("ip = %x\n", ip); // For testing

    while(*fmtStr != 0)
    {
        if(*fmtStr == '%')
        {
            fmtStr++;
            switch(*fmtStr)
            {
                case 'c': tmp = (char)*ip;
                        putc(tmp);

                        break;
                case 's': prints((char *)(*ip));
                        break;
                case 'd': printd((int)*ip);
                        break;
                case 'x': printx((uint16_t)*ip);
                        break;
                case 'u': printu((uint16_t)*ip);
                        break;
                case 'l': ip++; 
                        printu((uint32_t)*ip);
                        break;
                default:
                    prints(" Invalid format specifier ");
                    break;
            }
            //fflush(stdout);
            ip++;
        }
        else
        {
            putc(*fmtStr);
            //fflush(stdout);
            if(*fmtStr == '\n')
            {
                putc('\r');
            }
        }
        fmtStr++;
    }

}

putc();
getc();
