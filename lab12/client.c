#include "defs.h"
#include <signal.h>

void *message_handler_thread(void *arg);
void *input_handler_thread(void *arg);

void handle_message(app_message *message, app_client *client);

void handle_list_command(app_client *client);
void handle_2all_command(app_client *client);
void handle_2one_command(app_client *client);
void handle_stop_command(app_client *client);

void broadcast_stop();

pthread_mutex_t stdout_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t stop_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t stop_cond = PTHREAD_COND_INITIALIZER;
int stop = 0;

int client_socket_fd;

void sigint_handler(int sig)
{
    broadcast_stop();
}

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);

    if (argc != 4)
    {
        printf("Podaj nazwę, adres IP i port\n");
        return 1;
    }
    char *name = argv[1];
    char *ip_address = argv[2];
    int port = atoi(argv[3]);

    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    struct in_addr in_addr_val;
    if (inet_pton(AF_INET, ip_address, &in_addr_val) == -1)
    {
        printf("Błędny adres IP\n");
        return 1;
    }

    client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket_fd == -1)
    {
        printf("Error on socket(): %d\n", errno);
        return errno;
    }

    struct sockaddr_in server_addr_struct;
    server_addr_struct.sin_family = AF_INET;
    server_addr_struct.sin_port = htons(port);
    server_addr_struct.sin_addr = in_addr_val;
    socklen_t server_addr_length = sizeof(server_addr_struct);

    if (connect(client_socket_fd, (struct sockaddr *)&server_addr_struct, server_addr_length) == -1)
    {
        printf("Error on connect(): %d\n", errno);
        return errno;
    }

    app_client client;
    client.addr = server_addr_struct;
    client.addr_len = server_addr_length;
    strcpy(client.name, name);

    pthread_t message_handler_thread_id;
    if (pthread_create(&message_handler_thread_id, NULL, message_handler_thread, (void *)&client) != 0)
    {
        printf("Error on pthread_create(message_handler_thread): %d", errno);
    }

    app_message message;
    message.type = MTYPE_INIT;
    strcpy(message.sender, name);
    if (send(client_socket_fd, &message, sizeof(app_message), 0) == -1)
    {
        printf("Error on initial send(): %d\n", errno);
        return errno;
    }

    pthread_t input_handler_thread_id;
    if (pthread_create(&input_handler_thread_id, NULL, input_handler_thread, (void *)&client) != 0)
    {
        printf("Error on pthread_create(input_handler_thread): %d", errno);
    }

    pthread_mutex_lock(&stop_mutex);
    while (stop == 0)
    {
        pthread_cond_wait(&stop_cond, &stop_mutex);
    }
    handle_stop_command(&client);
    pthread_mutex_unlock(&stop_mutex);

    pthread_cancel(message_handler_thread_id);
    pthread_cancel(input_handler_thread_id);
    close(client_socket_fd);
    return 0;
}

void *message_handler_thread(void *arg)
{
    app_client *client = (app_client *)arg;
    app_message response;
    while (1)
    {
        ssize_t bytes_read = recv(client_socket_fd, &response, sizeof(app_message), 0);
        if (bytes_read == -1)
        {
            printf("Error on recv(): %d\n", errno);
        }
        else if (bytes_read != 0)
        {
            handle_message(&response, client);
        }
    }
    return NULL;
}

void *input_handler_thread(void *arg)
{
    char command[8];
    app_client *client = (app_client *)arg;

    while (1)
    {
        scanf("%s", command);
        if (strcmp(command, "STOP") == 0)
        {
            broadcast_stop();
            break;
        }
        else if (strcmp(command, "LIST") == 0)
        {
            handle_list_command(client);
        }
        else if (strcmp(command, "2ALL") == 0)
        {
            handle_2all_command(client);
        }
        else if (strcmp(command, "2ONE") == 0)
        {
            handle_2one_command(client);
        }
        else
        {
            printf("Operacje: LIST, 2ALL, 2ONE, STOP\n");
        }
    }
    return NULL;
}

void handle_message(app_message *message, app_client *client)
{
    pthread_mutex_lock(&stdout_mutex);
    switch (message->type)
    {
    case MTYPE_INIT:
        printf("Połączono z serwerem.\n");
        break;
    case MTYPE_LIST:
        printf("Lista użytkowników:\n%s", message->text);
        break;
    case MTYPE_2ALL:
        printf("[%s] %s\n", message->sender, message->text);
        break;
    case MTYPE_2ONE:
        printf("[%s -> %s] %s\n", message->sender, message->recipient, message->text);
        break;
    case MTYPE_ALIVE:
        app_message alive_message;
        alive_message.type = MTYPE_ALIVE;
        if (send(client_socket_fd, &alive_message, sizeof(app_message), 0) == -1)
        {
            printf("Error when handling ALIVE message: %d\n", errno);
        }
        break;
    default:
        printf("Unhandled message type in handle_message()\n");
        break;
    }
    pthread_mutex_unlock(&stdout_mutex);
}

void send_message(app_client *client, app_message *message)
{
    if (send(client_socket_fd, message, sizeof(app_message), 0) == -1)
    {
        printf("Error on send_message(): %d\n", errno);
    }
}

void handle_list_command(app_client *client)
{
    app_message message;
    message.type = MTYPE_LIST;
    strcpy(message.sender, client->name);
    send_message(client, &message);
}

void handle_2all_command(app_client *client)
{
    pthread_mutex_lock(&stdout_mutex);

    app_message message;
    message.type = MTYPE_2ALL;
    strcpy(message.sender, client->name);

    printf("Tekst wiadomości: ");
    scanf("%s", message.text);

    send_message(client, &message);

    pthread_mutex_unlock(&stdout_mutex);
}

void handle_2one_command(app_client *client)
{
    pthread_mutex_lock(&stdout_mutex);

    app_message message;
    message.type = MTYPE_2ONE;
    strcpy(message.sender, client->name);

    printf("Odbiorca: ");
    scanf("%s", message.recipient);

    printf("Tekst wiadomości: ");
    scanf("%s", message.text);

    send_message(client, &message);

    pthread_mutex_unlock(&stdout_mutex);
}

void handle_stop_command(app_client *client)
{
    app_message message;
    message.type = MTYPE_STOP;
    strcpy(message.sender, client->name);

    send_message(client, &message);
}

void broadcast_stop()
{
    pthread_mutex_lock(&stop_mutex);
    stop = 1;
    pthread_cond_broadcast(&stop_cond);
    pthread_mutex_unlock(&stop_mutex);
}