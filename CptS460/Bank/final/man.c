/********** man.c file *************/
#include "ucode.c"

char *manPages[32];
void (*functions[64])();

void man_rmdir()
{
	printf("Usage: rmdir DIRECTORY...\n");
	printf("Remove the DIRECTORY(ies), if they exist and are empty.\n");
}

void man_mkdir()
{
	printf("Usage: mkdir DIRECTORY...\n");
	printf("Create the DIRECTORY(ies), if they do not already exist.\n");
}

void man_cd()
{
	printf("Usage: cd [DIR]\n");
	printf("Change current directory to DIR (directory '/' used by default).\n");
}

void man_l2u()
{
	printf("Usage: l2u [SOURCE DEST]\n");
	printf("Convert letters from SOURCE to uppercase and output to DEST (standard input and output used by default).\n");
}

void man_grep()
{
	printf("Usage: grep PATTERN [FILE]...\n");
	printf("Display every line of FILE(s) that contains PATTERN.\n");
}

void man_more()
{
	printf("Usage: more [FILE]...\n");
	printf("Display FILE(s) one page at a time.\n");
}

void man_cat()
{
	printf("Usage: cat [FILE]...\n");
	printf("Display FILE(s).\n");
}

void man_ls()
{
	printf("Usage: ls [FILE]...\n");
	printf("List information about the FILE(s) (the current directory by default).\n");
}

void man_man()
{
	printf("Usage: man PAGE...\n");
	printf("Display helpful information about a user program(s) (PAGE).\n");
}

void help()
{
	int i = 0;
	printf("Available commands:\n");
	while (manPages[i] != 0)
	{
		printf("\n%s:\n", manPages[i]);
		(*functions[i])();
		i++;
	}
}

//return index of man page for function pointer, -1 if not found
int searchManPages(char *inputPage)
{
	int i = 0;
	while (manPages[i] != 0)
	{
		if (!(strcmp(inputPage, manPages[i])))
			return i;
		i++;
	}
	return -1;
}

void init()
{
	tokenize("man ls cat more grep l2u cd mkdir rmdir", manPages);

	functions[0] = man_man;
	functions[1] = man_ls;
	functions[2] = man_cat;
	functions[3] = man_more;
	functions[4] = man_grep;
	functions[5] = man_l2u;
	functions[6] = man_cd;
	functions[7] = man_mkdir;
	functions[8] = man_rmdir;
}

int main(int argc, char *argv[ ])
{
	int i = 1;
	int k;

	init();
	
	if (argc == 1)
	{
		dup2(1, 4); dup2(2, 1);
		printf("man: missing manual page operand\n");		//write to stderr that no manual page was included
		printf("Try 'man man' for more information.\n");
		dup2(4, 1);
	}
	else
	{
		if (strcmp(argv[1], "help") == 0)
		{
			help(); return 0;
		}
		while (argv[i] != 0)
		{
			if ((k = searchManPages(argv[i])) != -1)
			{
				if (argc > 2)
				{
					if (i != 1)
					{
						printf("\n");
					}
					printf("%s:\n", argv[i]);
				}

				(*functions[k])();		//function pointer to the desired manual page
			}
			else
			{
				dup2(1, 4); dup2(2, 1); printf("man: %s: manual page not implemented\n", argv[i]); dup2(4, 1);
			}
			i++;
		}
	}
}
