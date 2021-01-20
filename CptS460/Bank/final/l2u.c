/********** l2u.c file *************/
#include "ucode.c"

void toUpper(char *str)
{
	int i = 0;
	while (str[i])
	{
		/* check if current char is lowercase: */
		if (str[i] > 96 && str[i] < 123)
			str[i] -= 32;
		i++;
	}
}

int main(int argc, char *argv[ ])
{
	int in, out;
	char line[256];

	if (!((argc == 1) || (argc == 3)))
	{
		dup2(2, 1);
		printf("l2u: incorrect amount of file operands\n");		//write to stderr thatthere was incorrect input to l2u
		printf("Try 'man l2u' for more information.\n");
	}
	else
	{
		/* nothing passed to l2u so use standarn in for source file and standard out for destination file: */
		if (argc != 1)
		{
			in = open(argv[1], O_RDONLY);		//read from file, it must exist
			out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);		//write to file

			dup2(in, 0);
			dup2(out, 1);

			if (in < 0)
			{
				dup2(2, 1); printf("l2u: %s: could not open file\n", argv[1]); return 0;
			}
			else if (out < 0)
			{
				dup2(2, 1); printf("l2u: %s: could not open file\n", argv[2]); return 0;
			}
		}

		while (get2s(line) != 0)
		{
			toUpper(line);
			printf("%s", line);
		}
	}
}
