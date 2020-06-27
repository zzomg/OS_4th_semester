#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

extern char** environ;

int my_execvpe(char* file, char** argv, char** envp)
{
    char** init_env = environ;
    environ = NULL;
    while (*envp)
    {
        int res = putenv(*envp);
        if(res < 0) {
            return 1;
        }
        ++envp;
    }
    int ret = execvp(file, argv);
    clearenv();
    environ = init_env;
    return ret;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        perror("Usage : lab11");
        exit(1);
    }

    char* exec_argv[] = {argv[1], NULL};
    char* exec_env[] = {"TZ=Asia/Tokyo", NULL};
    if(my_execvpe(argv[1], exec_argv, exec_env) == -1) {
        perror("Execvp failed");
        exit(1);
    }

    exit(0);
}
