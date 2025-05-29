#include "defs.h"

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

    struct in_addr in_addr_val;
    if (inet_pton(AF_INET, ip_address, &in_addr_val) == -1)
    {
        printf("Błędny adres IP\n");
        return 1;
    }

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        printf("Error on socket(): %d\n", errno);
        return errno;
    }

    struct sockaddr_in addr_struct;
    addr_struct.sin_family = AF_INET;
    addr_struct.sin_port = htons(port);
    addr_struct.sin_addr = in_addr_val;
    socklen_t addr_struct_length = sizeof(addr_struct);
    if (connect(socket_fd, (struct sockaddr *)&addr_struct, addr_struct_length) == -1)
    {
        printf("Error on connect(): %d\n", errno);
        return errno;
    }
    app_message message;
    strcpy(message.client_name, name);
    if (write(socket_fd, &message, sizeof(message)) == -1)
    {
        printf("Error on write(): %d\n", errno);
        return errno;
    }
}
