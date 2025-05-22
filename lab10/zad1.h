#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

#define PATIENT_COUNT 20
#define PATIENT_TIME_LOW 2
#define PATIENT_TIME_HIGH 5

#define PHARMACIST_COUNT 3
#define PHARMACIST_TIME_LOW 5
#define PHARMACIST_TIME_HIGH 15

#define CONSULTATION_TIME_LOW 2
#define CONSULTATION_TIME_HIGH 4

#define DELIVERY_TIME_LOW 1
#define DELIVERY_TIME_HIGH 3

#define HOSPITAL_CAPACITY 3

#define MAX_MESSAGE_SIZE 64

typedef struct
{
    int id;
} thread_params_t;
