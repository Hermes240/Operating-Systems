#ifndef TYPE_H
#define TYPE_H

typedef unsigned char   uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long  uint32_t;

#define NULL 0;
#define NPROC    9
#define SSIZE 1024
#define PSIZE 10
#define NPIPE 10
#define NOFT 20
#define NFD 10

#define BROKEN_PIPE_ERROR -1

#define READ_PIPE 4
#define WRITE_PIPE 5

/******* PROC status ********/
#define FREE     0
#define READY    1
#define RUNNING  2
#define STOPPED  3
#define SLEEP    4
#define ZOMBIE   5

typedef struct proc{
    struct proc *next;
    int    *ksp;               // at offset 2

    int    uss, usp;           // at offsets 4,6

    int    pid;                // add pid for identify the proc
    int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE    
    int    ppid;               // parent pid
    struct proc *parent;
    int    priority;
    int    event;
    int    exitCode;
    char   name[32];

    struct oft *fd[NFD];

    int    kstack[SSIZE];      // per proc stack area
}PROC;

typedef struct oft{
    int mode; //READ, WRITE, READ_PIPE, WRITE_PIPE, etc.
    int refCount;

    struct pipe *pipe_ptr;
    //INODE inodePtr;
    //long offset //For ordinary files
}OFT;

typedef struct pipe{
    char buf[PSIZE];
    int head, tail, data, room;
    int nreader, nwriter; //Number of readers, writers
    int busy; //IN_USE or FREE
}PIPE;

char *statusStr[6] = {"FREE   ", "READY  ", "RUNNING", "STOPPED", "SLEEP  ", "ZOMBIE "};
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
char tmp_str[64];
int procSize = sizeof(PROC);

PIPE pipe[NPIPE];
OFT oft[NOFT];


//queue.c
int enqueue(PROC **queue, PROC *p);
PROC *dequeue(PROC **queue);
int printQueue(PROC *queue, PROC *freeList, PROC *sleepList);

//io.c
int atoi(char *s);
int gets(char s[]);
int strcmp(char *s1, char *s2);
int strncmp(char *s1, char *s2, uint32_t n);
int strcpy(char *s1, char *s2);
int strncpy(char *s1, char *s2, uint32_t n);
void prints(char *s);
void rpu(uint16_t x, int base);
void printu(uint16_t x);
void printd(int x);
void printx(uint16_t x);
void printl(uint32_t x);
void printf(char *fmt, ...);

//int.c
int kcinth();
int kgetpid();
int kps();
int kchname(char *name);
int kkwait(int *status);
int kkexit(int value);
char *get_str(char *str);

//wait.c
int ksleep(int event);
int kwakeup(int event);
int hasChildren(PROC *p);
PROC *findZombieChild();
int kwait(int *status);

//kernel.c
void goUmode();
PROC *kfork(char *filename);
int kexit(int event);
void printProc(PROC *p);

//forkexec.c
int fork();
int copyImage(int childSeg);
int kexec(char *filename);

//pipe.c
int write_pipe(int pd, char *buf, int n);
int read_pipe(int pd, char *buf, int n);
int showPipe(PIPE *p);
int kpipe(int pd[2]);
int close_pipe(int fd);
int pfd();

#endif
