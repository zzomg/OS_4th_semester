/*
16. Ответ без ввода новой строки
Напишите программу, которая печатает вопрос 
и требует односимвольного ответа. 
Измените атрибуты вашего терминала так, 
чтобы пользователю не нужно было вводить 
новую строку после ответа.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define MAX_ANSW_LEN 1

int main()
{
    char* question = "Answer this question? [y / n] ";
    char answ;
    int fd;
    struct termios tty, savtty;

    fd = open("/dev/tty", O_RDONLY);
    if(fd < 0) {
		perror("Error: couldn't open terminal\n");
		return EXIT_FAILURE;
	}
    tcgetattr(fd, &tty);
    /* isatty(3F) Этот системный вызов определяет, связан ли 
    файловый дескриптор с терминальным устройством или с 
    файлом какого-то другого типа. */
    /* The function fileno() examines the argument stream and
    returns its integer descriptor.*/
    if(isatty(fileno(stdout)) == 0) {
        fprintf(stderr, "Error : stdout is not a terminal\n");
        return EXIT_FAILURE;
    }
    savtty = tty;

    tty.c_lflag &= ~ICANON;
    tty.c_cc[VMIN] = 1;

    tcsetattr(fd, TCSAFLUSH, &tty);

    setbuf(stdout, (char*)NULL);
    printf("%s", question);
    
    for (int i = 0; i < MAX_ANSW_LEN; ++i) {
        read(fd, &answ, 1);
        printf("\n");
        if (answ == 'y') {
            printf("You answered this question\n");
        } else if (answ == 'n') {
            printf("You didn't answer this question\n");
        } else {
            printf("Unrecognized answer\n");
        }
    }

    tcsetattr(fd, TCSAFLUSH, &savtty);
    close(fd);
    return 0;
}
