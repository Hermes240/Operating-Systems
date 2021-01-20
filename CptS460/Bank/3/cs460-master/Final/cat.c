#include "ucode.c"

#define BUF_SIZE 69

int main(argc, argv) int argc; char* argv[];
{
	// expecting: argv[0] "cat", argv[1] is path/to/file
    char l[256];
    struct stat fs;
    int n = 0, eof = 0;
	
	write(2, "****************\n\r", 18);
    write(2, "*** cat time ***\n\r", 18);
    write(2, "****************\n\r", 18);

    reset(l, 256);

	if (argc == 1)
	{
        fstat(0, &fs);
        if(fs.st_mode & 0010000) //FIFO (it's a pipe!)
        {
            while(n = readLine(l, 256, 0, &eof))
            {
                write(1, l, n);
                putc('\r');
                reset(l, 256);
                if(eof) {putc(0); break;}
            }
        }
        else
        {
            while(gets(l))
            {
                if(l[0] == '~') {break;}
                printf("%s\n", l);
                reset(l, 256);
            }
        }
		// interactive mode
        /*i = 0;
		while(1)
		{
			char c = getc();
            buf[i++] = c;

			switch(c)
			{
				case '\r': 
                case '\n':
                    buf[i++] = '\r'; buf[i++] = '\n'; //putc('\n');  putc('\r'); 
                    write(1, buf, i);

                    i = 0;
                    break;
				case '~' : exit(0);		break;
                case (char)EOF: exit(0); break;
				default  : write(1, &c, 1);  	break;
			}
		}*/
	
	} else {
		int i = 0;
		// can cat a list of files

		for(i = 1; i < argc; i++)
		{
			char buf[BUF_SIZE];
			struct stat info;
			int bytes_left;
			
			int fd = open(argv[i], 0);

			//printf("fd=%d\n", fd);

			stat(argv[1], &info);
			bytes_left = info.st_size;

			while(bytes_left > 0)
			{
				int j = 0;
				int read_size = BUF_SIZE;

				if (bytes_left < BUF_SIZE)
					read_size = bytes_left;

				read(fd, buf, read_size);

				for(j = 0; j < read_size; j++)
				{
					char c = buf[j];
					switch(c)
					{
						case '\n': putc('\n');  putc('\r');	break;
						default  : putc(c);  	break;
					}
				}

				bytes_left -= read_size;
			}
            putc(0);
			close(fd);
		}
	}
    exit(0);
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

