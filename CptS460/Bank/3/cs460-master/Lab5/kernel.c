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

PROC *kfork(char *filename)
{
  int j, segment;
  PROC *p = dequeue(&freeList);
  
  if (p == 0) {
    printf("Failed to kfork()\n");
    getc();
    return(0);
  }
  
  p->status = READY;
  p->priority = 1;
  p->ppid = running->pid;
  p->parent = running;
  p->uss = segment = (p->pid + 1) * 0x1000;
  
  /*
   * INITIALIZE p's kstack to make it start from body() when it runs.
   * To do this, PRETNED that the process called tswitch() from the 
   * the entry address of body() and executed the SAVE part of tswitch()
   * to give up CPU before. 
   * Initialize its kstack[ ] and ksp to comform to these.
   */
  for (j=1; j<10; j++) {
    p->kstack[SSIZE - j] = 0;       // all saved registers = 0
  }
  p->kstack[SSIZE-1]=(int)goUmode;     // called tswitch() from body
  p->ksp = &(p->kstack[SSIZE-9]); // ksp -> kstack top

  //printf("Loading executable\n"); //FOR TESTING
  if(filename)
  {
    load(filename, segment);           // Load executable
    //printf("Executable loaded\n"); //FOR TESTING
    for (j=1; j<13; j++) {
        put_word(0, segment, -j*2);       // Set all registers to 0
    }
    put_word(0x0200, segment, -2);      // Flag
    p->usp = -24;
  }
  else
  {
    copyImage(segment);
    p->usp = running->usp;
    put_word(0, segment, p->usp+16);
  }
  put_word(segment, segment, p->usp+20);     // CS
  put_word(segment, segment, p->usp+2);    // ES
  put_word(segment, segment, p->usp);    // DS
    
  //printProc(p);
  
  enqueue(&readyQueue, p);
  printQueue(readyQueue, freeList, sleepList);
  /*printf("Ready queue:\n");
  print_queue(readyList);*/
  
  return(p->pid);
}

int kexit(int event)
{
    int i, hc = 0;

    running->exitCode = event;

    //run through each proc list to find any children
    for(i = 0; i < NPROC; i++)
    {
        if(proc[i].ppid == running->pid) //child found
        {
            hc = 1;
            if(running->pid == 1) {break;}
            proc[i].ppid = 1; //change parent to proc1
        }
    }
    if(running->pid == 1 && hc) //Can't let proc1 die
    {
        printf("Proc 1 still has children! Cannot die\n");
        return -1;
    }
    running->status = ZOMBIE;
    if(hc) {kwakeup(1);}
    kwakeup(running->ppid);
    tswitch();
}

void printProc(PROC *p) {
    int i, value;
    printf("*** Process Info ************************************\n");
    printf("Address: 0x%x Next: 0x%x Stack: 0x%x\n", p, p->next, p->ksp);
    printf("PID: %d Status: %d Priority: %d\n", p->pid, p->status, p->priority);
    printf("Parent: %d Address: 0x%x\n", p->ppid, p->parent);
    printf("*****************************************************\n");
    printf("usp: %d, uss: 0x%x\n", p->usp, p->uss);
    printf("Stack:\n");
    for(i=-30; i<0; i+=2) {
      value = get_word(p->uss, i);
      printf("Offset %d: 0x%x(%d)\n", i, value, value);
    }
    printf("*****************************************************\n");
}

