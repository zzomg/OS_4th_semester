#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <errno.h>
#include <aio.h>
#include <signal.h>

#define MAX_N_CLIENTS 32
#define LISTEN_BACKLOG 50
#define SOCKET_PATH "/tmp/1.socket"

#define BUF_SZ 256

#define CLOSE_CL_CMD "/END"
#define SHUT_SERVER "/DOWN"

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct aiocb** aiocb_list;
int n_clients = 0;

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

static void shutServer()
{
    for (int i = 0; i < n_clients; i++) {
        aio_cancel(aiocb_list[i]->aio_fildes, NULL);
        close(aiocb_list[i]->aio_fildes);
        free(aiocb_list[i]);
        aiocb_list[i] = NULL;
    }

    free(aiocb_list);
    unlink(SOCKET_PATH);
    exit(EXIT_SUCCESS);
}

static void closeClient(struct aiocb* req)
{
    for(int i = 0; i < n_clients; ++i) {
        if (aiocb_list[i] == req) {
            aio_cancel(aiocb_list[i]->aio_fildes, NULL);
            close(aiocb_list[i]->aio_fildes);
            aiocb_list[i] = NULL;
            for(int j = i; j < n_clients; ++j) {
                aiocb_list[j] = aiocb_list[j + 1];
            }
            --n_clients;

            free(req);
        }
    }
}

static void aio_sig_handler(int sig, siginfo_t *info, void *ucontext)
{
    struct aiocb* req;

    printf("Reading complete\n");
    if(sig != SIGIO) {
        return;
    }

    req = (struct aiocb*)info->si_value.sival_ptr;
    if(aio_error(req) == 0) {
        size_t size = aio_return(req);
        if(size == 0 || (!strncmp((char*)req->aio_buf, CLOSE_CL_CMD, BUF_SZ))) {
            printf("eof\n");
            closeClient(req);
            return;
        }
        if(!strncmp((char*)req->aio_buf, SHUT_SERVER, BUF_SZ)) {
            shutServer();
        }
        myToUpper((char*)req->aio_buf);
        strcpy((char*)req->aio_buf, ""); // ?
    }
    aio_read(req);
}

void init_server_socket(int* connection_socket)
{
    int server_len;
    struct sockaddr_un server_address;

    unlink(SOCKET_PATH);

    (*connection_socket) = socket(AF_UNIX, SOCK_STREAM, 0);
    if ((*connection_socket) == -1) {
        perror("Usage: lab32, socket");
        exit(EXIT_FAILURE);
    }

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, SOCKET_PATH);
    server_len = sizeof(server_address);

    if(bind((*connection_socket), (struct sockaddr*) &server_address, (socklen_t)server_len) == -1 ) {
        perror("Usage: lab32, bind");
        exit(EXIT_FAILURE);
    }
    if(listen((*connection_socket), LISTEN_BACKLOG) == -1) {
        perror("Usage: lab32, listen");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[])
{
    int connection_socket, client_socket;

    char buffer[BUF_SZ] = {0};

    init_server_socket(&connection_socket);

    aiocb_list = (struct aiocb**)calloc(MAX_N_CLIENTS, sizeof(struct aiocb*));
    if(aiocb_list == NULL) {
        errExit("Usage, lab32, calloc");
    }

    struct sigaction sigio_handle_reaction;
    memset(&sigio_handle_reaction, 0, sizeof(sigio_handle_reaction));
    sigio_handle_reaction.sa_sigaction = aio_sig_handler;
    sigio_handle_reaction.sa_flags = SA_SIGINFO;
    if(sigaction(SIGIO, &sigio_handle_reaction, NULL) == -1) {
        errExit("Usage, lab32, sigaction");
    }

    while (1) {
        printf("Waiting for client (%d total)...\n", n_clients);

        client_socket = accept(connection_socket, NULL, NULL);
        if(client_socket == -1) {
            continue;
        } else {
            printf("Adding client on fd %d\n", client_socket);

            struct aiocb* client = (struct aiocb*)calloc(1, sizeof(struct aiocb));
            if(client == NULL) {
                errExit("Usage, lab32, calloc");
            }

            client->aio_fildes = client_socket;
            client->aio_offset = 0;
            client->aio_buf = buffer;
            client->aio_nbytes = BUF_SZ;
            client->aio_sigevent.sigev_notify  = SIGEV_SIGNAL;
            client->aio_sigevent.sigev_signo = SIGIO;
            client->aio_sigevent.sigev_value.sival_ptr = client;

            int client_num;
            for (client_num = 0; aiocb_list[client_num]; ++client_num);
            printf("client num : %d\n", client_num);
            if (client_num == MAX_N_CLIENTS){
                printf("Clients limit reached. Try again later\n");
                continue;
            }

            aiocb_list[client_num] = client;

            ++n_clients;
        }

        printf("Reading...\n");
        for(int i = 0; i < n_clients; ++i) {
            if(aio_read(aiocb_list[i]) != 0) {
                closeClient(aiocb_list[i]);
                errExit("Usage: lab32, aio_read");
            }
        }
    }
}
