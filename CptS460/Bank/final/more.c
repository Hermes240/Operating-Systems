/********** more.c file *************/
//exactly like cat but display 15 lines at a time, press a key to show next 15 lines
#include "ucode.c"

#define LINES 20

int main(int argc, char *argv[ ])
{
	int i = 1;
	int curLineNum = 1;
	int in, r;
	char line[256];
	char c;
	STAT outLocation;

	fstat(1, &outLocation);

	/* no filenames were included with the call to more so read from standard in exactly like cat: */
	if (argc == 1)
	{
		char tyy[256];
		gettty(tyy);
		close(2);
		in = open(tyy, O_RDONLY);

		while (get2s(line) != 0)
		{
			printf("%s", line);
			if (S_ISCHR(outLocation.st_mode))
			{
				curLineNum++;

				if (curLineNum % LINES == 0)
				{
					read(2, &c, 1);
					curLineNum = 1;
				}
			}
		}
	}
	/* iterate through all filenames included with the call to more and more each one of them */
	else
	{
		while (argv[i] != 0)
		{
			in = open(argv[i], O_RDONLY);		//read from file, it must exist

			/* current file does not exit, iterate to next filename: */
			if (in < 0)
			{
				dup2(1, 4); dup2(2, 1); printf("more: %s: could not open file\n", argv[i]); dup2(4, 1);
			}

			/* current file exits, now more it if outputting to screen, otherwise do exactly like cat: */
			else
			{
				dup2(in, 0);
				while (get2s(line) != 0)
				{
					printf("%s", line);
					if (S_ISCHR(outLocation.st_mode))
					{
						curLineNum++;

						/* if it is the 15th line, wait for user input then continue: */
						if (curLineNum % LINES == 0)
						{
							getc();
							curLineNum = 1;
						}
					}
				}
				close(in);
			}
			i++;
		}
	}
}
