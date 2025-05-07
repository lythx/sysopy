#include "defs.h"

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);

    int client_count = 0;
    int client_queues[MAX_CLIENTS] = {0};

    key_t server_queue_key = ftok(SERVER_PATH, SERVER_ID);
    int server_queue_id = msgget(server_queue_key, IPC_CREAT | 0666);

    message_t client_message, server_message;

    while (1)
    {
        if (msgrcv(server_queue_id, &client_message, MESSAGE_SIZE, 0, 0) == -1)
        {
            printf("Error when receiving message %d\n", errno);
            continue;
        }
        switch (client_message.mtype)
        {
        case MTYPE_INIT:
            if (client_count == MAX_CLIENTS)
            {
                printf("Error when registering client. Max amount of clients reached\n");
                continue;
            }

            int client_id = client_count;
            int client_queue_id = msgget(client_message.client_queue_key, 0666);
            if (client_queue_id == -1)
            {
                printf("Error when accessing client queue %d\n", errno);
                continue;
            }

            server_message.mtype = MTYPE_REGISTERED;
            server_message.client_id = client_id;
            if (msgsnd(client_queue_id, &server_message, MESSAGE_SIZE, 0) == -1)
            {
                printf("Error when accessing client queue %d\n", errno);
                continue;
            }

            printf("Client %d joined the chat\n", client_id);
            server_message.mtype = MTYPE_NEW_CLIENT;
            server_message.client_id = client_id;

            for (int i = 0; i < client_count; i++)
            {
                if (msgsnd(client_queues[i], &server_message, MESSAGE_SIZE, 0) == -1)
                {
                    printf("Error when sending message to client id=%d errno=%d\n", client_message.client_id, errno);
                }
            }

            client_queues[client_count] = client_queue_id;
            client_count++;
            break;
        case MTYPE_CLIENT_MESSAGE:
            printf("[%d]: %s\n", client_message.client_id, client_message.text);

            server_message.mtype = MTYPE_SERVER_MESSAGE;
            strcpy(server_message.text, client_message.text);
            server_message.client_id = client_message.client_id;

            for (int i = 0; i < client_count; i++)
            {
                if (i == client_message.client_id)
                {
                    continue;
                }

                if (msgsnd(client_queues[i], &server_message, MESSAGE_SIZE, 0) == -1)
                {
                    printf("Error when sending message to client id=%d errno=%d\n", client_message.client_id, errno);
                }
            }
        }
    }

    return 0;
}