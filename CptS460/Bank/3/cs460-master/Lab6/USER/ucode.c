// ucode.c file

char *cmd[]={"getpid", "ps", "chname", "kmode", "switch", "wait", "exit", "fork", "exec", "pipe", "pfd", "read", "write", "close", 0};

#define LEN 64

int show_menu()
{
   printf("*********************** Menu **********************\n");
   printf("*  ps  chname  kmode switch  wait  exit fork exec *\n");
   printf("*  pipe  pfd  read  write  close                  *\n");
   printf("***************************************************\n");
}

int find_cmd(char *name)
{
   int i=0;   
   char *p=cmd[0];

   while (p){
         if (strcmp(p, name)==0)
            return i;  
         p = cmd[++i];
   } 
   return(-1);
}

int getpid()
{
   return syscall(0,0,0);
}

int ps()
{
   return syscall(1, 0, 0);
}

int chname()
{
    char s[64];
    printf("\ninput new name : ");
    gets(s);
    return syscall(2, s, 0);
}

int kmode()
{
    printf("kmode : enter Kmode via INT 80\n");
    printf("proc %d going to Kmode ....\n", getpid());
    syscall(3, 0, 0);
    printf("proc %d back from Kernel\n", getpid());
}

int fork()
{   
  int child;
  child = syscall(7, 0, 0);
  if(child) {printf("proc %d kforked a child %d\n", getpid(), child);}
  else {printf("child %d return fromfork, child=%d\n", getpid(), child);}
}    

int kswitch()
{
    printf("proc %d enter Kernel to switch proc\n", getpid());
    syscall(4,0,0);
    printf("proc %d back from Kernel\n", getpid());
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n");
    return child;
} 

int geti()
{
  char s[16];
  return atoi(gets(s));
}

int exec() 
{
    int r;
    char filename[64];
    printf("Enter exec filename: ");
    gets(filename);
    r = syscall(8, filename, 0);
    printf("exec failed\n");
}

int _kexit(int exitValue)
{
  return syscall(6,exitValue,0);
}

int exit()
{
   int exitValue;
   printf("enter exitValue (0-9): ");
   exitValue = geti();
   printf("exitvalue=%d\n", exitValue);
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   _kexit(exitValue);
}

int getc()
{
  return syscall(90,0,0) & 0x7F;
}

int putc(char c)
{
  return syscall(91,c,0,0);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}

int pd[2];

int pipe()
{
    printf("pipe syscall\n");
    syscall(30, pd, 0);
    printf("proc %d created a pipe with fd = %d %d\n", getpid(), pd[0], pd[1]);
}

int pfd()
{
    syscall(34,0,0,0);
}

void show_pipe()
{
    int i;
    pfd();
    printf("Which pipe: ");
    i = atoi(gets());
    syscall(35, i);
}

int read_pipe()
{
    char fds[32], buf[1024];
    int fd, n, nbytes;
    pfd();

    printf("Read: enter fd:");
    fd = atoi(gets(fds));
    printf("nbytes: ");
    nbytes = atoi(gets(fds));
    printf("rd=%d nbytes=%d\n", fd, nbytes);
    n = syscall(31, fd, buf, nbytes);

    if (n>=0) {
        printf("proc %d back to Umode, read %d bytes from pipe : ", getpid(), n);
        buf[n]=0;
        printf("%s\n", buf);
    }
    else { printf("read pipe failed\n"); }
}

int write_pipe()
{
    char fds[16], buf[1024];
    int fd, n, nbytes;
    pfd();
    printf("write : enter fd: ");
    fd = atoi(gets(fds));
    printf("String: ");
    gets(buf);
    nbytes = strlen(buf);

    printf("fd=%d nbytes=%d : %s\n", fd, nbytes, buf);

    n = syscall(32,fd,buf,nbytes);
    
    if (n>=0) {
        printf("\nproc %d back to Umode, wrote %d bytes to pipe\n", getpid(), n);
    }
    else { printf("write pipe failed\n"); }
}

int close_pipe()
{
    char s[16];
    int fd; 
    printf("enter fd to close : ");
    gets(s);
    fd = atoi(s);
    syscall(33, fd);
}
