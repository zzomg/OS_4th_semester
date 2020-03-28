#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char** argv, char** envp)
{
    if (argc < 2) {
        std::cerr << "Execution error : Wrong arguments" << std::endl;
        return EXIT_FAILURE;
    }
    std::string command(argv[1]);
    std::cout << "Argc: " << argc << std::endl;
    std::cout << "Init command: " << command << std::endl;
    for(int i = 2; i < argc; ++i) {
        command = command + " ";
        command = command + std::string(argv[i]);
        std::cout << "Command is: " << command << std::endl;
    }

    std::cout << "Init process ID: " << getpid() << std::endl;

    std::cout << "-----\nGetting value of USER env variable: " << std::endl;
    const char* name = "USER";
    std::cout << getenv(name) << std::endl;
    std::cout << "\n-----" << std::endl;

    int subp_id = fork();
    int status;

    if(subp_id == -1) {
        std::cerr << "Error: subprocess was not created" << std::endl;
        return EXIT_FAILURE;
    }

    if(subp_id == 0) { // Child process
        std::cout << "Child ID: " << getpid() << std::endl;
        std::cout << "Child's parent ID: " << getppid() << std::endl;
        system(command.c_str());
    } else { // Parent process 
        std::cout << "Subprocces successfully created" << std::endl;
        std::cout << "Parent ID: " << getpid() << std::endl;
        int w = waitpid(subp_id, &status, WUNTRACED);
        if(w == -1) {
            std::cerr << "Error: subprocess didn't terminate correctly" << std::endl;
            return EXIT_FAILURE;
        }
        if (WIFEXITED(status)) {
            printf("Subprocess terminated, status = %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Killed by signal %d\n", WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("Stopped by signal %d\n", WSTOPSIG(status));
        } 
    }

    return 0;
}
