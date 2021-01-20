/********** sh.c file *************/
// sh.c : Upon entry, argv[0]=sh, argv[1]=username

#include "ucode.c"

char CWD[256];
char inputLine[256];

void exec_cmd(char *line)
{
	int io, i;
	char cmd[128];
	char tmp[256];
	char *toks[32];
	STAT filestat;

	tokenize(line, toks);
	strcpy(cmd, toks[0]);

	/* create command string and assign stdin and stdout as necessary: */

	i = 1;
	while (toks[i] != 0)	//iterate through all tokens
	{
		/* if a token is >, >>, or <, then it is an indicator is IO redirection so assign following token to appropriate file descriptor:  */
		if (strcmp(toks[i], ">") == 0 || strcmp(toks[i], ">>") == 0 || strcmp(toks[i], "<") == 0)
		{
			/* make sure that the following token exists and is not either >, >>, <, or |: */
			if (toks[i + 1] != 0 && !(strcmp(toks[i + 1], ">") == 0 || strcmp(toks[i + 1], ">>") == 0 || strcmp(toks[i + 1], "<") == 0))
			{
				if (strcmp(toks[i], ">") == 0)
				{
					close(1);	//close stdout for output redirection
					io = open(toks[i + 1], O_WRONLY | O_CREAT | O_TRUNC);		//write to file, create if doesnt exist
				}
				else if (strcmp(toks[i], ">>") == 0)
				{
					close(1);	//close stdout for output redirection
					io = open(toks[i + 1], O_WRONLY | O_CREAT | O_APPEND);		//append write to file, create if doesnt exist
				}
				else
				{
					close(0);	//close stdin for input redirection
					io = open(toks[i + 1], O_RDONLY);		//read from file, it must exist
				}

				if (io < 0)
				{
					dup2(2, 1); printf("sh: %s: could not open file\n", toks[i + 1]); return;	//write to stderr that it could not open file
				}

				i += 2;
			}
			else
			{
				dup2(2, 1); printf("sh: invalid syntax\n"); return;	//write to stderr that there was invalid syntax
			}
		}

		/* if current tok was not an IO redirection indicator then append token to cmd: */
		else
		{
			strcpy(cmd + strlen(cmd), " ");
			strcpy(cmd + strlen(cmd), toks[i]);
			i++;
		}
	}

	/* command string has been built, now execute it, if it doesnt continue from execution, then command was not found */
	strcpy(tmp, "/bin/");
	strcpy(tmp + strlen(tmp), toks[0]);
	if (stat(tmp, &filestat) == 0)
	{
		exec(cmd);	//execute the command
		dup2(2, 1); printf("sh: %s: could not execute command\n", toks[0]);
	}
	else
	{
		dup2(2, 1); printf("sh: %s: command not found\n", toks[0]);
	}
}

//this is ran by a child process
void analyze(char *line)
{
	int io, i, k, m, n;
	char cmd[128];
	char *toks[32];
	int pid, pd[2];

	char left[128];
	char right[128];

	int len = strlen(line);
	for (k = len - 1; k >= 0; k--)
	{
		/* current char is | indicating a pipe: */
		if (line[k] == '|')
		{
			for (m = 0; m + k + 1 < len; m++)
				right[m] = line[m + k + 1];
				//printf("rc: %c\n", right[m]);
			for (m = 0; m < k; m++)
				left[m] = line[m];
			right[len] = 0;
			left[k] = 0;

			pipe(pd); // create a pipe: pd[0]=READ, pd[1]=WRITE
			pid = fork(); // fork a child to share the pipe
			if (pid) 
			{	// parent: as pipe READER
				close(pd[1]); // close pipe WRITE end
				dup2(pd[0], 0); // redirect stdin to pipe READ end
				exec_cmd(right);
			}
			else 
			{	// child : as pipe WRITER
				close(pd[0]); // close pipe READ end
				dup2(pd[1], 1); // redirect stdout to pipe WRITE end
				analyze(left);
			}
		}
	}
	// we got here meaning we didnt find a pipe so send line to exec_cmd
	exec_cmd(line);
}

void run_cmd(char *line)
{
	int pid, status;
	pid = fork();

	if (pid)
	{
		//parent
		pid = wait(&status);
	}
	else
	{
		//child
		analyze(line);
	}
}

int main(int argc, char *argv[ ])
{
	STAT tmpstat;
	char tmpline[256];
	char *inputs[32];
	int len;

	while (1)
	{
		getcwd(CWD);
		printf("%s@proc-%d:%s$ ", argv[1], getpid(), CWD);

		/* if the user typed "logout" or pressed ctrl+D, kill the process. the parent in the init function will then fork another login child*/
		if (((len = gets(inputLine)) == 0) || (strcmp(inputLine, "logout") == 0))
			exit(1);

		if (len == 1)
		{
			printf("\n"); continue;
		}

		strcpy(tmpline, inputLine);
		tokenize(tmpline, inputs);

		/* the user wants to change directories: */
		if (strcmp(inputs[0], "cd") == 0)
		{
			/* if nothing was included with call to cd, change to '/' : */
			if (inputs[1] == 0)
			{
				chdir("/");
			}
			/* a directory was included with the call to cd so change to that: */
			else
			{
				if (stat(inputs[1], &tmpstat) == 0)
				{
					chdir(inputs[1]);
				}
				else
				{
					printf("sh: %s: directory does not exist\n", inputs[1]);
				}
			}
		}
		else if (strcmp(inputs[0], "help") == 0)
		{
			run_cmd("man help");
		}
		else
		{
			run_cmd(inputLine);
		}
	}
}
