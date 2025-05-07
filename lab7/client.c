#include "defs.h"

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);
    key_t server_queue_key = ftok(SERVER_PATH, SERVER_ID);
    int server_queue_id = msgget(server_queue_key, 0666);
    if (server_queue_id == -1)
    {
        printf("Error when accessing server queue: %d\n", errno);
        return errno;
    }

    key_t client_queue_key = ftok(SERVER_PATH, getpid());
    int client_queue_id = msgget(client_queue_key, IPC_CREAT | 0666);
    if (client_queue_id == -1)
    {
        printf("Error when creating client queue %d\n", errno);
        return errno;
    }

    message_t message;
    message.client_queue_key = client_queue_key;
    message.mtype = MTYPE_INIT;
    if (msgsnd(server_queue_id, &message, MESSAGE_SIZE, 0) == -1)
    {
        printf("Error when sending init message %d\n", errno);
        return errno;
    }

    if (msgrcv(client_queue_id, &message, MESSAGE_SIZE, MTYPE_REGISTERED, 0) == -1)
    {
        printf("Error when receiving init response %d\n", errno);
        return errno;
    }
    int client_id = message.client_id;

    if (fork() == 0)
    {
        while (1)
        {
            if (msgrcv(client_queue_id, &message, MESSAGE_SIZE, 0, 0) == -1)
            {
                printf("Error when receiving message %d\n", errno);
                continue;
            }
            switch (message.mtype)
            {
            case MTYPE_SERVER_MESSAGE:
                printf("[%d]: %s\n", message.client_id, message.text);
                break;
            case MTYPE_NEW_CLIENT:
                printf("Client %d joined the chat\n", message.client_id);
            }
        }
    }
    else
    {
        while (1)
        {
            scanf("%s", message.text);
            message.client_id = client_id;
            message.mtype = MTYPE_CLIENT_MESSAGE;
            if (msgsnd(server_queue_id, &message, MESSAGE_SIZE, 0) == -1)
            {
                printf("Error when sending message %d\n", errno);
                return errno;
            }
        }
    }
    return 0;
}
