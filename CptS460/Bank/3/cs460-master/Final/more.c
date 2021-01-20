#include "ucode.c"

int main(char *argc, char *argv[])
{
    char l[256], c;
    int count = 0, fd = 0, n = 0, eof = 0;
    write(2, "*****************\n\r", 19);
    write(2, "*** more time ***\n\r", 19);
    write(2, "*****************\n\r", 19);

    reset(l, 256);
    //getc();
    if(argc == 1)
    {
        fd = open("/dev/tty0", O_RDONLY);
        while(n = readLine(l, 256, 0, &eof)) //gets(l))
        {
             if(l[0] == '~') {exit(0);}
             write(1, l, n);
             putc('\r');
             reset(l, 256);
             count++;
             if(count > 22)
             {
                read(fd, &c, 1);
                count = 0;
             }
             if(eof) { break;}
        }
        close(fd);
    }
    else { more(argv[1]); }
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
    if(c == '\n' || c == '\r') {buf[i++] = c;}
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

int more(char *pathname)
{
    char mybuf[1025], c;
    struct stat fs;
    int eof = 0, fd = 0, left = 0, lines = 0, tot = 0, n = 0;
    reset(mybuf[1025]);
    
    if(!pathname) { fd = 0; }
    else { fd = open(pathname, O_RDONLY); }
    if(fd < 0) { return -1; }
    //printf("going to stat file now\n");
    fstat(fd, &fs);
    left = fs.st_size;
    //printf("File size: %d\n", left);
    while(left > 0)
    {
        lines = 0;
        tot = 0;
        while(lines < 23 && left > 0) //&& tot < 1920)
        {
            if(left >= 1024) //&& (tot + 1024) <= 1920)
            {
                n = readLine(mybuf, 1024, fd, &eof);
            }
            else //if(left + tot <= 1920)
            {
                //printf("reading %d bytes or less\n", left);
                n = readLine(mybuf, left, fd, &eof);
            }
            /*else
            {
                n = readLine(mybuf, 1920 - tot, fd, &eof);
            }*/
            mybuf[n] = 0;
            write(1, mybuf, n);
            putc('\r');
            //printf("%s\r", mybuf);
            reset(mybuf, 1025);
            left -= n;
            tot += n;
            lines++;
            if(eof) { left = 0; break;}
        }
        if(left > 0) {getc(&c); }
    }
    if(pathname) {close(fd);}
    return 1;
}

