/********** grep.c file *************/
#include "ucode.c"

char * search(char *string, char *substring)
{
	char *a, *b;
	b = substring;
	if (*b == 0) {
		return string;
	}
	for (; *string != 0; string += 1) {
		if (*string != *b) {
			continue;
		}
		a = string;
		while (1) {
			if (*b == 0) {
				return string;
			}
			if (*a++ != *b++) {
				break;
			}
		}
		b = substring;
	}
	return (char *)0;
}

int main(int argc, char *argv[ ])
{
	int i = 2;
	int in;
	char line[256];
	char c;
	STAT tmpstat;

	if (argc == 1)
	{
		dup2(1, 4); dup2(2, 1);
		printf("grep: missing pattern operand\n");		//write to stderr that no manual page was included
		printf("Try 'man grep' for more information.\n");
		dup2(4, 1);
	}
	else
	{
		/* no filenames were included with the call to grep so read from standard in */
		if (argc == 2)
		{
			while (get2s(line) != 0)
			{
				if (search(line, argv[1]) != 0)
				{
					printf("%s", line);
				}
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
					dup2(1, 4); dup2(2, 1); printf("cat: %s: could not open file\n", argv[i]); dup2(4, 1);
				}

				/* current file exits, now grep it: */
				else
				{
					dup2(in, 0);
					while (get2s(line) != 0)
					{
						if (search(line, argv[1]) != 0)
						{
							printf("%s", line);
						}
					}
					close(in);
				}
				i++;
			}
		}
	}
}
