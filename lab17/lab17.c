#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define TC_MAX_BUF 4095
#define STR_MAX_LEN 10

int main()
{
    int fd;
    struct termios tty, savtty;
    FILE* logs = fopen("logs.txt", "a");
    fprintf(logs, "\n----------------New session----------------\n");

    fd = open("/dev/tty", O_RDONLY);
    if(fd < 0) {
        perror("Error: couldn't open terminal\n");
        return EXIT_FAILURE;
    }
    tcgetattr(fd, &tty);

    if(isatty(fileno(stdout)) == 0) {
        fprintf(stderr, "Error : stdout is not a terminal\n");
        return EXIT_FAILURE;
    }
    savtty = tty;

    tty.c_lflag &= ~(ICANON | ECHO);
    tty.c_cc[VMIN] = 1;

    tcsetattr(fd, TCSAFLUSH, &tty);
    setbuf(stdout, (char*)NULL);

    /*According to termios man page:
    The read buffer will only accept 4095 chars in noncanonical mode*/
    char ch;
    int wordtable[TC_MAX_BUF] = {0};
    int space_flag = 0;
    int line_len = 0;
    int i, word;

    for(i = 0, word = 0; i < TC_MAX_BUF; ++i)
    {
        read(fd, &ch, 1);
        if (ch >= 32 && ch <= 126) {
            putchar(ch);
            if (ch == 32 && space_flag == 0) { // space
                space_flag = 1;
            }
            if (ch != 32 && space_flag == 1) {
                ++word;
                space_flag = 0;
            }
            ++wordtable[word];
            ++line_len;
            fprintf(logs, "new symbol: line_len = %d\n", line_len);
        } else if (ch == 8) { // backspace (Ctrl-H)
            putchar(127);
            if (i > 0) {
                --i; // this iteration with BS symbol
            }
            if (wordtable[word] > 0) {
                --wordtable[word];
                --line_len;
            } else {
                if (word > 0) {
                    --word;
                    if (wordtable[word] > 0) {
                        --wordtable[word];
                        --line_len;
                    }
                    space_flag = 1;
                }
            }
            fprintf(logs, "backspace (Ctrl-H): line_len = %d\n", line_len);
        } else if (ch == 23) { // delete word (Ctrl-W)
            for(int c = 0; c < wordtable[word]; ++c) {
                putchar(127);
                --i;
                --line_len;
            }
            wordtable[word] = 0;
            if (word > 0) {
                --word;
            }
            space_flag = 0;
            fprintf(logs, "delete word (Ctrl-W): line_len = %d\n", line_len);
        } else if (ch == 21) { // delete line (Ctrl-U)
            for (int wc = word; wc >= 0; --wc) {
                for(int c = 0; c < wordtable[wc]; ++c) {
                    putchar(127);
                }
            }
            i = 0;
            word = 0;
            space_flag = 0;
            line_len = 0;
                        fprintf(logs, "delete line (Ctrl-U): line_len = %d\n", line_len);
        } else if (ch == 4 && line_len == 0) { // close program (Ctrl-D)
            fprintf(logs, "close program (Ctrl-D): line_len = %d\n", line_len);
            break;
        } else {.
            putchar(7);
        }
        if (line_len > STR_MAX_LEN) {
            putchar(10);
            line_len = 0;
            space_flag = 0;
            word = 0;
            for (int wc = word; wc >= 0; --wc) {
                wordtable[wc] = 0;
            }
            fprintf(logs, "line length exceeded - new line: line_len = %d\n", line_len);
        }
    }

    tcsetattr(fd, TCSAFLUSH, &savtty);
    close(fd);
    fflush(logs);
    fclose(logs);
    return 0;
}

