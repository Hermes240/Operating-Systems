/********** ls.c file *************/
#include "ucode.c"

void ls_file(char *filePath)
{
	char *t1 = "xwrxwrxwr-------";
	char *t2 = "----------------";
	
	STAT curFile;
	char buf[1024];
	int i;
	getBasename(buf, filePath);

	if (stat(filePath, &curFile) == 0)
	{
		//writing if its dir, reg, or link...
		if ((curFile.st_mode & 0xF000) == 0x8000) { printf("%c", '-'); }
		if ((curFile.st_mode & 0xF000) == 0x4000) { printf("%c", 'd'); }
		if ((curFile.st_mode & 0xF000) == 0xA000) { printf("%c", 'l'); }

		//writing the permission bits...
		for (i = 8; i >= 0; i--)
		{
			if (curFile.st_mode & (1 << i)) { printf("%c", t1[i]); }
			else { printf("%c", t2[i]); }
		}
		printf("  ");

		printf("%3d", curFile.st_nlink);
		printf("%3d", curFile.st_gid);
		printf("%3d", curFile.st_uid);
		printf("%6d", curFile.st_size);
		printf("%s", buf);

		//if its a link:
		if ((curFile.st_mode & 0xF000) == 0xA000)
		{
			if (readlink(filePath, buf) != -1)
			{
				printf(" -> %s", buf);
			}
			else
			{
				dup2(1, 4); dup2(2, 1); printf("ls: %s: could not read link\n", filePath); dup2(4, 1);
			}
		}
		printf("\n");
	}
	else
	{
		dup2(1, 4); dup2(2, 1); printf("ls: %s: does not exist\n", filePath); dup2(4, 1);
	}
}


int main(int argc, char *argv[ ])
{
	int i = 1;
	STAT curFile;
	char cwd[1024];

	/* no filenames were included with the call to ls so ls current working directory */
	if (argc == 1)
	{
		getcwd(cwd);
		//strcpy(argv[1], cwd);
		argv[1] = cwd;
		argv[2] = 0;
		argc++;
	}
	while (argv[i] != 0)
	{
		if (stat(argv[i], &curFile) == 0)
		{
			if (argc > 2)
			{
				if (i != 1)
				{
					printf("\n");
				}
				printf("%s:\n", argv[i]);
			}
			
			/* the file is directory so ls everything inside of it: */
			if ((curFile.st_mode & 0xF000) == 0x4000)
			{
				DIR *dp;
				char *cp;
				int j;
				char buf[BLKSIZE];
				char tmp[BLKSIZE];
				char curName[BLKSIZE];
				int fd;
				int size = curFile.st_size;

				fd = open(argv[i], O_RDONLY);
				while (size > 0)
				{
					read(fd, buf, BLKSIZE);
					size -= BLKSIZE;
					
					dp = (DIR *)buf;
					cp = buf;

					while (cp < buf + BLKSIZE)
					{
						for (j = 0; j < dp->name_len; j++)
						{
							curName[j] = dp->name[j];
						}
						curName[dp->name_len] = '\0';		//must add null terminator to end because there isnt one

						strcpy(tmp, argv[i]);
						strcpy(tmp + strlen(tmp), "/");
						strcpy(tmp + strlen(tmp), curName);

						ls_file(tmp);

						cp += dp->rec_len; // advance cp to the next entry (by rec_len)
						dp = (DIR *)cp;	//have the dir entry follow along
					}
				}
			}

			/* the file is just a file so ls it directly: */
			else
			{
				ls_file(argv[i]);
			}
		}
		else
		{
			dup2(1, 4); dup2(2, 1); printf("ls: %s: does not exist\n", argv[i]); dup2(4, 1);
		}
		i++;
	}
}
