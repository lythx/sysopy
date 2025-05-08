#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define PRINTER_SEM_PATH getenv("HOME")
#define PRINTER_SEM_ID 0

#define REQUEST_SEM_PATH getenv("HOME")
#define REQUEST_SEM_ID 1

#define SHARED_MEM_PATH getenv("HOME")
#define SHARED_MEM_ID 2

#define SHARED_MEM_SIZE 65536

#define MAX_MESSAGE_TEXT 256
#define MAX_CLIENTS 32

typedef enum
{
  MTYPE_INIT = 1,
  MTYPE_REGISTERED = 2,
  MTYPE_CLIENT_MESSAGE = 3,
  MTYPE_SERVER_MESSAGE = 4,
  MTYPE_NEW_CLIENT = 5
} message_type;

typedef struct
{
  long mtype;
  int client_id;
  key_t client_queue_key;
  char text[MAX_MESSAGE_TEXT];
} message_t;

#define MESSAGE_SIZE (sizeof(message_t) - sizeof(long))
