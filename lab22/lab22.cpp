#include <poll.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#define BUF_SZ 10
#define TIMEOUT (10 * 1000)

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); } while (0)

using namespace std;

string parse_argv(char* argv)
{
    string argv_str(argv);
    string tty_path;
    size_t found;

    if((found = argv_str.find("tty")) != string::npos) {
        tty_path = "/dev/" + argv_str.substr(found, 3);
        printf("File name is : %s\n", tty_path.c_str());
        return tty_path;
    }

    printf("File name is : %s\n", argv_str.c_str());
    return argv_str;
}

int remove_fd(struct pollfd* pfds, int fd_index, int num_open_fds)
{
    close(pfds[fd_index].fd);
    pfds[fd_index].events = 0;
    printf("Removing fd %d\n", pfds[fd_index].fd);
    for(int i = fd_index; i < num_open_fds; i++)
    {
        pfds[i] = pfds[i + 1];
    }
    num_open_fds--;

    return num_open_fds;
}

/*IT WORKS!*/

int main(int argc, char *argv[])
{
    int nfds, num_open_fds;
    struct pollfd *pfds;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s file...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    num_open_fds = nfds = argc - 1;
    pfds = (struct pollfd*)calloc(nfds, sizeof(struct pollfd));
    if (pfds == NULL) {
        errExit("Usage: lab22, malloc");
    }

    for (int j = 0; j < nfds; j++) {
        const char* arg = parse_argv(argv[j + 1]).c_str();
        pfds[j].fd = open(arg, O_RDONLY);
        if (pfds[j].fd == -1) {
            errExit("Usage: lab22, open");
        }

        printf("Opened \"%s\" on fd %d\n", arg, pfds[j].fd);

        pfds[j].events = POLLIN;
    }

    while(num_open_fds > 0)
    {
        for(int i = 0; i < num_open_fds; ++i)
        {
            char buf[BUF_SZ];

            int ready;

            printf("Polling...\n");
            ready = poll(&pfds[i], 1, TIMEOUT);
            if (ready == -1)
                errExit("Usage: lab22, poll");

            if ((pfds[i].revents) & POLLIN) {
                int nread;
                ioctl((pfds[i].fd), FIONREAD, &nread);

                if( nread == 0 )
                {
                    printf("\tclosing fd %d\n", pfds[i].fd);
                    num_open_fds = remove_fd(pfds, i, num_open_fds);
                } else {
                    printf("Reading...\n");
                    ssize_t s = read(pfds[i].fd, buf, BUF_SZ);
                    if (s == -1) {
                        errExit("read");
                    }
                    printf("\tread %zd bytes: %.*s\n", s, (int) s, buf);
                }
            }
            else {
                printf("  fd=%d; events: %s%s%s\n", pfds[i].fd,
                        (pfds[i].revents & POLLHUP) ? "POLLHUP " : "",
                        (pfds[i].revents & POLLERR) ? "POLLERR " : "",
                        (pfds[i].revents & POLLNVAL) ? "POLLNVAL " : "");
                printf("\tclosing fd %d\n", pfds[i].fd);
                num_open_fds = remove_fd(pfds, i, num_open_fds);
            }
        }
    }
    printf("All file descriptors closed; bye\n");
    exit(EXIT_SUCCESS);
}
