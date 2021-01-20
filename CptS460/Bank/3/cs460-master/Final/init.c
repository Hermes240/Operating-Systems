#include "ucode.c"

int pid, child1, child2, child3, status;
int stdin, stdout;

int main(int argc, char *argv[])
{
    // open /dev/tty0 as 0 (read) & 1 (write) in order to display messages
    stdin = open("/dev/tty0", O_RDONLY);
    stdout = open("/dev/tty0", O_WRONLY);
    //stderr = open("/dev/tty0", O_WRONLY);

    printf("INIT : fork a login task on console\n");
    child1 = fork();
    if(!child1){ login("login /dev/tty0"); }
    
    /*child2 = fork();
    if(!child2) { pause(1); login("login /dev/ttyS0"); }
    
    child3 = fork();
    if(!child3) { login("login /dev/ttyS1"); }*/
    parent();
}

int login(char *s)
{
    exec(s);
}

int parent()
{
    while(1) 
    {
        printf("KCINIT : waiting ....\n");

        pid = wait(&status);

        if(pid == child1)
        {
            child1 = fork();
            if(!child1) {login("login /dev/tty0");}
        }
        else if(pid == child2)
        {
            child2 = fork();
            if(!child2) {login("login /dev/ttyS0");}
        }
        else if(pid == child3)
        {
            child3 = fork();
            if(!child3) {login("login /dev/ttyS1");}
        }
        else
            printf("INIT : buried an orphan child %d\n", pid);
    }
}
