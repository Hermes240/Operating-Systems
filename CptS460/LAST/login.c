#include "ucode.c"


int in, out, err, last_index;
char uname[128], password[128];
char *tokens[7], *passfile[3];


main()
{
    // 1. close file descriptors 0,1 inherited from INIT
    close(0), close(1);

    // 2. open argv[1] 3 times as in(0), out(1), err(2)
    in = open(argv[1],O_RDONLY);
    out = open(argv[1],O_WRONLY);
    err = open(argv[1],O_WRONLY);
    printf("login: tty=%s\n",argv[1]);
    printf("HOLOGIN: open %s as stdin=%d , stdout=%d , stderr=%d\n",argv[1],in,out,err);

    // 3. set tty name string in PROC.tty
    settty(argv[1]);

    // 4. open password file
    int password_file = open("/etc/passwd",O_RDONLY);

    last_index = read(password_file,line,1024);
    line[last_index] = 0;
    tokenize2(line);

    while (1)
    {
        // 5. get username and password
        printf("login: "); gets(uname);
        printf("password: "); gets(password);

        uname[strlen(uname)] = '\0';
        password[strlen(password)] = '\0';
       
        int copy = dup(in);
        int index = 0;
        close(0);

        while (index <= strlen(passfile))
        {
            //
            tokenize(passfile[index]);

            // 6. check if user has a valid account
            if(strcmp(tokens[0],uname) == 0 && strcmp(tokens[1],password) == 0)
            {
                // 7. change uid, gid to user's uid, gid //chuid
                chuid(atoi(tokens[3]),atoi(tokens[2]));
                //change cwd to user's home DIR
                chdir(tokens[5]);

                printf("HOLOGIN : Welcome ! %s\n",tokens[0]);
                printf("HOLOGIN : cd to HOME=%s\n",tokens[5]);
                // close opened /etc/passwd file
                close(0);
                dup2(copy,0);
                printf(tokens[5]);
                // 8. exec to program in user account
                exec("sh");

            }

            index++;

        }

        printf("login failed, try again\n");
        close(0);
        dup2(copy,0);
        close(copy);
    
    }
     
}


void tokenize(char *line)
{
    for(int i = 0; i < 7; i++)
    {
        tokens[i] = line;

        while(*line != ':' && *line != '\0')
        {
            ++line;
        }

        *line = '\0';
        line++;
    }
}

void tokenize2(char *line)
{
    for(int i = 0; i < 3; i++)
    {
        passfile[i] = line;

        while(*line != '\n' && *line != '\r\n')
        {
            ++line;
        }

        *line = '\0';
        line++;
    }
}