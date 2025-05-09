#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>

#define SEM_PATH getenv("HOME")
#define SEM_ID 31

#define SHARED_MEM_PATH getenv("HOME")
#define SHARED_MEM_ID 30

#define SHARED_MEM_SIZE 30
#define PRINT_TEXT_SIZE 10
#define PRINT_QUEUE_SIZE 3

typedef enum
{
  SEM_AVAILABLE_PRINTERS = 0,
  SEM_PENDING_REQUESTS = 1,
  SEM_QUEUE_SPOTS = 2
} sem_type;
