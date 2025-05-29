#include "defs.h"

void accept_process(int socket_fd);
void receive_process(int socket_fd);

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

    if (fork() == 0)
    {
        accept_process(socket_fd);
    }
    else
    {
        receive_process(socket_fd);
    }

    return 0;
}

void accept_process(int socket_fd)
{
    while (1)
    {
        accept(socket_fd, NULL, NULL);
    }
}

void receive_process(int socket_fd)
{

    while (1)
    {
        // read(socket_fd,);
    }
}