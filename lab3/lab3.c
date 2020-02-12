#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void set_euid_to_ruid(uid_t ruid, uid_t euid) 
{
    int status;

    status = setuid (ruid);
    
    if (status < 0) {
        fprintf (stderr, "Couldn't set uid.\n");
        exit (status);
        }
}

int main()
{
    FILE* file;

    uid_t ruid;
    uid_t euid;

    ruid = getuid();
    euid = geteuid();

    printf("Real UID before=: %d\n", ruid);
    printf("Effective UID before=: %d\n", euid);

    file = fopen("input.txt", "r");
    if (!file) {
        perror("File opening failed");
        return EXIT_FAILURE;
    }
    else {
        printf("Success!\n");
        fclose(file);
    }

    set_euid_to_ruid(ruid, euid);

    ruid = getuid();
    euid = geteuid();

    printf("Real UID after=: %d\n", ruid);
    printf("Effective UID after=: %d\n", euid);

    file = fopen("input.txt", "r");
    if (!file) {
        perror("File opening failed");
        return EXIT_FAILURE;
    }
    else {
        printf("Success!\n");
        fclose(file);
    }

    return 0;
}
