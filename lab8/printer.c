#include "common.h"

int main()
{
    char shared_buf[SHARED_MEM_SIZE];

    key_t printer_sem_key = ftok(PRINTER_SEM_PATH, PRINTER_SEM_ID);
    int printer_sem_id = semget(printer_sem_key, 1, 0666);
    if (printer_sem_id == -1)
    {
        printf("Error when accessing printer semaphore: %d", errno);
        return errno;
    }

    key_t request_sem_key = ftok(REQUEST_SEM_PATH, REQUEST_SEM_ID);
    int request_sem_id = semget(request_sem_key, 1, 0666);
    if (request_sem_id == -1)
    {
        printf("Error when accessing request semaphore: %d", errno);
        return errno;
    }

    key_t shared_mem_key = ftok(SHARED_MEM_PATH, SHARED_MEM_ID);
    int shared_mem_id = shmget(shared_mem_key, SHARED_MEM_SIZE, 0666);
    if (shared_mem_id == -1)
    {
        printf("Error when accessing shared memory: %d", errno);
        return errno;
    }
    *shared_buf = shmat(shared_mem_id, NULL, 0);

    struct sembuf op = {0};

    while (1)
    {
        op.sem_num = 1;
        op.sem_op = 1;
        op.sem_flg = 0;
        semop(printer_sem_id, &op, 1);

        op.sem_num = 1;
        op.sem_op = -1;
        op.sem_flg = 0;
        semop(request_sem_id, &op, 1);

        sleep(1);
    }
}