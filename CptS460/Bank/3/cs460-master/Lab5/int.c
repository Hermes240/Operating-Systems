#include "type.h"
/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/

/****************** syscall handler in C ***************************/
int kcinth()
{
   int a,b,c,d, r;

    //WRITE CODE TO GET get syscall parameters a,b,c,d from ustack 
    a = get_word(running->uss, running->usp + 26);
    b = get_word(running->uss, running->usp + 28);
    c = get_word(running->uss, running->usp + 30);
    d = get_word(running->uss, running->usp + 32);

   switch(a){
       case 0 : r = kgetpid();        break;
       case 1 : r = kps();            break;
       case 2 : r = kchname(b);       break;
       case 3 : r = kkfork();         break;
       case 4 : r = ktswitch();       break;
       case 5 : r = kkwait(b);        break;
       case 6 : r = kkexit(b);        break;
       case 7 : r = kexec(b);         break;

       case 90: r = getc();           break;
       case 91: r = putc(b);          break;
       case 99: kkexit(b);            break;
       default: printf("invalid syscall # : %d\n", a); 
   }

    //WRITE CODE to let r be the return value to Umode
    put_word(r, running->uss, running->usp + 16);
    return r;

}

//============= WRITE C CODE FOR syscall functions ======================

int kgetpid()
{
    return running->pid;
}

char *get_str(char *str) {
    int i = 0;
    
    while (i < sizeof(tmp_str)) {
        tmp_str[i] = get_byte(running->uss, str + i);
        if (tmp_str[i] == '\0') {
            break;

        }
        i++;
    }
    
    return(tmp_str);
}

int kexec(char *filename) {
    int i, reg;
    char *str = get_str(filename);
    
    // Load new uMode image
    load(str, running->uss);
    
    // Reset uStack
    for (i=-2; i>=-24; i-=2) {
        switch(i) {
            case -2:    reg = 0x0200; break;
            case -4:
            case -22:
            case -24:   reg = running->uss; break;
            default:    reg = 0; break;
        }
        put_word(reg, running->uss, i);
    }
    
    running->usp = -24;
    return(0);
}


int kps()
{
    PROC *p;
    int i;
    printf("////////////////////////////////////////////////////////////////////\n");
    printf("%s%s%s%s\n", "name        ", "status        ", "pid       ", "ppid      ");
    printf("////////////////////////////////////////////////////////////////////\n");

    for (i = 0; i < NPROC; i++)
    {
        printf("%s     %s        %d         %d\n", proc[i].name, statusStr[proc[i].status], proc[i].pid, proc[i].ppid); 
    }
    printf("/////////////////////////////////////////////////////////////////\n");
}

int kchname(char *name)
{
    char c;
    int i = 0;
   
    while (i < 32) {
      c = get_byte(running->uss, name + i);
      running->name[i] = c;
      if (c == '\0') { break; }
      i++;
   }
   //printf("New name: %s\n", running->name);
   return(0);
}

int kkfork()
{
  int i = kfork();
  return i;
}

int ktswitch()
{
   return tswitch();
}

int kkwait(int *status)
{

  int pid, c;
  pid = kwait(&c);
  put_word(c, running->uss, status);
  return pid;
}

int kkexit(int value)
{
    int i = kexit(value);
    return i;
}
