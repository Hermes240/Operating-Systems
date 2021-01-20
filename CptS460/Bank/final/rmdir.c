/********** rmdir.c file *************/
#include "ucode.c"

int isDirEmpty(char *filepath)
{
	DIR *dp;
	char *cp;
	int j = 0; 
	int size, fd;
	char buf[BLKSIZE];
	STAT curFile;

	if (stat(filepath, &curFile) == 0)
	{
		if ((curFile.st_mode & 0xF000) == 0x4000)
		{
			size = curFile.st_size;

			fd = open(filepath, O_RDONLY);
			while (size > 0)
			{
				read(fd, buf, BLKSIZE);
				size -= BLKSIZE;

				dp = (DIR *)buf;
				cp = buf;

				while (cp < buf + BLKSIZE)
				{
					j++;
					cp += dp->rec_len; // advance cp to the next entry (by rec_len)
					dp = (DIR *)cp;	//have the dir entry follow along

					if (j > 2)
					{
						printf("rmdir: cannot remove directory '%s': not empty\n", filepath);
						return 0;
					}
				}
				return 1;
			}
		}
		else
		{
			printf("rmdir: cannot remove directory '%s': not a directory\n", filepath);
			return 0;
		}
	}
	else
	{
		printf("rmdir: cannot remove directory %s: does not exist\n", filepath);
		return 0;
	}
}

int main(int argc, char *argv[ ])
{
	int i = 1;
	char parentDir[256];
	STAT filestat;

	dup2(2, 1);		//the only time rmdir would output would be during an error
	if (argc == 1)
	{
		printf("rmdir: missing removing directory operand\n");		//write to stderr that no directory to create was included was included
		printf("Try 'man rmdir' for more information.\n");
	}
	else
	{
		while (argv[i] != 0)
		{
			if (isDirEmpty(argv[i]) == 1)
			{
				//rmdir(argv[i]);
				if (rmdir(argv[i]) == -1)
				{
					printf("rmdir: cannot remove directory '%s': currently busy\n", argv[i]);
				}
			}
			i++;
		}
	}
}
