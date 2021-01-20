#include "ucode.c"

int main(char *argc, char *argv[])
{
    char l[1025];
    int len = 0;
    struct stat fs;
    int fdin, fdout, left, eof = 0, n = 0;
    write(2, "************************\n\r", 26);
    write(2, "*** lower2upper time ***\n\r", 26);
    write(2, "************************\n\r", 26);

    //getc();
    if(argc == 1)
    {
        fstat(0, &fs);
        if(fs.st_mode & 0010000) //FIFO (it's a pipe!)
        {
            while(n = readLine(l, 256, 0, &eof))
            {
                if(l[0] == '~') {exit(0);}
                l2u(l);
                write(1, l, n);
                //putc('\n');
                putc('\r');
                reset(l, 256);
                if(eof) {putc(0); break;}
            }
        }
        else
        {
            while(gets(l))
            {
                if(l[0] == '~') {exit(0);}
                l2u(l);
                printf("%s\n", l);
                reset(l, 256);
            }
        }
    }
    else 
    { 
     
        fdin = open(argv[1], O_RDONLY);
        if(fdin < 0) { return -1; }
        if(argc == 3) {fdout = open(argv[2], O_WRONLY|O_CREAT); }
        else { fdout = 1; }
        if(fdout < 0) {close(fdin); return -1; }
        //printf("All files open\n"); //FOR TESTING

        fstat(fdin, &fs);
        left = fs.st_size;
        reset(l, 1025);
        while(left > 0)
        {
            if(left >= 1024)
            {
                n = readLine(l, 1024, fdin, &eof);
            }
            else
            {
                n = readLine(l, left, fdin, &eof);
            }
            //printf("Just read %d bytes: %s\n", n, l);
            l[n+1] = 0;
            l2u(l);
            write(fdout, l, n);
            if(argc < 3) {putc('\r');}
            //printf("Just wrote %d bytes: %s\n", n, l); //FOR TESTING
            reset(l, 1025);
            left -= n;
        }
        close(fdin);
        if(argc == 3) {close(fdout);}
    }
    //printf("\n");
    exit(0);
}

int readLine(char *buf, int size, int file, int *eof)
{
    int i = 0;
    char c;
    read(file, &c, 1);
    while(c != '\n' && c != 0 && i < size)
    {
        buf[i++] = c;
        read(file, &c, 1);
        //putc(c); //FOR TESTING
    }
    if(c == '\n' || c == '\r') {buf[i++] = '\n';}
    if(c == 0) { *eof = 1; }
    return i;
}

int reset(char *buf, int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        buf[i] = 0;
    }
}

int l2u(char *buf)
{
    int i, len;

    len = strlen(buf);
    for(i = 0; i < len; i++)
    {
        if (((int)buf[i] > 96) && ((int)buf[i] < 123)) //lower case
        buf[i] -= 32;
    }
    return 1;
}

