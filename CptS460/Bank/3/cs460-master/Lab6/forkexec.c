#include "type.h"

int copyImage(int childSeg)
{
    int i, word;
    printf("Copying 0x%x to 0x%x\n", running->uss, childSeg);
    for(i = 0; i < 0xFFFE; i += 2)
    {
        word = get_word(running->uss, i);
        put_word(word, childSeg, i);
    }
    return 0;
}

int fork()
{
  int i, pid;  uint16_t segment;

  PROC *p = kfork(0);   // kfork() but do NOT load any Umode image for child 
  if (p == 0)           // kfork failed 
    return -1;

   return(p->pid);
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

