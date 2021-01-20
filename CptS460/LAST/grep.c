/********** grep.c file *************/
#include "ucode.c"


 void search(char *string, char *temp,char *substring)
{

    for(;*string!='\0';string++)
    {

        char *source = string;
        char *destination;
        strncpy(destination,source,strlen(substring));

        if(strcmp(destination,substring) == 0)
        {
            printf("%s\n",temp);
            break;
           
        }
       
    }
       
}



int main(int argc, char *argv[ ])
{
	int i = 2;
	int in;
	char line[1024];
	char c;
	STAT tmpstat;

	if (argc == 1)
	{
		dup2(1, 4); dup2(2, 1);
		printf("grep: missing pattern operand\n");		//write to stderr that no manual page was included
		dup2(4, 1);
	}
	else
	{
		/* no filenames were included with the call to grep so read from standard in */
		if (argc == 2)
		{
			while (gets(line) != 0)
			{
                search(line,line, argv[1]);
                 
			}
		}
		/* iterate through all filenames included with the call to grep and grep each one of them */
		else
		{
			while (argv[i] != 0)
			{
				in = open(argv[i], O_RDONLY);		//read from file, it must exist

				/* current file does not exit, iterate to next filename: */
				if (in < 0)
				{
					dup2(1, 4); dup2(2, 1); printf("grep: %s: could not open file\n", argv[i]); dup2(4, 1);
				}

				/* current file exits, now grep it: */
				else
				{
                    dup2(in, 0);
                                   
					while (getline(line))
					{
                        line[strlen(line)] = '\0';
                        search(line,line, argv[1]);

                        memset(line,0,1024);
					}

					close(in);
				}
				i++;
			}
		}
	}
}

