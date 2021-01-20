#include "ucode.c"

int in, out;
STAT in_stat, out_stat;

//This function opens files and stats them for later.
void open_files(char *infile, char *outfile)
{
    in = open(infile, O_RDONLY); // open you are reading from

    stat(infile, &in_stat); // stats the file

    if( in < 0)
    {
        printf("cp: couldn't open file for reading!");
        exit(-1);
    }

    stat(outfile, &out_stat);

    if(out_stat.st_ino == in_stat.st_ino) // make sure you are not reading from same file
    {
        printf("cp: cannot cp a file into itself\n");
        exit(-1);
    }

    creat(outfile); // create file

    out = open(outfile, O_WRONLY);
    if(out < -1)
    {
        printf("cp: could not open file for writing!\n");
        exit(-1);
    }
}


int main(int argc, char *argv[])
{
    char c;

    //Make sure we didn't get a weird cp call
    if(argc > 3 && argc < 3)
    {
        printf("usage: cp <filename> <filename2>");
    }
    //If we got a valid cp call, open files for copying
    if(argc == 3)
    {
        open_files(argv[1],argv[2]);
    }

    //Make sure the fd's are valid
    if(out < 0)
    {
        printf("cp: could not open file for writing!");
        exit(-1);
    }

    //Read byte by byte into the new file from the old file
    while(read(in, &c, 1024))
    {
        write(out, &c, 1024);
    }

    exit(0);
}