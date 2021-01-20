// #include <stdio.h>
// #include <stdlib.h>
 //#include <string.h>
 //#include <fcntl.h>
 //#include <unistd.h>
 #include "ucode.c"

 char homePath[256];


 void getInput(char *inputString);
 void parseInput(char* input, char inputArr[20][64]);
 void handleCommand(char input[20][64], char* inputString);
 void cd(char* pathname);
 void exitProg();
 int forkChild(char* inputString);
 char* isRedirect(char *inputString, int* red, char *file);
 void redirect(char* file, int red);
 int hasPipe(char *inputString);
 void doPipe(char *inputString);
 void runProg(char* inputString);


int main (int argc, char *argv[])
{
    char inputString[256];
    char inputArray[20][64];
    char temp[256];
  
    //findPath();
    getcwd(homePath);

    while(1)
    {
        reset(inputString, 256);
        getInput(inputString);
        //printf("You entered: %s\n", inputString); //FOR TESTING
        strcpy(temp, inputString);
        //printf("Again, you entered: %s\n", inputString);
        parseInput(temp, inputArray);
        //printf("Now we will run command: %s\n", inputString); //FOR TESTING
        handleCommand(inputArray, inputString);
    }
	return 0;
}

int readLine(char *buf, int size, int file)
{
    int i = 0;
    char c;
    read(file, &c, 1);
    while(c != '\n' && c != 0 && i < size)
    {
        buf[i++] = c;
        read(file, &c, 1);
        //putc(c); //FOR TESTING
    }
    return i;
}

int reset(char *buf, int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        buf[i] = 0;
    }
}

 void getInput(char *inputString)
 {
    char buf[1024];
    char curdir[1024];
    char *temp = NULL;
    int len = 0;

    getcwd(curdir);
    printf("User@mysh: %s$ ", curdir);
    gets(inputString);
    len = strlen(inputString);
    //printf("len: %d\n", len); //FOR TESTING
    if(inputString[len-1] == '\n')
    {
        inputString[len-1] = 0;
    }
 }

 // Parse user input string into argv[] array of char*'s
 void parseInput(char *input, char inputArr[20][64])
 {
    int count = 0, i = 0;
    char *str = 0;
    // Allocate char* inputArr[20]
    str = strtok(input, " ");
    while(str)
    {
        //reset(inputArr[count], 64);
        //printf("Str: %s, strlen: %d\n", str, strlen(str)); //FOR TESTING
        strcpy(inputArr[count], str);
        count++;
        //printf("inputArr[%d]: %s\n", count-1, inputArr[count-1]); //FOR TESTING
        str = strtok(0, " ");
    }
    inputArr[count][0] = 0;
    /*for(i = 0; i < count; i++)
    {
        printf("inputArr[%d]: %s\n", i, inputArr[i]); //FOR TESTING
    }*/
    return;
 }

 // Determine which command to run and run it
 void handleCommand(char input[20][64], char *inputString)
 {
    char *temp;
    int i = 0;
    // If command = cd run cd command
    //printf("input[0]: %s\n", input[0]);
    if(strcmp(input[0], "cd") == 0)
    {
        //printf("Found command cd\n"); //FOR TESTING
        cd(input[1]);
    }
    else if(strcmp(input[0], "exit") == 0)
    {
        exit(1);
    }
    else
    {
        //printf("Need to fork a child to do command\n"); //FOR TESTING
        forkChild(inputString);
    }
    return;
}

 // Change directory command
 void cd(char* pathname)
 {
    if(pathname)
    {
        chdir(pathname);
    }
    else
    {
        chdir(homePath);
    }
 }

 // Exit command
 void exitProg()
 {
    exit(1);
 }

 // All other commands
 /*void otherCommand(char** input, char* inputString)
 {
    char original[128];
    char new[1024];
    int i = 0;
    strcpy(new, "/bin/")'
    strcat(new, input[0]);
    exec(new);

 }*/


// Fork a new child and check if pipe
 int forkChild(char* inputString)
 {
    int pid;
    int status = 0;
    int i = 0;

    pid = fork();

    if(pid < 0)
    {
        printf ("Could not fork child\n");
        exit(1);
    }

    if(pid)
    {   
        printf("Parent %d forks child %d \n", getpid(), pid);
        pid = wait(&status);
        printf("Parent %d found dead child %d\n", getpid(), pid);
    }
    else
    {
        
        // Check for pipe
        if(hasPipe(inputString))
        {
            //printf("Pipe Found!\n"); //FOR TESTING
            doPipe(inputString);
        }
        else
        {
            //printf("No pipe, running program\n"); //FOR TESTING
            runProg(inputString);
        }
    }
    //printf("Done waiting\n"); //FOR TESTING
    return status;
 }

// Determine if there is an I/O redirect. Set redirect 1 for input, 2 for output, 
//   3 for append output 0 if no redirect. Return filename to redirect to/from
 char* isRedirect(char *inputString, int* red, char *file)
 {
    int i = 1;
    char temp[64], temp2[64];
    
    while(!(*red) && inputString[i])
    {
        if(inputString[i] == '<') // Redirect input
        {
            *red = 1;
        }
        else if(inputString[i] == '>') //Redirect output
        {
            if(inputString[i+1] == '>') { *red = 3; }
            else { *red = 2; }
        }
        if(*red)
        {
            printf("Redirect %d found\n", *red); //FOR TESTING
            // set redirect indicator in input array to NULL
            inputString[i-1] = 0;
            inputString[i++] = 0;
            inputString[i++] = 0;
            if(inputString[i] == ' ') {inputString[i++] = 0; }
            strcpy(file, inputString + i);
            //printf("redirection to/from %s\n", temp); //FOR TESTING
        }
        i++;
    }
    return;
 }

// redirect to different input or output
 void redirect(char* file, int red)
 {
    switch(red)
    {
        case 1: close(0);
                open(file, O_RDONLY);
                break;
        case 2: close(1);
                open(file, O_WRONLY|O_CREAT, 0644);
                break;
        case 3: close(1);
                open(file, O_APPEND|O_WRONLY|O_CREAT, 0644);
                break;
        default: printf("Error, bad redirect\n");
                break;
    }
 }

// Determine if input has a pipe (if so return 1 else return 0)
int hasPipe(char *inputString)
{
    int i = 0;
    while(inputString[i] != 0)
    {
        if(inputString[i] == '|') {return 1; }
        i++;
    }
    return 0;
}

int tailString(char *inputString)
{
    int i, j;
    char temp[256];
    strcpy(temp, inputString);

    i = 0; j = 0;
    while(temp[i] != '|') {i++;}
    i += 2;
    while(temp[i])
    {
        inputString[j++] = temp[i++];
    }
    inputString[j] = 0;
}

int headString(char *inputString)
{
    int i;
    while(inputString[i] != '|') {i++;}
    inputString[i-1] = 0;
}

// fork a new child and redirect input/output through pipe
void doPipe(char* inputString)
{
   int pd[2];
   int pid;
   
   pipe(pd); // Create pipe

    // Create new child to share pipe

   pid = fork();
   if(!pid) //Child process runs head
   {
        headString(inputString);
        //printf("Head String = %s\n", inputString); //FOR TESTING
        close(1);
        dup2(pd[1], 1);
        close(pd[0]);
        runProg(inputString);
    }
    else // parent process runs tail
    {
        tailString(inputString);
        printf("Parent %d forks child %d \n", getpid(), pid);
        //printf("Tail String = %s\n", inputString); //FOR TESTING
        close(0);
        dup2(pd[0], 0);
        close(pd[1]);
        if(hasPipe(inputString))
        {
            doPipe(inputString);
        }
        else
        {
            close(1);
            open("/dev/tty0", O_WRONLY);
            runProg(inputString);
        }
    } 
}

// Check for redirect and run output
void runProg(char *inputString)
{
    int red = 0;
    char file[64];

    // Check for file redirection
    
    //printf("Checking for redirection\n"); //FOR TESTING
    isRedirect(inputString, &red, file);
    if(red)
    {
        printf("File redirect found!\n"); //FOR TESTING
        redirect(file, red);
    }
    //printf("No redirect found, executing input string: %s\n", inputString);
    exec(inputString);
}


