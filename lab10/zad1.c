#include "zad1.h"

void *patient(void *arg);
void *pharmacist(void *arg);
void *doctor(void *arg);

int main(void)
{
    srand(time(NULL));
    for (int i = 0; i < PATIENT_COUNT; i++)
    {
        pthread_t thread_id;
        int *id = malloc(sizeof(int));
        *id = i;
        if (pthread_create(&thread_id, NULL, patient, (void *)id) != 0)
        {
            printf("Failed to create patient %d. errno: %d", i, errno);
            return errno;
        }
    }
    for (int i = 0; i < PHARMACIST_COUNT; i++)
    {
        pthread_t thread_id;
        int *id = malloc(sizeof(int));
        *id = i;
        if (pthread_create(&thread_id, NULL, pharmacist, (void *)id) != 0)
        {
            printf("Failed to create pharmacist %d. errno: %d", i, errno);
            return errno;
        }
    }
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, doctor, NULL) != 0)
    {
        printf("Failed to create doctor. errno: %d", errno);
        return errno;
    }

    while (1)
    {
    }

    return 0;
}

void *patient(void *arg)
{
    int id = *(int *)arg;
    char *name = "Pacjent";
    char message[MAX_MESSAGE_SIZE];
    int wait_time = rand_min_max(PATIENT_TIME_LOW, PATIENT_TIME_HIGH);
    sprintf(message, "ide do szpitala, bede za %d s", wait_time);
    say(name, id, message);
    sleep(wait_time);
    while (1)
    {
        int wait_time = rand_min_max(PATIENT_TIME_LOW, PATIENT_TIME_HIGH);
        sprintf(message, "za dużo pacjentów, wracam później za %d s", wait_time);
        say(name, id, message);
        sleep(wait_time);
    }
    sprintf(message, "czeka %d pacjentów na lekarza", 0);
    say(name, id, message);
    if (1)
    {
        say(name, id, "budzę lekarza");
    }
    say(name, id, "kończę wizytę");
    return NULL;
}

void *pharmacist(void *arg)
{
    int id = *(int *)arg;
    char *name = "Farmaceuta";
    char message[MAX_MESSAGE_SIZE];
    int wait_time = rand_min_max(PHARMACIST_TIME_LOW, PHARMACIST_TIME_HIGH);
    sprintf(message, "ide do szpitala, bede za %d s", wait_time);
    say(name, id, message);
    sleep(wait_time);
    if (1)
    {
        say(name, id, "czekam na oproznienie apteczki");
    }
    say(name, id, "budzę lekarza");
    say(name, id, "kończę wizytę");
    say(name, id, "zakończyłem dostawę");
    return NULL;
}

void *doctor(void *arg)
{
    char *name = "Lekarz";
    char message[MAX_MESSAGE_SIZE];
    while (1)
    {
        say(name, -1, "budzę się");
        if (1)
        {
            sprintf(message, "konsultuję pacjentów %d, %d, %d", 0, 1, 2);
            say(name, -1, message);
            int consultation_time = rand_min_max(CONSULTATION_TIME_LOW, CONSULTATION_TIME_HIGH);
            sleep(consultation_time);
        }
        else
        {
            say(name, -1, "przyjmuję dostawę leków");
            int delivery_time = rand_min_max(DELIVERY_TIME_LOW, DELIVERY_TIME_HIGH);
            sleep(delivery_time);
        }
        say(name, -1, "zasypiam");
    }

    return NULL;
}

void say(char *name, int id, char *message)
{
    if (id == -1)
    {
        printf("[%d] - %s: %s\n", 0, name, message);
    }
    else
    {
        printf("[%d] - %s(%d): %s\n", 0, name, id, message);
    }
}

int rand_min_max(int low, int high)
{
    return low + (rand() % (high + 1 - low));
}
