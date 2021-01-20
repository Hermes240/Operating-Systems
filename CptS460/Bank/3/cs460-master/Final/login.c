#include "ucode.c"

char *tty;
char username[32], password[32], uid[32], home[32], prog[32], line1[128];
char *temp;
int stdin, stdout, stderr;

int main(int argc, char *argv[])
{
    tty = argv[1];
    close(0); close(1); close(2);
    stdin = open(tty, 0);
    stdout = open(tty, 1);
    stderr = open(tty, 2);

    settty(tty);

    signal(2,0);

    while(1)
    {
        printf("****Login****\n");
        printf("Username: ");
        gets(username);
        //printf("You entered: %s", username); //FOR TESTING
        printf("Password: ");
        gets(password);
        if(check(username, password))
        {
            printf("Correct username & password\n"); //FOR TESTING
            setuid(atoi(uid));
            chdir(home);
            exec(prog);
        }
        printf("Incorrect Username and/or password\n");
    }
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
    if(c == '\n' || c == '\r') {i++;}
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

int check(char *username, char *password)
{
    struct stat fs;
    int left, n;
    int pfile = open("/etc/passwd", O_RDONLY);
    if(!pfile) {printf("Error opening file\n"); return 0;}
    //printf("File is open\n"); //FOR TESTING

    fstat(pfile, &fs);
    left = fs.st_size;
    reset(line1, 128);
    while(left > 0)
    {
        if(left >= 128) { n = readLine(line1, 128, pfile); }
        else { n = readLine(line1, left, pfile); }
        left -= n;
        //printf("line: %s\n", line1); //FOR TESTING
        temp = strtok(line1, ":");
        //printf("temp: %s\n", temp); //FOR TESTING
        if(strcmp(temp, username) == 0)
        {
            temp = strtok(0, ":");
            if(strcmp(temp, password) == 0)
            {
                temp = strtok(0, ":");
                strcpy(uid, temp);
                //printf("uid: %s\n", uid);
                temp = strtok(0, ":");
                temp = strtok(0, ":");
                temp = strtok(0, ":");
                strcpy(home, temp);
                //printf("home: %s\n", home);
                temp = strtok(0, ":");
                strcpy(prog, temp);
                //printf("prog: %s\n", prog);
                return 1;
            }
        }
        reset(line1, 128);
    }
    close(pfile);
    return 0;
}
