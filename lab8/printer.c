#include "common.h"

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);

    int printer_id = argc < 2 ? getpid() : atoi(argv[1]);

    key_t sem_key = ftok(SEM_PATH, SEM_ID);
    int sem_id = semget(sem_key, 0, 0666);
    if (sem_id == -1)
    {
        printf("Error when accessing semaphores: %d\n", errno);
        return errno;
    }

    key_t shared_mem_key = ftok(SHARED_MEM_PATH, SHARED_MEM_ID);
    int shared_mem_id = shmget(shared_mem_key, SHARED_MEM_SIZE, 0666);
    if (shared_mem_id == -1)
    {
        printf("Error when accessing shared memory: %d\n", errno);
        return errno;
    }
    char *shared_buf;
    shared_buf = shmat(shared_mem_id, NULL, 0);

    char text_to_print[PRINT_TEXT_SIZE];

    struct sembuf op_queue_inc = {0};
    op_queue_inc.sem_num = SEM_QUEUE_SPOTS;
    op_queue_inc.sem_flg = 0;
    op_queue_inc.sem_op = 1;

    struct sembuf op_printer_inc = {0};
    op_printer_inc.sem_num = SEM_AVAILABLE_PRINTERS;
    op_printer_inc.sem_flg = 0;
    op_printer_inc.sem_op = 1;

    struct sembuf op_request_dec = {0};
    op_request_dec.sem_num = SEM_PENDING_REQUESTS;
    op_request_dec.sem_flg = 0;
    op_request_dec.sem_op = -1;

    while (1)
    {
        semop(sem_id, &op_printer_inc, 1);
        semop(sem_id, &op_request_dec, 1);
        semop(sem_id, &op_queue_inc, 1);

        strncpy(text_to_print, shared_buf, PRINT_TEXT_SIZE);
        text_to_print[PRINT_TEXT_SIZE] = '\0';

        memmove(shared_buf, shared_buf + PRINT_TEXT_SIZE, strlen(shared_buf) - PRINT_TEXT_SIZE + 1);

        for (int i = 0; i < PRINT_TEXT_SIZE; i++)
        {
            printf("[Printer %d] %c\n", printer_id, text_to_print[i]);
            sleep(1);
        }
    }
}