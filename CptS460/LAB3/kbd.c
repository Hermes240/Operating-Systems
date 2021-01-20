// kbd.c file

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

#include "keymap"
#include "keymap2"

typedef struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

KBD kbd;
int release, shiftted,control;

int keyset;
int kbd_init()
{
  char scode;
  keyset = 1; // default to scan code set-1
  
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10; // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;
  kp->head = kp->tail = 0;
  kp->data = 0; kp->room = 128;

  release = 0;

  printf("Detect KBD scan code: press the ENTER key : ");
  while( (*(kp->base + KSTAT) & 0x10) == 0);
  scode = *(kp->base + KDATA);
  printf("scode=%x ", scode);
  if (scode==0x5A)
    keyset=2;
  printf("keyset=%d\n", keyset);

  control = 0;
  shiftted = 0;
}

void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;

  scode = *(kp->base + KDATA);

   if (scode == 0xF0){ // key release 
    release = 1;
    return;
  }

  if (release && scode){
    release = 0;
    if(scode == 0x12)
    {
      shiftted = 0;

    }

    else if(scode == 0x14)
    {
      control = 0;
    }
    return;
  }

  if(scode == 0x12)
  {
    shiftted = 1;
    return;
  }

  if(scode == 0x14)
  {
    control = 1;
    return;
  }

  if(control)
  {
    printf("c = %x ^%c\n",utab[scode],utab[scode]);

    if(utab[scode] =='D')
    {
      kp->buf[kp->head++] = 0x04;
      kp->head %= 128;
      kp->data++;
      kp->room--;
    }
    return;
  }

  if(!shiftted)
  {
    c = ltab[scode];
  }
  else
  {
    c = utab[scode];
  }

  printf("c = %x %c\n",c,c);

  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++;
  kp->room--;
  

}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  while (kp->data == 0);   // BUSY WAIT for kp>data
  
  lock();
   c = kp->buf[kp->tail++];
   kp->tail %= 128;
   kp->data--; kp->room++;
  unlock();
  return c;
}

int kgets(char s[ ])
{
  char c;
  while( (c = kgetc()) != '\r'){
    if (c=='\b'){
      s--;
      continue;
    }
    *s++ = c;
  }
  *s = 0;
  return strlen(s);
}
