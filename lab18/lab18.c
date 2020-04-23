#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

#define MAX_FILENAME 255

void getFileName(char* filename, char* name)
{
    for (int k = 0; k < MAX_FILENAME; ++k) {
        name[k] = '\0';
    }

    char c;
    int j = 0;
    int file_len = strlen(filename);

    for(int i = 0; i < file_len; ++i) {
        c = filename[i];
        if (c == '/') {
            j = 0;
            for (int k = 0; k < MAX_FILENAME; ++k) {
                name[k] = '\0';
            }
        } else {
            name[j] = c;
            ++j; 
        }
    }
}

struct passwd* getFileHost(struct stat *stbuf)
{
    struct passwd *pw;

    pw = getpwuid(stbuf->st_uid);

    return pw;
}

struct group* getFileGroup(struct stat *stbuf)
{
    struct group *grp;
    grp = getgrgid(stbuf->st_gid);

    return grp;
}

char getType(struct stat *stbuf)
{
    char type;

    switch(stbuf->st_mode & S_IFMT) {
        case S_IFDIR:
            type = 'd';
            break;
        case S_IFREG:
            type = '-';
            break;
        default:
            type = '?';
    }

    return type;
}

void permisToString(int permis, char* permis_printable) 
{
    char c;
    int d;

    // 0 = user, 1 = group, 2 = others 
    int user_type = 2;
    
    int read = 0;
    int write = 1;
    int execute = 2;
    
    for (int i = 0; i < 3; ++i) {
        d = permis % 8;
        switch (d)
        {
        case 1:
            permis_printable[3 * user_type + read] = '-';
            permis_printable[3 * user_type + write] = '-';
            permis_printable[3 * user_type + execute] = 'x';
            break;
        case 2:
            permis_printable[3 * user_type + read] = '-';
            permis_printable[3 * user_type + write] = 'w';
            permis_printable[3 * user_type + execute] = '-';
            break;
        case 3:
            permis_printable[3 * user_type + read] = '-';
            permis_printable[3 * user_type + write] = 'w';
            permis_printable[3 * user_type + execute] = 'x';
            break;
        case 4:
            permis_printable[3 * user_type + read] = 'r';
            permis_printable[3 * user_type + write] = '-';
            permis_printable[3 * user_type + execute] = '-';
            break;
        case 5:
            permis_printable[3 * user_type + read] = 'r';
            permis_printable[3 * user_type + write] = '-';
            permis_printable[3 * user_type + execute] = 'x';
            break;
        case 6:
            permis_printable[3 * user_type + read] = 'r';
            permis_printable[3 * user_type + write] = 'w';
            permis_printable[3 * user_type + execute] = '-';
            break;
        case 7:
            permis_printable[3 * user_type + read] = 'r';
            permis_printable[3 * user_type + write] = 'w';
            permis_printable[3 * user_type + execute] = 'x';
            break;
        default:
            break;
        }
        --user_type;
        permis /= 8;
    }

    int special = permis % 8;
    switch (special)
    {
    case 0:
        break;
    case 1: // sticky-bit
        user_type = 2;
        permis_printable[3 * user_type + execute] = 't';
    case 2: // setgid
        user_type = 1;
        permis_printable[3 * user_type + execute] = 's';
    case 3: // sticky-bit + setgid
        user_type = 2;
        permis_printable[3 * user_type + execute] = 't';
        user_type = 1;
        permis_printable[3 * user_type + execute] = 's';
    case 4: // setuid
        user_type = 0;
        permis_printable[3 * user_type + execute] = 's';
    case 5: // sticky-bit + setuid
        user_type = 2;
        permis_printable[3 * user_type + execute] = 't';
        user_type = 0;
        permis_printable[3 * user_type + execute] = 's';
    case 6: // setuid + setgid
        user_type = 1;
        permis_printable[3 * user_type + execute] = 's';
        user_type = 0;
        permis_printable[3 * user_type + execute] = 's';
    case 7: // setuid + setgid + sticky-bit
        user_type = 2;
        permis_printable[3 * user_type + execute] = 't';
        user_type = 1;
        permis_printable[3 * user_type + execute] = 's';
        user_type = 0;
        permis_printable[3 * user_type + execute] = 's';
    default:
        break;
    }
}

char getFileInfo(char* filename)
{
    struct stat stbuf;

    // execute (search) permission is required on all of the 
    // directories in pathname that lead to the file -- may cause error
    if (stat(filename, &stbuf) == -1) {
        perror("Can not obtain file information");
        exit(1);
    }

    char name[MAX_FILENAME];
    getFileName(filename, name);
    
    char type = getType(&stbuf);
    int permis = stbuf.st_mode & 0777;
    char permis_printable[9];
    permisToString(permis, permis_printable);
    
    size_t userID = stbuf.st_uid;
    char* userName = (getFileHost(&stbuf))->pw_name;

    char* lastModified = ctime(&stbuf.st_mtime);

    size_t size;

    printf("Filename:               %s\n", name);
    printf("Type:                   %c\n", type);
    printf("Permissions:            ");
    for (int i = 0; i < 9; ++i) {
        printf("%c", permis_printable[i]);
    }
    printf("\n");
    printf("User ID:                %d\n", userID);
    printf("Username:               %s\n", userName);
    printf("Last modified:          %s", lastModified);

    if (type == '-') {
        size = stbuf.st_size;
        printf("Size:                   %d\n", size);
    }
} 

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    } 
    for (int i = 1; i < argc; ++i) {
        getFileInfo(argv[i]);
    }

    return 0;
}
