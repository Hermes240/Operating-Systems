/********************************************
*Programmer: Hermes Obiang
*Class: CptS 460, Spring 2020
*Programming Assignment: Last LAB
*Date: April 28, 2020
*********************************************/
#include "ucode.c"

# define NULL ((void *)0)

char homePath[256];
int count;
char *tokens[32];


int main (int argc, char *argv[])
{
    char commandLine[256];
    char inputArray[20][64];
    char temp[256];
  
    //save homepath for later use
    getcwd(homePath);

    while(1)
    {
        // clear buffer that saves tokenized strings
        memset(tokens,0,32); 
         // clear user input
        memset(commandLine,0,256);
        // get input from user
        getInput(commandLine); 

        if(strlen(commandLine)> 0)
        {
            //make a copy of the input
            strcpy(temp, commandLine);
            // tokenize the input
            parseInput(temp, inputArray);
        
            handleCommand(inputArray, commandLine);
        }
        
    }
	return 0;
}


// This tokenize function is a modified version of KC Wang token()
void tokenize(char *line)
{
  char *cp;
  cp = line;
  count = 0;
  
  while (*cp != 0){
       while (*cp == ' ') *cp++ = 0;        
       if (*cp != 0)
           tokens[count++] = cp;         
       while (*cp != ' ' && *cp != 0) cp++;                  
       if (*cp != 0)   
           *cp = 0;                   
       else 
            break; 
       cp++;
  }
  tokens[count] = 0;
}

// Get input from user
 void getInput(char *inputString)
 {
    
    char current_dir[1024];

    getcwd(current_dir);
    printf("\nHERMES:~%s# ", current_dir);
    gets(inputString);

    inputString[strlen(inputString)] = 0;
    
 }

 // tokenize the input
 void parseInput(char *input, char inputArr[20][64])
 {
    int count = 0, i = 0;
    char *str = 0;
   

    tokenize(input);
  
    inputArr = tokens;
    return;
 }

 // Determine which command to run and run it
 void handleCommand(char input[20][64], char *inputString)
 {
    char *temp;
    int i = 0;
    // If command = cd run cd command
    
    // run simple commands
    if(strcmp(tokens[0], "cd") == 0)
    {
       if(tokens[1])
            {
                chdir(tokens[1]);
                
            }
            else
            {
                chdir(homePath);
                
            }

    }
    else if(strcmp(tokens[0], "logout") == 0)
    {
        printf("sh: logging you out.\n");
        chdir(homePath);
        exit(0);
    }
    else
    {
        // for binary executable command
        forkChild(inputString);
    }
    return;
}

 

// Fork a new child and check if pipe
 int forkChild(char* inputString)
 {
    int pid;
    int status = 0;
    int i = 0;

    pid = fork(); // fork a child process

    if(pid < 0)  
    {
        printf ("Could not fork child\n");
        exit(1);
    }

    if(pid)  // parent sh
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
            
            doPipe(inputString);
        }
        else
        {
            
            runProg(inputString);
        }
    }
    
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
            
            // set redirect indicator in input array to NULL
            inputString[i-1] = 0;
            inputString[i++] = 0;
            inputString[i++] = 0;
            if(inputString[i] == ' ') {inputString[i++] = 0; }
            strcpy(file, inputString + i);
            
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
                open(file, O_WRONLY|O_CREAT);
                break;
        case 3: close(1);
                open(file, O_APPEND|O_WRONLY|O_CREAT);
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
        
        close(1);
        dup2(pd[1], 1);
        close(pd[0]);
        runProg(inputString);
    }
    else // parent process runs tail
    {
        tailString(inputString);
        printf("Parent %d forks child %d \n", getpid(), pid);
       
        close(0);
        dup2(pd[0], 0);
        close(pd[1]);
        if(hasPipe(inputString) == 0)
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
    
    
    isRedirect(inputString, &red, file);
    if(red)
    {
       
        redirect(file, red);
    }
    //printf("No redirect found, executing input string: %s\n", inputString);
    exec(inputString);
}

