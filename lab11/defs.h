#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#define MAX_CLIENTS 32
#define MAX_CLIENT_NAME_LENGTH 128
#define MAX_QUEUED_MESSAGES 128
#define MAX_TEXT_LENGTH 4096
#define ALIVE_CHECK_INTERVAL_SECONDS 10

typedef enum
{
    MTYPE_LIST = 0,
    MTYPE_2ALL = 1,
    MTYPE_2ONE = 2,
    MTYPE_STOP = 3,
    MTYPE_ALIVE = 4,
    MTYPE_INIT = 5
} app_message_type;

typedef struct
{
    app_message_type type;
    char sender[MAX_CLIENT_NAME_LENGTH];
    char recipient[MAX_CLIENT_NAME_LENGTH];
    char text[MAX_TEXT_LENGTH];
} app_message;

typedef struct
{
    uint32_t address;
    in_port_t port;
    int socket_fd;
    char name[MAX_CLIENT_NAME_LENGTH];
    int alive_confirmed;
    int deleted;
    int initialized;
} app_client;
