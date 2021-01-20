/********** cat.c file *************/
#include "ucode.c"

int main(int argc, char *argv[ ])
{
	int i = 1;
	int in;
	char line[1024];
	memset(line,0,1024);
	

	/* no filenames were included with the call to cat so read from standard in */
	if (argc == 1)
	{
		while (gets(line) != 0)
		{
			printf("%s\n", line);
		}
	}

	// iterate through all filenames included with the call to cat and cat each one of them 
	else
	{
		while (argv[i] != 0)
		{
			in = open(argv[i], O_RDONLY);		//read from file, it must exist
			
			// current file does not exit, iterate to next filename: 
			if (in < 0)
			{
				dup2(1, 4); dup2(2, 1); printf("cat: %s: could not open file\n", argv[i]); dup2(4, 1);
			}

			/* current file exits, now cat it: */
			else
			{
				dup2(in, 0);
				while (read(in,line,1024) != 0)
				{
					write(1,line,1024);
					//printf("%s", line);
				}
				close(in);
			}
			i++;
		}
	}
}