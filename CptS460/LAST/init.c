#include "ucode.c"
int console, console0, console1;

int parent()
{
  while(1)
  {
    int status;
    printf("HOINIT: wait for a ZOMBIE child\n");
    pid = wait(&status);

    if(pid == console) // if console login process died
    {
      printf("HOINIT: console 1 died, fork a new one\n");
      console = fork();  // fork another one

      if(!console)
      {
        exec("login /dev/tty0"); // new console login process
      }

      continue;
    }

    if(pid = console0)
      {
        printf("HOINIT: console 2 died, fork a new one\n");
        console0 = fork();  // fork another one

        if(!console0)
        {
          exec("login /dev/ttyS0"); // new console login process
        }

        continue;
      }

      if(pid = console1)
      {
        printf("HOINIT: console 3 died, fork a new one\n");
        console1 = fork();  // fork another one

        if(!console1)
        {
          exec("login /dev/ttyS1"); // new console login process
        }

        continue;
      }

    printf("HOINIT: I just buried an orphan child proc %d\n",pid);

  }
}


main()
{
  int in, out; // file descriptors for terminal I\O

  in = open("/dev/tty0",O_RDONLY); // file descriptor 0
  out = open("/dev/tty0",O_WRONLY); // for display to console

  printf("HOINIT: fork a login proc on console\n");
  console = fork();

  if(!console)
  {
    exec("login /dev/tty0");
  }


  printf("HOINIT: fork a login proc on console\n");
  console0 = fork();

  if(!console0)
  {
    exec("login /dev/ttyS0");
  }

  printf("HOINIT: fork a login proc on console\n");
  console1 = fork();

  if(!console1)
  {
    exec("login /dev/ttyS1");
  }


  parent();
  
}






