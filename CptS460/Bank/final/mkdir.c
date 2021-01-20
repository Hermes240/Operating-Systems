/********** mkdir.c file *************/
#include "ucode.c"

int main(int argc, char *argv[ ])
{
	int i = 1;
	char parentDir[256];
	STAT filestat;

	dup2(2, 1);		//the only time mkdir would output would be during an error
	if (argc == 1)
	{
		printf("mkdir: missing new directory operand\n");		//write to stderr that no directory to create was included was included
		printf("Try 'man mkdir' for more information.\n");
	}
	else
	{
		while (argv[i] != 0)
		{
			getParentpath(parentDir, argv[i]);

			/* check that the parent directory exists: */
			if (stat(parentDir, &filestat) == 0)
			{
				/* check that the parent directory is actually a direcory: */
				if ((filestat.st_mode & 0xF000) == 0x4000)
				{
					/* check that the requested new direcory doesnt already exist: */
					if (stat(argv[i], &filestat) != 0)
					{
						mkdir(argv[i]);
					}
					else
					{
						printf("mkdir: cannot create directory '%s': directory already exists\n", argv[i]);
					}
				}
				else
				{
					printf("mkdir: cannot create directory '%s': parent '%s' is not a directory\n", argv[i], parentDir);
				}
			}
			else
			{
				printf("mkdir: cannot create directory '%s': parent '%s' does not exist\n", argv[i], parentDir);
			}
			i++;
		}
	}
}
