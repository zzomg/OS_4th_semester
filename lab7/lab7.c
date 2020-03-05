#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/poll.h>

#define TIMEOUT 5
#define STOR_SIZE 1024

struct fileMapping {
    int fd;
    size_t fsize;
    unsigned char* dataPtr;
};

struct newLines
{
	int line_offset;
	int line_len;
	int line_num;
};

int main()
{
    struct stat st;
    struct pollfd fds[1];

	int ret;
	char buf;
	int line_n = -1;

    int file = open("input.txt", O_RDONLY);
	if(file < 0) {
		perror("Error: File opening failed\n");
		return -1;
	}

    if(fstat(file, &st) < 0) {
        perror("Error: Couldn't obtain information about file\n");
        close(file);
        return -1;
    }

    size_t fsize = (size_t)st.st_size;

    unsigned char* dataPtr = (unsigned char*)mmap(NULL, fsize,
                                              PROT_READ, //Pages may be read
                                              MAP_PRIVATE,
                                              file, 0);
    if(dataPtr == MAP_FAILED) {
        perror("Error: Mapping failed\n");
        close(file);
        return -1;
    }

    struct newLines lines[STOR_SIZE];
	for (int i = 0; i < STOR_SIZE; ++i) {
		lines[i].line_len = 0;
		lines[i].line_offset = -1;
		lines[i].line_num = 0;
	}

    fds[0].fd = 0;
	fds[0].events = POLLIN;

    struct fileMapping* mapping = (struct fileMapping*)malloc(sizeof(struct fileMapping));

    mapping->fd = file;
    mapping->fsize = fsize;
    mapping->dataPtr = dataPtr;

    int k = 0;
    int offset = 0;
    printf("Writing output:\n");
    for(int bytes = 0; bytes < mapping->fsize; ++bytes) {
        char c;
        if ((c = mapping->dataPtr[bytes]) == '\n') {
            lines[k].line_len = bytes - offset;
            lines[k].line_offset = offset;
            lines[k].line_num = k + 1;
            ++k;
            offset = bytes + 1;
        }
    }

    for(int i = 0; i < STOR_SIZE && lines[i].line_offset != -1; ++i) {
        printf("line_num: %d | line_len: %d | line_offset: %d\n", 
            lines[i].line_num,
            lines[i].line_len,
            lines[i].line_offset);
    }

    int num_of_lines = 0;
	for (int i = 0; i < STOR_SIZE && lines[i].line_num != 0; ++i) {
		++num_of_lines;
	}

    printf("Enter line number: \n");
	/* watch stdin for input */
	fds[0].fd = 0;
	fds[0].events = POLLIN;

	ret = poll(fds, 1, TIMEOUT * 1000);
	scanf("%d", &line_n);

	if (ret == -1) {
		perror("Error: poll\n");
		return -1;
	}
	else if (ret == 0) {
		printf("Sorry, %d seconds expired. Please, try again\n", TIMEOUT);
		system("cat input.txt");
		return 0;
	}
	else {
		if (fds[0].revents & POLLIN)
			fds[0].revents = 0;
		if (line_n == 0) {
			return 0;
		}
	}

    if (line_n > num_of_lines) {
		perror("Error : Line number out of range\n");
		close(file);
		return -1;
	}

    char line[1024];
	for (int i = 0; i < 1024; ++i) {
		line[i] = '\0';
	}

    for(int i = 0; i < lines[line_n - 1].line_len; ++i) {
        printf("%c", mapping->dataPtr[lines[line_n - 1].line_offset + i]);
    }
    printf("\n");

    munmap(mapping->dataPtr, mapping->fsize);
    close(mapping->fd);
    free(mapping);

    return 0;
}
