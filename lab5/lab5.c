#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <fcntl.h> 

struct newLines
{
	int line_offset;
	int line_len;
	int line_num;
};

int main()
{
	int file = open("input.txt", O_RDONLY);

	char buf;
	int offset = 0;
	int line_n = 0;

	struct newLines lines[1024];
	for (int i = 0; i < 1024; ++i) {
		lines[i].line_len = 0;
		lines[i].line_offset = -1;
		lines[i].line_num = 0;
	}

	printf("Enter line number: \n");
	scanf("%d", &line_n);
	if (line_n == 0) {
		return 0;
	}

	int i = 0;
	/*seeking new line symbols*/
	while (read(file, &buf, 1)) {
		if (buf == '\n') {
			lines[i].line_offset = lseek(file, 0L, SEEK_CUR);
			++i;
		}
	}

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
		return EXIT_FAILURE;
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
