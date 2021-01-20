/********** login.c file *************/
// login.c : Upon entry, argv[0]=login, argv[1]=/dev/ttyX
//a login function which asks user for name and password and if combo in /etc/psswd file, continue

#include "ucode.c"

//a function which will save the string from the startIndex to the next ':' char
int getPeice(int startIndex, char *buf, char *line)
{
	int i = 0;
	while (line[startIndex] != ':')
	{
		buf[i++] = line[startIndex++];
	}
	buf[i] = '\0';
	return startIndex + 1;
}

void getData(char *curName, char *curPassword, int *uid, int *gid, char *line)
{
	char suid[64], sgid[64];
	int startIndex = 0;

	startIndex = getPeice(startIndex, curName, line);		//get the name at the current line of password file
	startIndex = getPeice(startIndex, curPassword, line);	//get the password at the current line of password file
	startIndex = getPeice(startIndex, suid, line);			//get the uid at the current line of password file
	getPeice(startIndex, sgid, line);						//get the gid at the current line of password file

	*uid = atoi(suid);	//convert string representation of uid to int
	*gid = atoi(sgid);	//convert string representation of gid to int
}

int main(int argc, char *argv[ ])
{
	int in, out, err, passFile, uid, gid;
	char name[64], password[64], curName[64], curPassword[64], line[264];
	char curChar;

	/* close file descriptors 0,1 inherited from INIT: */
	close(0);
	close(1);

	/*  open argv[1] 3 times as in(0), out(1), err(2): */
	in = open(argv[1], O_RDONLY);
	out = open(argv[1], O_WRONLY);
	err = open(argv[1], O_WRONLY);

	settty(argv[1]); // set tty name string in PROC.tty

	//passFile = open("/etc/passwd", O_RDONLY);	// open /etc/passwd file for READ

	while (1)
	{
		passFile = open("/etc/passwd", O_RDONLY);	// open /etc/passwd file for READ

		/* ask for username and password: */
		printf("login:"); gets(name);
		printf("password:"); gets(password);

		while (readLine(passFile, line) != 0)	//Iterate through password file line by line
		{
			getData(curName, curPassword, &gid, &uid, line);	//parse data of current line in password file to these variables

			if ((strcmp(name, curName) == 0) && (strcmp(password, curPassword) == 0))	//user has valid account (username and password matched)
			{
				printf("Welcome back, %s\n", name);
				chuid(uid, gid);	//change uid, gid to user's uid, gid;

				if (uid == 0)	//root directory is / everyone else has own directory under /user
				{
					chdir("/");		//user is root so change directory to /
				}
				else
				{
					strcpy(line, "/user/");
					strcpy(line + strlen(line), name);	//concatonate the users name to "/user/"
					chdir(line);	//change directory to user's home directory
				}

				strcpy(line, "sh ");
				strcpy(line + strlen(line), name);	//concatonate sh with username to send to sh program

				close(passFile);	//close opened /etc/passwd file
				exec(line);		// exec to sh program in user account
			}
		}

		printf("Incorrect credentials, please try again\n");
		lseek(passFile, 0, 0);
	}
}
