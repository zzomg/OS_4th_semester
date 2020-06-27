#include <stdio.h>
#include "fcntl.h"
#include "stdlib.h"
#include "unistd.h"

int main() {

    int file = open("text.txt", O_WRONLY);

    struct flock lock;

    lock.l_type = F_WRLCK; // write-lock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    lseek(file, -2, SEEK_END);
    write(file, "1", 1);

    // returns -1 if a conflicting lock is held by another process
    if(fcntl(file, F_SETLK, &lock) == -1)
    {
        perror("text.txt ");
        close(file);
        exit(EXIT_FAILURE);
    }

    printf("Advisory record locking: \n");
    for(int i = 0; i < 5; i++)
    {
        printf("%d\n", 5 - i);
        sleep(1);
    }
    printf("\n");

    system("nano text.txt");

    lock.l_type = F_UNLCK;
    fcntl(file, F_SETLK, &lock);
    close(file);

    /***************************************/
    
    file = open("/tmp/text_tmp.txt", O_WRONLY);
    lock.l_type = F_WRLCK;

    if(fcntl(file, F_SETLKW, &lock) == -1)
    {
        perror("text_tmp.txt ");
        close(file);
        exit(EXIT_FAILURE);
    }

    lseek(file, -2, SEEK_END);
    write(file, "2", 1);

    printf("Mandatory locking: \n");
    for(int i = 0; i < 5; i++)
    {
        printf("%d\n", 5-i);
        sleep(1);
    }
    printf("\n");

    system("nano /tmp/text_tmp.txt");

    lock.l_type = F_UNLCK;
    fcntl(file, F_SETLKW, &lock);
    close(file);
    printf("End.\n");

    return 0;
}
