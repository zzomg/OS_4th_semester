#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/1.socket"
#define BUF_SZ 256

#define CLOSE_CL_CMD "/END"
#define SHUT_SERVER "/DOWN"

int main(int argc, char* argv[])
{
    struct sockaddr_un addr;

    int server_socket;

    char buffer[BUF_SZ] = {0};

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Usage: lab30, socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

    if (connect(server_socket, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) == -1) {
        perror("Server is down");
        exit(EXIT_FAILURE);
    }

    while(1) {
        scanf("%s", buffer);
        if(write(server_socket, buffer, strlen(buffer) + 1) == -1) {
            perror("Usage: lab30, write");
            break;
        }
        if(strcmp(buffer, CLOSE_CL_CMD) == 0 || strcmp(buffer, SHUT_SERVER) == 0) {
            break;
        }
        strcpy(buffer, "");
    }

    close(server_socket);
    exit(EXIT_SUCCESS);
}
