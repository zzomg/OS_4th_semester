#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char** argv)
{
    int subp_id = fork();
    int status;
    std::string file_path = "input.txt"; 
    std::string command = "cat " + file_path;

    if(subp_id == -1) {
        std::cerr << "Error: subprocess was not created" << std::endl;
        return EXIT_FAILURE;
    }

    if(subp_id == 0) {
        system(command.c_str());
    }
    
    if (subp_id > 0) {
        std::cout << "Procces successfully created" << std::endl;
        if (waitpid(subp_id, &status, WUNTRACED) > 0) {
            std::cout << "Hello world" << std::endl;
        }
    }

    return 0;
}
