/********** unlink.c file *************/
#include "ucode.c"

int main(int argc, char *argv[ ])
{
	int i = 1;
	STAT curFile;

	dup2(2, 1);		//the only time rmdir would output would be during an error
	if (argc == 1)
	{
		printf("unlink: missing removing file operand\n");		//write to stderr that no directory to create was included was included
		printf("Try 'man unlink' for more information.\n");
	}
	else
	{
		while (argv[i] != 0)
		{
			if (stat(argv[i], &curFile) == 0)
			{
				if ((curFile.st_mode & 0xF000) == 0x8000)
				{
					if (unlink(argv[i]) == -1)
					{
						printf("unlink: cannot remove file '%s': incorrect permissions\n", argv[i]);
					}
				}
				else
				{
					printf("unlink: cannot remove file '%s': not a regular file\n", argv[i]);
					return 0;
				}
			}
			else
			{
				printf("unlink: cannot remove file %s: does not exist\n", argv[i]);
				return 0;
			}
			i++;
		}
	}
}
