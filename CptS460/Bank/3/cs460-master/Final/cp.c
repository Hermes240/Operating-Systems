#include "ucode.c"

int main(char *argc, char *argv[])
{
    write(2, "***************\n\r", 17);
    write(2, "*** cp time ***\n\r", 17);
    write(2, "***************\n\r", 17);

    cp_file(argv);
    exit(0);
}

int readLine(char *buf, int size, int file)
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
    if(c == '\n' || c == '\r') {buf[i++] = c;}
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

int cp_file(char *paths[])
{
    int fd = 0, gd = 0, n = 0, left = 0;
    struct stat fs;
    char buf[1024];

    fd = open(paths[1], O_RDONLY);
    if(fd < 0){ return -1;}
    fstat(fd, &fs);
    left = fs.st_size;
    gd = open(paths[2], O_WRONLY|O_CREAT);
    if(gd < 0) { close(fd); return -1; }
    reset(buf, 1024);
    while(left > 0)
    {
        if(left >= 1024)
        {
            n = readLine(buf, 1024, fd);
        }
        else
        {
            n = readLine(buf, left, fd);
        }
        write(gd, buf, n);
        reset(buf, 1024);
        left -= n;
    }
    close(fd);
    close(gd);
   return 1; 
}


