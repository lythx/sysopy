#include "defs.h"

int client_count = 0;
app_client clients[MAX_CLIENTS] = {0};
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *request_handler_thread(void *arg);
void *alive_checker_thread(void *arg);

void send_message(app_client *client, app_message *message);
void handle_request(app_client *client, app_message *request);

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);

    if (argc != 3)
    {
        printf("Podaj adres IP i port\n");
        return 1;
    }
    char *ip_address = argv[1];
    int port = atoi(argv[2]);

    struct in_addr in_addr_val;
    if (inet_pton(AF_INET, ip_address, &in_addr_val) == -1)
    {
        printf("Błędny adres IP\n");
        return 1;
    }

    struct sockaddr_in addr_struct;
    addr_struct.sin_family = AF_INET;
    addr_struct.sin_port = htons(port);
    addr_struct.sin_addr = in_addr_val;
    socklen_t addr_struct_length = sizeof(addr_struct);
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        printf("Error on socket(): %d\n", errno);
        return errno;
    }
    if (bind(socket_fd, (struct sockaddr *)&addr_struct, addr_struct_length) == -1)
    {
        printf("Error on bind(): %d\n", errno);
        return errno;
    }
    if (listen(socket_fd, 64) == -1)
    {
        printf("Error on listen(): %d\n", errno);
        return errno;
    }

    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, alive_checker_thread, NULL) != 0)
    {
        printf("Error on pthread_create(alive_checker_thread): %d", errno);
    }

    while (1)
    {
        int client_socket_fd = accept(socket_fd, (struct sockaddr *)&addr_struct, &addr_struct_length);
        if (client_socket_fd == -1)
        {
            printf("Error on accept(): %d\n", errno);
            continue;
        }

        app_client *client = &clients[client_count];
        client->address = addr_struct.sin_addr.s_addr;
        client->port = addr_struct.sin_port;
        client->socket_fd = client_socket_fd;
        client->alive_confirmed = 1;
        client->deleted = 0;
        client->initialized = 0;

        client_count++;

        if (pthread_create(&thread_id, NULL, request_handler_thread, (void *)client) != 0)
        {
            printf("Error on pthread_create(request_handler_thread): %d", errno);
        }
    }

    return 0;
}

void *request_handler_thread(void *arg)
{
    app_client *client = (app_client *)arg;
    app_message request;
    while (!client->deleted)
    {
        ssize_t bytes_read = recv(client->socket_fd, &request, sizeof(app_message), MSG_WAITALL);
        if (bytes_read == -1)
        {
            printf("Error on recv(): %d\n", errno);
        }
        else if (bytes_read != 0)
        {
            handle_request(client, &request);
        }
    }
    return NULL;
}

void handle_request(app_client *client, app_message *request)
{
    pthread_mutex_lock(&clients_mutex);
    app_message response;
    switch (request->type)
    {
    case MTYPE_INIT:
        strcpy(client->name, request->sender);
        client->initialized = 1;

        response.type = MTYPE_INIT;
        strcpy(response.sender, request->sender);
        strcpy(response.recipient, request->sender);
        strcpy(response.text, "");

        send_message(client, &response);
        break;
    case MTYPE_LIST:
        response.type = MTYPE_LIST;
        strcpy(response.sender, request->sender);
        strcpy(response.recipient, request->sender);

        strcpy(response.text, "");
        for (int i = 0; i < client_count; i++)
        {
            if (!clients[i].deleted && clients[i].initialized)
            {
                strcat(response.text, clients[i].name);
                strcat(response.text, "\n");
            }
        }

        send_message(client, &response);
        break;
    case MTYPE_2ALL:
        response.type = MTYPE_2ALL;
        strcpy(response.sender, request->sender);
        strcpy(response.text, request->text);

        for (int i = 0; i < client_count; i++)
        {
            if (&clients[i] != client && !clients[i].deleted && clients[i].initialized)
            {
                strcpy(response.recipient, clients[i].name);
                send_message(&clients[i], &response);
            }
        }
        break;
    case MTYPE_2ONE:
        response.type = MTYPE_2ONE;
        strcpy(response.sender, request->sender);
        strcpy(response.recipient, request->recipient);
        strcpy(response.text, request->text);

        for (int i = 0; i < client_count; i++)
        {
            if (strcmp(clients[i].name, response.recipient) == 0 && !clients[i].deleted && clients[i].initialized)
            {
                send_message(&clients[i], &response);
            }
        }

        break;
    case MTYPE_STOP:
        client->deleted = 1;
        break;
    case MTYPE_ALIVE:
        client->alive_confirmed = 1;
        break;
    default:
        printf("Unhandled message type in handle_request()");
        break;
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *alive_checker_thread(void *arg)
{
    app_message message;
    while (1)
    {
        pthread_mutex_lock(&clients_mutex);
        for (int i = client_count - 1; i >= 0; i--)
        {
            if (clients[i].deleted || !clients[i].initialized)
            {
                continue;
            }
            if (!clients[i].alive_confirmed)
            {
                clients[i].deleted = 1;
                continue;
            }
            clients[i].alive_confirmed = 0;
            message.type = MTYPE_ALIVE;
            send_message(&clients[i], &message);
        }
        pthread_mutex_unlock(&clients_mutex);
        sleep(ALIVE_CHECK_INTERVAL_SECONDS);
    }
}

void send_message(app_client *client, app_message *message)
{
    if (write(client->socket_fd, message, sizeof(app_message)) == -1)
    {
        printf("Error on send_message(): %d\n", errno);
    }
}
