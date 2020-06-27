#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>

#define SOCKET_PATH "/tmp/1.socket"
#define LISTEN_BACKLOG 50
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

int main(int argc, char **argv)
{
    int down_flag = 0;
    char buffer[BUF_SZ] = {0};

    int connection_socket, client_socket;
    struct sockaddr_un server_address;

    connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (connection_socket == -1) {
        perror("Usage: lab30, socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(struct sockaddr_un));

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, SOCKET_PATH);

    if(bind(connection_socket, (struct sockaddr*) &server_address, sizeof(struct sockaddr_un)) == -1) {
        perror("Usage: lab30, bind");
        exit(EXIT_FAILURE);
    }

    if(listen(connection_socket, LISTEN_BACKLOG) == -1) {
        perror("Usage: lab30, listen");
        exit(EXIT_FAILURE);
    }

    /* Now we can accept incoming connections one at a time using accept*/

    while(1)
    {
        /* Wait for incoming connection. */
        client_socket = accept(connection_socket, NULL, NULL);
        if(client_socket == -1) {
            perror("Usage: lab30, accept");
            exit(EXIT_FAILURE);
        }
        printf("Accepted client\n");

        while (1)
        {
            /* Wait for next data packet. */
            if (read(client_socket, buffer, BUF_SZ) == -1) {
                perror("Usage: lab30, read");
                exit(EXIT_FAILURE);
            }

            buffer[BUF_SZ - 1] = 0;

            if(!strncmp(buffer, SHUT_SERVER, BUF_SZ)) {
                down_flag = 1;
                break;
            }

            if(!strncmp(buffer, CLOSE_CL_CMD, BUF_SZ)) {
                break;
            }
            myToUpper(buffer);
            strcpy(buffer, "");
        }

        close(client_socket);
        if(down_flag) {
            break;
        }
    }

    close(connection_socket);
    unlink(SOCKET_PATH);
    exit(EXIT_SUCCESS);
}
