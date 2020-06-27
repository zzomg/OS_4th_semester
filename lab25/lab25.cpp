#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

char input[80] = {0};

void myRead(char* str)
{
    for (int i = 0; i < 80; ++i)
    {
        read(0, &str[i], 1);
        if(str[i] == '\0') break;
    }
}

void myToUpper(char* str)
{
    for (int i = 0; i < strlen(str); ++i)
    {
        printf("%c", toupper(str[i]));
    }
}

int main(int argc, char **argv)
{
    int fd[2];
    void myToUpper(char* str);

    if (pipe(fd) == -1) {
        perror(argv[0]);
        exit(1);
    }

    if (fork() == 0) { /* first child */
        close(1);
        dup(fd[1]); /* redirect std output */
        close(fd[0]);
        close(fd[1]);
        for(int i = 0; i < 80; ++i)
        {
            char c = getchar();
            if (c == '\n') break;
            input[i] = c;
        }
        printf("%s\n", input);
        fflush(stdout);

        exit(127);
    }

    if (fork() == 0) { /* second child */
        close(0);
        dup(fd[0]); /* redirect std input */
        close(fd[0]);
        close(fd[1]);
        myRead(input);
        myToUpper(input);
        fflush(stdout);

        exit(127);
    }

    close(fd[0]);
    close(fd[1]);
    while (wait((int *) 0) != -1)
    ; /* null */
    exit(0);
}

