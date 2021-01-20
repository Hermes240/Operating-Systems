#include "type.h"

show_pipe(PIPE *p)
{
   int i, j;
    printf("Head: %u, Tail: %u, Data: %u, Room: %u\n",
           p->head, p->tail, p->data, p->room);
    printf("Readers: %u, Writers: %u\n", p->nreader, p->nwriter);
   printf("------------ PIPE CONTENTS ------------\n"); 
   for(i = 0, j = p->tail; i < p->data; i++, j++)
   {
        if(j == PSIZE) { j = 0;}
        printf("%c", p->buf[j]);
   }
   printf("\n----------------------------------------\n");
}

char *MODE[ ]={"READ_PIPE ","WRITE_PIPE"};

int pfd()
{
    int i;
    running->fd;
    printf("-----------File Descriptor Info----------\n");
    printf("fd   mode        refCount\n");
    for(i = 0; i < NFD; i++)
    {
        if(running->fd[i]->refCount > 0)
        {
            printf("%d   %s   %d\n", i, MODE[running->fd[i]->mode-4], running->fd[i]->refCount);
        }
    }
    printf("-----------------------------------------\n");
  // print running process' opened file descriptors
} 
//============================================================


int read_bytes(PIPE *p, char *buf, int *br, int n)
{
    int i = 0;
    while(*br < n && p->data > 0)
    {
        //buf[(*br)++] = p->buf[(p->tail)++];
        put_byte(p->buf[(p->tail)++], running->uss, buf  +  *br);
        (p->data)--;
        (p->room)++;
        i++;
        *br += 1;
        if(p->tail == PSIZE) {p->tail = 0;}
    }
    return i;
}

int write_bytes(PIPE *p, char *buf, int *bw, int n)
{
    int i = 0;
    while(*bw < n && p->room != 0)
    {
        //p->buf[(p->head)++] = buf[(*bw)++];
        p->buf[(p->head)++] = (char)get_byte(running->uss, buf+(*bw));
        (p->data)++;
        (p->room)--;
        i++;
        *bw += 1;
        if(p->head == PSIZE) {p->head = 0;}
    }
    return i;
}

int read_pipe(int fd, char *buf, int n)
{
  //
  int i, bytesRead;
  // your code for read_pipe()
  PIPE *p = running->fd[fd]->pipe_ptr;
  bytesRead = 0;
  while(1)
  {
    if(!p) {printf("ERROR: No pipe at given file descriptor\n"); return -1;}
  
    //1. If no writers on pipe read nbytes or as much data as available
    if(p->nwriter < 1)
    {
        read_bytes(p, buf, &bytesRead, n);
        return bytesRead;
    }

    //2. If pipe has data & writers read until nbytes, if not enough data go to 3
    if(p->data > 0)
    {
        printf("There's data, let's read!\n"); //FOR TESTING
        read_bytes(p, buf, &bytesRead, n);
        kwakeup(&p->room);
        if(bytesRead == n) {return bytesRead;}
    }
    //3. If no data in pipe wakeup writers waiting for room, wait for data, then back to step 1
    if(p->data == 0)
    {
        //Wakeup writers and wait for data
        kwakeup(&p->room);
        ksleep(&p->data);
    }
   }
   return -1;
}

int write_pipe(int fd, char *buf, int n)
{
  // your code for write_pipe()
  int bytesWritten = 0;
  PIPE *p = running->fd[fd]->pipe_ptr;
  printf("Going to write %d bytes to pipe with fd=%d\n", n, fd); //FOR TESTING
  while(1)
  {
    //1. If no readers return error
    if(p->nreader < 1) {printf("Error: No readers\n"); return BROKEN_PIPE_ERROR;}

    //2. If pipe has room write nbytes or until until data = PSIZE
    if(p->room != 0) //There is room
    {
        printf("There's room, let's write!\n"); //FOR TESTING
        write_bytes(p, buf, &bytesWritten, n);
        printf("We wrote %d bytes\n", bytesWritten);
        kwakeup(&p->data);
        if(bytesWritten == n) {return bytesWritten;}
    }
    if(p->room == 0)
    {
        //wakeup readers & wait
        kwakeup(&p->data);
        printf("No room, now we sleep\n");
        ksleep(&p->room);
    }
  }
  return -1;
}

int kpipe(int pd[2])
{
  // create a pipe; fill pd[0] pd[1] (in USER mode!!!) with descriptors
  int i1, j1, k, i2, j2, fd1, fd2;
  
  //Find 2 available fds in running proc
  for(j1 = 0; j1 < NFD; j1++)
  {
    if(running->fd[j1] == 0) {break;}
  }
  if(j1 == NFD) {return -1;}
  for(j2 = j1+1; j2 < NFD; j2++)
  {
    if(running->fd[j2] == 0) {break;}
  }
  if(j2 == NFD) {return -1;}

  //Find 2 availabe ofts in oft
  for(i1 =0; i1 < NOFT; i1++)
  {
    if(oft[i1].refCount == 0) {break;}
  }
  if(i1 == NOFT) { return -1; } //No fd available
  for(i2 = i1+1; i2 < NOFT; i2++)
  {
    if(oft[i2].refCount == 0) {break;}
  }
  if(i2 == NOFT) {return -1;}

  //Find 1 available pipe
  for(k = 0; k < NPIPE; k++)
  {
    if(pipe[k].busy == 0) {break;}
  }
  if(k == NPIPE) {return -1;}

  //Set necessary parameters for fds, ofts, & pipes
  oft[i1].refCount = 1;
  oft[i1].mode = READ_PIPE;
  oft[i1].pipe_ptr = &pipe[k];
  pipe[k].nreader = 1;
  pipe[k].nwriter = 1;
  pipe[k].busy = 1;
  pipe[k].head = pipe[k].tail = pipe[k].data = 0;
  pipe[k].room = PSIZE;
  running->fd[j1] = &oft[i1];
  fd1 = j1;

  oft[i2].refCount = 1;
  oft[i2].mode = WRITE_PIPE;
  oft[i2].pipe_ptr = &pipe[k];
  running->fd[j2] = &oft[i2];
  fd2 = j2;

  //Send fd1 and fd2 to usermode
  put_word(fd1, running->uss, &pd[0]);
  put_word(fd2, running->uss, &pd[1]);
  return;
  
}

int close_pipe(int fd)
{
  OFT *op; PIPE *pp;

  printf("proc %d close_pipe: fd=%d\n", running->pid, fd);

  op = running->fd[fd];
  running->fd[fd] = 0;                 // clear fd[fd] entry 

  //close READ_PIPE case
  if (op->mode == READ_PIPE){
      pp = op->pipe_ptr;
      pp->nreader--;                   // dec n reader by 1

      if (--op->refCount == 0){        // last reader
	if (pp->nwriter <= 0){         // no more writers
	     pp->busy = 0;             // free the pipe   
             return;
        }
      }
      kwakeup(&pp->room); 
      return;
  }
  //close WRITE_PIPE case
  if (op->mode == WRITE_PIPE) {
    pp = op->pipe_ptr;
    pp->nwriter--; //dec n writer by 1

    if (--op->refCount == 0) { //last writer
        if(pp->nreader <= 0) {//no more readers
            pp->busy = 0;
            return;
        }
    }
    kwakeup(&pp->data);
    return;
  }
  return -1;
}
