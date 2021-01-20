/********** symlink.c file *************/
#include "ucode.c"

int main(int argc, char *argv[ ])
{
	int in, out;
	char line[256];
	STAT inFile;
	STAT outFile;

	dup2(2, 1);		//the only time symlink would output would be during an error

	if (!(argc == 3))
	{
		printf("symlink: incorrect amount of file operands\n");		//write to stderr thatthere was incorrect input to syslink
		printf("Try 'man symlink' for more information.\n");
	}
	else
	{
		if (stat(argv[1], &inFile) != 0)
		{
			printf("symlink: cannot link from '%s': does not exist\n", argv[1]);
			return 0;
		}
		if (stat(argv[2], &outFile) == 0)
		{
			printf("symlink: cannot link to '%s': already exists\n", argv[2]);
			return 0;
		}

		//if (argv[2][0] != '/')
		//{
		//	strcpy(line, (argv[2]));
		//	getcwd(argv[2]);
		//	strcpy(argv[2] + strlen(argv[2]), line);
		//}
		//in = open(argv[2], O_RDONLY | O_CREAT);
		//close(in);
		//stat(argv[2], &outFile);
		//outFile.st_mode = (outFile.st_mode & 0x0FFF) | 0xA000;		//change to LNK type
		//outFile.st_mode = 0xF000;
		//printf("argv[2] = %s\n", argv[2]);
		//getcwd(line);
		symlink(argv[1], argv[2]);
		//chdir(line);
	}
}
