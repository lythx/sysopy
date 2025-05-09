#include "common.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Podaj liczbę drukarek i klientów");
    }

    setbuf(stdout, NULL);

    int printer_count = atoi(argv[1]);
    int client_count = atoi(argv[2]);

    key_t sem_key = ftok(SEM_PATH, SEM_ID);
    int sem_id = semget(sem_key, 3, 0666 | IPC_CREAT);
    if (sem_id == -1)
    {
        printf("Error when creating semaphores: %d\n", errno);
        return errno;
    }
    semctl(sem_id, SEM_AVAILABLE_PRINTERS, SETVAL, 0);
    semctl(sem_id, SEM_PENDING_REQUESTS, SETVAL, 0);
    semctl(sem_id, SEM_QUEUE_SPOTS, SETVAL, PRINT_QUEUE_SIZE);

    key_t shared_mem_key = ftok(SHARED_MEM_PATH, SHARED_MEM_ID);
    int shared_mem_id = shmget(shared_mem_key, SHARED_MEM_SIZE, 0666 | IPC_CREAT);
    if (shared_mem_id == -1)
    {
        printf("Error when creating shared memory: %d\n", errno);
        return errno;
    }

    char *shared_buf;
    shared_buf = shmat(shared_mem_id, NULL, 0);
    strcpy(shared_buf, "\0");

    char id_buf[16];
    for (int i = 0; i < printer_count; i++)
    {
        sprintf(id_buf, "%d", i);
        if (fork() == 0)
        {
            execl("printer", "printer", id_buf, NULL);
        }
    }

    for (int i = 0; i < client_count; i++)
    {
        sprintf(id_buf, "%d", i);
        if (fork() == 0)
        {
            execl("client", "client", id_buf, NULL);
        }
    }

    while (1)
    {
    }

    return 0;
}
