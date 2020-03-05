#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <fcntl.h> 
#include <sys/poll.h>

#define TIMEOUT 5 //n of sec to wait

struct newLines
{
	int line_offset;
	int line_len;
	int line_num;
};

int main()
{
	int file = open("input.txt", O_RDONLY);
	if(file < 0) {
		perror("File opening failed\n");
		return -1;
	}

	struct pollfd fds[1];

	int ret;
	char buf;
	int line_n = -1;

	struct newLines lines[1024];
	for (int i = 0; i < 1024; ++i) {
		lines[i].line_len = 0;
		lines[i].line_offset = -1;
		lines[i].line_num = 0;
	}

	printf("Enter line number: \n");
	/* watch stdin for input */
	fds[0].fd = 0;
	fds[0].events = POLLIN;

	while((ret = poll(fds, 1, TIMEOUT * 1000))) {
	    if(scanf("%d", &line_n)) break;
    }

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

	int i = 0;
	/*seeking new line symbols*/
	while (read(file, &buf, 1)) {
		if (buf == '\n') {
			lines[i].line_offset = lseek(file, 0L, SEEK_CUR);
			++i;
		}
	}

	int offset = 0;
	/*getting line length*/
	for (int i = 0; i < 1024 && lines[i].line_offset != -1; ++i) {
		lines[i].line_len = lines[i].line_offset - offset - 1;
		lines[i].line_num = i + 1;
		offset = lines[i].line_offset;
	}

	int num_of_lines = 0;
	for (int i = 0; i < 1024 && lines[i].line_num != 0; ++i) {
		++num_of_lines;
	}

	if (line_n > num_of_lines) {
		perror("Error : Line number out of range\n");
		close(file);
		return -1;
	}

	/*for (int i = 0; i < 1024 && lines[i].line_offset != -1; ++i) {
		printf("%d: offset %d, line_len %d\n", lines[i].line_num, lines[i].line_offset, lines[i].line_len);
	}*/

	char line[1024];
	for (int i = 0; i < 1024; ++i) {
		line[i] = '\0';
	}

	lseek(file, 0L, SEEK_SET);
	lseek(file, lines[line_n - 1].line_offset - lines[line_n - 1].line_len - 1, SEEK_SET);
	read(file, line, lines[line_n - 1].line_len);
	
	for (int i = 0; i < 1024 && line[i] != '\0'; ++i) {
		printf("%c", line[i]);
	}
	printf("\n");

	close(file);
	return 0;
}
