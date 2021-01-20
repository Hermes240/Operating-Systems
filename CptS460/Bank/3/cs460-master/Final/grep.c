#include "ucode.c"

int main(char *argc, char *argv[])
{
    char l[1025], c = 0;
    int fd = 0, left = 0, n = 0, eof = 0;
    struct stat fs;
    write(2, "*****************\n\r", 19);
    write(2, "*** grep time ***\r\n", 19);
    write(2, "*****************\n\r", 19);

    reset(l, 1025);
    //getc();
    if(argc == 2)
    {
        fstat(0, &fs);
        if((fs.st_mode & 0010000)) //FIFO (it's a pipe!)
        {
            while(n = readLine(l, 256, 0, &eof))
            {

                if(l[0] == '~') {exit(0);}
                if(grep(l, argv[1]))
                {
                    write(1, l, n);
                    putc('\r');
                }
                reset(l, 1025);
                if(eof) {putc(0); break;}
            }
        }
        else
        {
            while(gets(l))
            {
                if(l[0] == '~') {exit(0);}
                if(grep(l, argv[1]))
                {
                    printf("%s\n", l);
                }
                reset(l, 256);
            }
        }
    }
    else 
    { 
        fd = open(argv[2], O_RDONLY); 
        if(fd < 0) { return -1; }
        fstat(fd, &fs);
        left = fs.st_size;
        while(left > 0)
        {
            if(left >= 1024)
            {
                n = readLine(l, 1024, fd, &eof);
            }
            else
            {
                    n = readLine(l, left, fd, &eof);
            }
            l[n+1] = 0;
            if(grep(l, argv[1]))
            {
                write(1, l, n);
                putc('\r');
            }
            reset(l, 1025);
            left -= n;
        }
        putc(0);
        close(fd);
    }
    //printf("\n");
    exit(1);
}

int readLine(char *buf, int size, int file, int *eof)
{
    int i = 0;
    char c = 0;
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
    int i = 0;
    for(i = 0; i < size; i++)
    {
        buf[i] = 0;
    }
}

int inStr(char *pattern, char *buf)
{
    int i, j, blen, plen;

    plen = strlen(pattern);
    blen = strlen(buf);
    i = 0; j = 0;
    
    while(i < blen - plen)
    {
        if(strncmp(buf+i, pattern, plen) == 0)
        {
            return 1;
        }
        i++;
    }
    return 0;
}

int grep(char *l, char *pattern)
{
    return inStr(pattern, l);
}
