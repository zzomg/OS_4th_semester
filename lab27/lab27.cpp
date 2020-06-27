#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        perror("Usage : lab27.c");
        exit(1);
    }

    string file(argv[1]);
    string command = "grep '^$' " + file;

    FILE *fpin, *fpout;
    char line[BUFSIZ];

    fpin = popen(command.c_str(), "r");
    fpout = popen("wc -l", "w");

    while(fgets(line,BUFSIZ,fpin) != (char *)NULL)
        fputs(line, fpout);

    pclose(fpin);
    pclose(fpout);

    return 0;
}
