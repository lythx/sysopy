#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX_CLIENTS 32
#define MAX_CLIENT_NAME_LENGTH 128

typedef enum
{
    OP_LIST = 0,
    OP_TO_ALL = 1,
    OP_TO_ONE = 2,
    OP_STOP = 3,
    OP_ALIVE = 4
} app_message_type;

typedef struct
{
    char client_name[MAX_CLIENT_NAME_LENGTH];
    app_message_type message_type;
} app_message;

typedef struct
{
    uint32_t address;
    in_port_t port;
    int socket_fd;
    char client_name[MAX_CLIENT_NAME_LENGTH];
} app_client;
