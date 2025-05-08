#include "common.h"

int main()
{
    setbuf(stdout, NULL);

    int printer_count = 10;
    int client_count = 20;

    key_t printer_sem_key = ftok(PRINTER_SEM_PATH, PRINTER_SEM_ID);
    int printer_sem_id = semget(printer_sem_key, 1, 0666 | IPC_CREAT | IPC_EXCL);
    if (printer_sem_id == -1)
    {
        printf("Error when creating printer semaphore: %d", errno);
        return errno;
    }
    semctl(printer_sem_id, 0, SETVAL, printer_count);

    key_t request_sem_key = ftok(REQUEST_SEM_PATH, REQUEST_SEM_ID);
    int request_sem_id = semget(request_sem_key, 1, 0666 | IPC_CREAT | IPC_EXCL);
    if (request_sem_id == -1)
    {
        printf("Error when creating request semaphore: %d", errno);
        return errno;
    }
    semctl(request_sem_id, 0, SETVAL, 0);

    key_t shared_mem_key = ftok(SHARED_MEM_PATH, SHARED_MEM_ID);
    int shared_mem_id = shmget(shared_mem_key, SHARED_MEM_SIZE, 0666 | IPC_CREAT | IPC_EXCL);
    if (shared_mem_id == -1)
    {
        printf("Error when creating shared memory: %d", errno);
        return errno;
    }

    for (int i = 0; i < printer_count; i++)
    {

        if (fork() == 0)
        {
            execl("printer", "printer", NULL);
        }
    }

    for (int i = 0; i < client_count; i++)
    {
        if (fork() == 0)
        {
            execl("client", "client", NULL);
        }
    }

    while (1)
    {
    }

    return 0;
}
