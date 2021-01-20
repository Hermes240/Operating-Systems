// wait.c file

extern PROC *running;
extern PROC *sleepList;

// int kexit()  // SIMPLE kexit() for process to terminate
// {
//   printf("proc %d exit\n", running->pid);
//   running->status = ZOMBIE;
//   tswitch();
// }

int ksleep(int event)
{
  int SR = int_off();
  printf("proc%d sleep on event=%x\n", running->pid, event);  
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);

  tswitch();
  int_on(SR);
}

int kwakeup(int event)
{
  PROC *p, *tmp=0;
  int SR = int_off();
  while((p = dequeue(&sleepList))!=0){
    if (p->event==event){
      if (running->pid)
         printf("proc%d wakeup %d\n", running->pid, p->pid);
      p->status = READY;
      enqueue(&readyQueue, p);
    }
    else{
      enqueue(&tmp, p);
    }
  }
  sleepList = tmp;
  int_on(SR);
}

void remove(PROC *p)
{
  PROC *q;

  if(p->parent->child == p)
  {
    p->parent->child = p->sibling;
  }

  else
  {
    for(q = p->parent->child; q->sibling != p; q = q->sibling)
    {
      q->sibling = p->sibling;
    }
  }

  p->child = 0;
  p->parent = 0;
  p->sibling = 0;
  p->ppid = 0;
  
}


int kwait(int *status) // wait for ZOMBIE child
{
PROC *p; int i, hasChild = 0;
while(1){
// search PROCs for a child
for (i=1; i<NPROC; i++){
// exclude P0
p = &proc[i];
if (p->status != FREE && p->ppid == running->pid){
hasChild = 1;
// has child flag
if (p->status == ZOMBIE){
  printf("\nFOUND A ZOMBIE %d\n",p->pid);
// lay the dead child to rest
*status = p->exitCode;
// collect its exitCode
p->status = FREE;
// free its PROC
enqueue(&freeList,p);
//put_proc(&freeList, p); // to freeList
remove(p);
// once less processes
return(p->pid);
// return its pid
}
}
}
if (!hasChild) return -1;
// no child, return ERROR
ksleep(running); // still has kids alive: sleep on PROC address
}
}

int kexit(int exitValue)
{
  PROC *p;
int i, wakeupP1 = 0;
if (running->pid==0){ // nproc = number of active PROCs
printf("other procs still exist, P1 can't die yet\n");
return -1;
}
/* send children (dead or alive) to P1's orphanage */
for (i = 1; i < NPROC; i++){
p = &proc[i];
if (p->status != FREE && p->ppid == running->pid){
p->ppid = 1;
insert_child(&proc[1],p);
//p->parent = &proc[1];
wakeupP1++;
}

}
/* record exitValue and become a ZOMBIE */
running->exitCode = exitValue;
running->status = ZOMBIE;
/* wakeup parent and also P1 if necessary */
kwakeup(running->parent); // parent sleeps on its PROC address
if (wakeupP1)
{
  kwakeup(&proc[1]);
}
  
tswitch();
// give up CPU

}
