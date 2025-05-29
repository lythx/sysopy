#include "defs.h"

int client_count = 0;
app_client clients[MAX_CLIENTS] = {0};

void accept_process(int socket_fd);
void receive_process(app_client *client);

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
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        printf("Error on socket(): %d\n", errno);
        return errno;
    }
    if (bind(socket_fd, (struct sockaddr *)&addr_struct, sizeof(addr_struct)) == -1)
    {
        printf("Error on bind(): %d\n", errno);
        return errno;
    }
    if (listen(socket_fd, 64) == -1)
    {
        printf("Error on listen(): %d\n", errno);
        return errno;
    }

    accept_process(socket_fd);

    return 0;
}

void accept_process(int socket_fd)
{
    struct sockaddr_in addr_struct;
    socklen_t addr_struct_length = sizeof(addr_struct);
    while (1)
    {
        int client_socket_fd = accept(socket_fd, (struct sockaddr *)&addr_struct, &addr_struct_length);
        if (client_socket_fd == -1)
        {
            printf("Error on accept(): %d\n", errno);
            continue;
        }

        app_client client;
        client.address = addr_struct.sin_addr.s_addr;
        client.port = addr_struct.sin_port;
        client.socket_fd = client_socket_fd;
        clients[client_count] = client;
        if (fork() == 0)
        {
            receive_process(&client);
        }
        client_count++;
    }
}

void receive_process(app_client *client)
{
    app_message message;
    while (1)
    {
        ssize_t bytes_read = recv(client->socket_fd, &message, sizeof(message), MSG_WAITALL);
        if (bytes_read == -1)
        {
            printf("Error on recv(): %d\n", errno);
            continue;
        }
        else if (bytes_read == 0)
        {
            continue;
        }
        switch (message.message_type)
        {
        case MTYPE_INIT:

            break;

        default:
            break;
        }
        printf("RECEIVE SUCCESS %s", message.client_name);
    }
}