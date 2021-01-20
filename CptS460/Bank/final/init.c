/********** init.c file *************/
//an initalizer program which forks 3 login processes, one to system, 2 to UART

#include "ucode.c"
int console;
int uart1;
int uart2;

int parent() // P1's code
{
	int pid, status;
	while (1) {
		printf("INIT : wait for ZOMBIE child\n");
		pid = wait(&status);

		if (pid == console) { // if console login process died
			printf("INIT: forks a new console login\n");
			console = fork(); // fork another one
			if (console)
				continue;
			else
				exec("login /dev/tty0"); // new console login process
		}

		if (pid == uart1) { // if console login process died
			printf("INIT: forks a new UART1 login\n");
			uart1 = fork(); // fork another one
			if (uart1)
				continue;
			else
				exec("login /dev/ttyS0"); // new console login process
		}

		if (pid == uart2) { // if console login process died
			printf("INIT: forks a new UART2 login\n");
			uart2 = fork(); // fork another one
			if (uart2)
				continue;
			else
				exec("login /dev/ttyS1"); // new console login process
		}

		printf("INIT: I just buried an orphan child proc %d\n", pid);
	}
}

int main(int argc, char *argv[ ])
{
	int in, out, u1in, u1out, u2in, u2out; // file descriptors for terminal I/O
	in = open("/dev/tty0", O_RDONLY); // file descriptor 0
	out = open("/dev/tty0", O_WRONLY); // for display to console

	printf("INIT: fork a login proc on console\n", out);
	console = fork();

	if (console)
	{
		printf("INIT: fork a login proc on UART1\n");
		uart1 = fork();
		if (uart1)
		{
			printf("INIT: fork a login proc on UART2\n");
			uart2 = fork();
			if (uart2)
				parent();
			else
				exec("login /dev/ttyS1");
		}
		else
			exec("login /dev/ttyS0");
	}
	else
		exec("login /dev/tty0");
}
