#include "type.h"

int ksleep(int event)
{
    running->event = event;
    running->status = SLEEP;
    //printf("Set proc %d to status = %s in sleep function\n", running->pid, statusStr[running->status]); //FOR TESTING
    proc[running->pid].status = SLEEP;
    tswitch();

}

int kwakeup(int event)
{
    PROC *p, *tempPrev, *temp = sleepList;
    p = tempPrev = 0;
    while(temp)
    {
        if(temp->event == event)
        {
            p = dequeue(&temp);
            p->status = READY;
            //printf("Set proc %d to %s in wakeup function\n", p->pid, statusStr[p->status]); //FOR TESTING
            enqueue(&readyQueue, p);
            if(tempPrev) { tempPrev->next = temp; }
            else {sleepList = temp;}
            return event;
        }
        tempPrev = temp;
        temp = temp->next;
    }
    return -1;
}

int hasChildren(PROC *p)
{
        //PROC *temp = readyQueue;
        //printf("Looking for kids\n"); //FOR TESTING
   // while(temp)
   int i;
    for(i = 0; i < NPROC; i++){
        if(proc[i].ppid == p->pid && proc[i].status != FREE) {return 1;}
        //printf("temp->ppid = %d, p->pid = %d\n", temp->ppid, p->pid); //FOR TESTING
    }
    //}
    //temp = sleepList;
    /*while(temp)
    {
        if(temp->ppid == p->pid) {return 1;}
        temp = temp->next;
    }*/
    printf("returning 0, no kids\n"); //FOR TESTING
    return 0;
}

PROC *findZombieChild()
{
    int i;
    printf("Searching for zombie child\n");

    for(i = 0; i < NPROC; i++)
    {
        //printf("proc %d ppid = %d, running pid = %d\n", i, proc[i].ppid, running->pid);
        if(proc[i].ppid == running->pid && proc[i].status == ZOMBIE)
        {
            return &proc[i];
        }
    }
    return 0;
}

int kwait(int *status)
{
    PROC *child = 0;
    int deadPid = 0;
    //search for children
    if(!hasChildren(running)) {return -1;}
    while(1) {
        while((child = findZombieChild())) //Zombie found
        {
            printf("Found zombie child\n"); //FOR TESTING
            *status = child->exitCode;
            deadPid = child->pid;
            child->status = FREE;
            child->ppid = 0;
            enqueue(&freeList, child);
            return deadPid;
        }
        printf("Sleeping now\n");
        ksleep(running->pid);
    }
}

