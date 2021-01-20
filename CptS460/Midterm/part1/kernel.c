// kernel.c file

#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue, * ChildList;
PROC *sleepList;
int procsize = sizeof(PROC);
int body();

int init()
{
  int i, j; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = READY;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; // circular proc list

  freeList = &proc[0];
  readyQueue = 0;
  sleepList = 0;
  ChildList = 0;
  
  p = dequeue(&freeList);
  p->priority = 0;
  p->ppid = 0;
  running = p;

  kprintf("running = %d\n", running->pid);
  printList("freeList", freeList);

  p->parent = 0;
  p->sibling = 0;
  p->child = 0;
  p->child->pid = 0;
  p->sibling->pid = 0;
}

  
PROC* kfork(int func, int priority)
{
  int i;
  PROC *p = dequeue(&freeList);
  if (p==0){
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  
  p->ppid = running->pid;
  p->parent = running;
  p->sibling = running->child;
  p->child = 0;
  p->status = READY;
  p->priority = priority;
  
  // set kstack for new proc to resume to func()
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);
  enqueue(&readyQueue, p);

  printf("\nproc %d kforked a child %d\n", running->pid, p->pid);
  //printList("readyQueue", readyQueue);
  return p;
}

int scheduler()
{
  //  kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
     enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  //kprintf("next running = %d\n", running->pid);
  if (running->pid){
    color = running->pid;
  }
}

int wakeup()
{
  printf("enter an event to wakeup with: ");
  int event = geti();
  kwakeup(event);
}

int sleep()
{
   printf("enter an event to wakeup with: ");
  int event = geti();
  ksleep(event);
}

int fork()
{
  PROC *q = kfork((int)body, 1);
  insert_child(running,q);
  return 0;
  
}

int wait()
{
  int status, pid;

  pid = kwait(&status);
}

int body()
{
  char c, cmd[64];
  printf("\n");
  kprintf("proc %d resume to body()\n", running->pid);
  while(1){
    printf("-------- proc %d running -----------\n", running->pid); 
    printList("freeList  ", freeList);
    //printList("readyQueue", readyQueue);
    //printsleepList(sleepList);
    printChildList(running->child);

    //printf("running:%d\n",running->child->pid);
	
    printf("Enter a command [switch|kfork|exit|sleep|wakup|wait] : ",
	   running->pid);
    kgets(cmd);
    printf("\n");

    if (strcmp(cmd, "switch")==0)
    {
      tswitch();
      
    } 
    else if (strcmp(cmd, "kfork")==0)
    {
      fork();
    }
    else if (strcmp(cmd, "exit")==0)
    {
      kexit(running->pid);
    }
    else if (strcmp(cmd, "sleep")==0)
    {
      sleep();  
    }
    else if (strcmp(cmd, "wakeup")==0)
    {
      wakeup();   
    }

    else if (strcmp(cmd, "wait")==0)
    {
      wait();   
    }

  }
}

int printChildList(PROC *q)
{
  printf("ChildList=");
  
  while(q){
    if(q->status == 0)
    {
      printf("[%d FREE]->", q->pid);
    }

    else if(q->status == 1)
    {
      printf("[%d READY]->", q->pid);
    }

    else if(q->status == 2)
    {
      printf("[%d SLEEP]->", q->pid);
    }

    else if(q->status == 3)
    {
      printf("[%d BLOCK]->", q->pid);
    }

    else if(q->status == 4)
    {
      printf("[%d ZOMBIE]->", q->pid);
    }
    
    q = q->sibling;
  }
  printf("NULL\n");
}


int insert_child( PROC *parent, PROC *q)
{
  PROC *p;

  p = parent->child;
  if (parent->child==0)
  {
    q->sibling = 0;
    parent->child = q;
    
    
  }
    
  else{
    while(p->sibling)
    {
      p = p->sibling;
    }
    p->sibling = q;
  }

  q->parent = parent;
  q->sibling = 0;
}

