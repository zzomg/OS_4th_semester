#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <ctype.h>

#define POLL_SIZE 32
#define LISTEN_BACKLOG 50
#define SOCKET_PATH "/tmp/1.socket"

#define BUF_SZ 256

#define CLOSE_CL_CMD "/END"
#define SHUT_SERVER "/DOWN"

void myToUpper(char* str)
{
    printf("Result:\n");
    for (int i = 0; i < strlen(str); ++i)
    {
        printf("%c", toupper(str[i]));
        if(i == strlen(str) - 1) {
            printf("\n");
        }
    }
}

int closeClient(struct pollfd* poll_set, int fd_index, int numfds)
{
    close(poll_set[fd_index].fd);
    poll_set[fd_index].events = 0;
    printf("Removing client on fd %d\n", poll_set[fd_index].fd);
    for(int i = fd_index; i < numfds; i++)
    {
        poll_set[i] = poll_set[i + 1];
    }
    numfds--;

    return numfds;
}

int main(int argc, char* argv[])
{
    int connection_socket, client_socket;
    int server_len, client_len;
    struct sockaddr_un server_address;

    char buffer[BUF_SZ] = {0};

    struct pollfd poll_set[POLL_SIZE];
    int numfds = 0;

    int ret;
    int down_flag = 0;
    int timeout = 30 * 1000; // 10 s

    unlink(SOCKET_PATH);
    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connection_socket == -1) {
        perror("Usage: lab31, socket");
        exit(EXIT_FAILURE);
    }

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, SOCKET_PATH);
    server_len = sizeof(server_address);

    if(bind(connection_socket, (struct sockaddr*) &server_address, (socklen_t)server_len) == -1 ) {
        perror("Usage: lab31, bind");
        exit(EXIT_FAILURE);
    }
    if(listen(connection_socket, LISTEN_BACKLOG) == -1) {
        perror("Usage: lab31, listen");
        exit(EXIT_FAILURE);
    }
    memset(poll_set, 0, sizeof(poll_set));

    poll_set[0].fd = connection_socket;
    poll_set[0].events = POLLIN;
    numfds++;

    while (1)
    {
        printf("Waiting for client (%d total)...\n", numfds - 1);
        ret = poll(poll_set, numfds, timeout);

        if( ret < 0 ) {
            perror("Usage: lab31, poll");
            break;
        }

        if ( ret == 0 ) {
            printf("Lab31, poll timed out\n");
            break;
        }

        for(int fd_index = 0; fd_index < numfds; fd_index++)
        {
            if( poll_set[fd_index].revents & POLLIN ) {
                if(poll_set[fd_index].fd == connection_socket) {
                    client_socket = accept(connection_socket, NULL, NULL);
                    if(client_socket == -1) {
                        continue;
                    }

                    poll_set[numfds].fd = client_socket;
                    poll_set[numfds].events = POLLIN;
                    numfds++;

                    printf("Adding client on fd %d\n", client_socket);
                }
                else {
                    int nread;
                    ioctl(poll_set[fd_index].fd, FIONREAD, &nread);

                    if( nread == 0 )
                    {
                        numfds = closeClient(poll_set, fd_index, numfds);
                    } else {
                        if(read(poll_set[fd_index].fd, buffer, BUF_SZ) == -1) {
                            perror("Usage: lab31, read");
                            numfds = closeClient(poll_set, fd_index, numfds);
                        }
                        buffer[BUF_SZ - 1] = 0;

                        if(!strncmp(buffer, SHUT_SERVER, BUF_SZ)) {
                            down_flag = 1;
                            break;
                        }

                        if(!strncmp(buffer, CLOSE_CL_CMD, BUF_SZ)) {
                            numfds = closeClient(poll_set, fd_index, numfds);
                        }

                        myToUpper(buffer);
                        strcpy(buffer, "");
                    }
                }
            }
        }
        if(down_flag) {
            break;
        }
    }
    for (int i = 0; i < numfds; i++)
    {
        if(poll_set[i].fd >= 0) {
            close(poll_set[i].fd);
        }
    }
    unlink(SOCKET_PATH);
    exit(EXIT_SUCCESS);
}
