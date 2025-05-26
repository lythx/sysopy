#include "zad1.h"

int patients = 0;
int patient_ids[3] = {0};
pthread_mutex_t patients_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t patients_cond = PTHREAD_COND_INITIALIZER;

int medkits = 0;
pthread_mutex_t medkits_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t medkits_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t pharmacist_mutex = PTHREAD_MUTEX_INITIALIZER;

int visit = 0;
pthread_mutex_t visit_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t visit_cond = PTHREAD_COND_INITIALIZER;

int initial_time;

void *patient(void *arg);
void *pharmacist(void *arg);
void *doctor(void *arg);

void say(char *name, int id, char *message);
int rand_min_max(int low, int high);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Podaj liczbę pacjentów i farmaceutów");
        return 1;
    }

    int patient_count = atoi(argv[1]);
    int pharmacist_count = atoi(argv[2]);

    srand(time(NULL));
    initial_time = time(NULL);
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, doctor, NULL) != 0)
    {
        printf("Failed to create doctor. errno: %d", errno);
        return errno;
    }
    for (int i = 0; i < patient_count; i++)
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
    for (int i = 0; i < pharmacist_count; i++)
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
        pthread_mutex_lock(&patients_mutex);
        if (patients < HOSPITAL_CAPACITY)
        {
            patient_ids[patients] = id;
            patients++;
            break;
        }

        int wait_time = rand_min_max(PATIENT_TIME_LOW, PATIENT_TIME_HIGH);
        sprintf(message, "za dużo pacjentów, wracam później za %d s", wait_time);
        say(name, id, message);
        pthread_mutex_unlock(&patients_mutex);
        sleep(wait_time);
    }
    sprintf(message, "czeka %d pacjentów na lekarza", patients);
    say(name, id, message);

    if (patients == HOSPITAL_CAPACITY)
    {
        pthread_mutex_unlock(&patients_mutex);
        pthread_mutex_lock(&medkits_mutex);
        while (medkits < HOSPITAL_CAPACITY)
        {
            pthread_cond_wait(&medkits_cond, &medkits_mutex);
        }
        pthread_mutex_unlock(&medkits_mutex);

        pthread_mutex_lock(&visit_mutex);
        while (visit)
        {
            pthread_cond_wait(&visit_cond, &visit_mutex);
        }
        say(name, id, "budzę lekarza");
        visit = 1;
        pthread_cond_broadcast(&visit_cond);
    }
    else
    {
        pthread_mutex_unlock(&patients_mutex);
    }

    while (visit || patients != 0)
    {
        pthread_cond_wait(&visit_cond, &visit_mutex);
    }
    pthread_mutex_unlock(&visit_mutex);

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

    pthread_mutex_lock(&medkits_mutex);
    if (medkits >= HOSPITAL_CAPACITY)
    {
        say(name, id, "czekam na oproznienie apteczki");
    }
    pthread_mutex_unlock(&medkits_mutex);

    pthread_mutex_lock(&pharmacist_mutex);
    pthread_mutex_lock(&medkits_mutex);
    while (medkits >= HOSPITAL_CAPACITY)
    {
        pthread_cond_wait(&medkits_cond, &medkits_mutex);
    }

    pthread_mutex_lock(&visit_mutex);
    while (visit)
    {
        pthread_cond_wait(&visit_cond, &visit_mutex);
    }

    say(name, id, "budzę lekarza");
    visit = 1;
    pthread_cond_broadcast(&visit_cond);

    say(name, id, "dostarczam leki");
    medkits = MEDKIT_CAPACITY;
    pthread_cond_broadcast(&medkits_cond);
    pthread_mutex_unlock(&medkits_mutex);

    while (visit)
    {
        pthread_cond_wait(&visit_cond, &visit_mutex);
    }
    pthread_mutex_unlock(&visit_mutex);
    pthread_mutex_unlock(&pharmacist_mutex);

    say(name, id, "zakończyłem dostawę");
    return NULL;
}

void *doctor(void *arg)
{
    char *name = "Lekarz";
    char message[MAX_MESSAGE_SIZE];
    while (1)
    {
        pthread_mutex_lock(&visit_mutex);
        while (!visit)
        {
            pthread_cond_wait(&visit_cond, &visit_mutex);
        }

        say(name, -1, "przyjmuję dostawę leków");
        int delivery_time = rand_min_max(DELIVERY_TIME_LOW, DELIVERY_TIME_HIGH);
        sleep(delivery_time);

        say(name, -1, "zasypiam");
        visit = 0;
        pthread_cond_broadcast(&visit_cond);
        pthread_mutex_unlock(&visit_mutex);

        while (1)
        {
            pthread_mutex_lock(&visit_mutex);
            while (!visit)
            {
                pthread_cond_wait(&visit_cond, &visit_mutex);
            }
            say(name, -1, "budzę się");

            sprintf(message, "konsultuję pacjentów %d, %d, %d", patient_ids[0], patient_ids[1], patient_ids[2]);
            say(name, -1, message);
            int consultation_time = rand_min_max(CONSULTATION_TIME_LOW, CONSULTATION_TIME_HIGH);
            sleep(consultation_time);

            pthread_mutex_lock(&medkits_mutex);
            medkits -= 3;
            pthread_cond_broadcast(&medkits_cond);
            pthread_mutex_unlock(&medkits_mutex);

            pthread_mutex_lock(&patients_mutex);
            patients = 0;
            pthread_cond_broadcast(&patients_cond);
            pthread_mutex_unlock(&patients_mutex);

            say(name, -1, "zasypiam");
            visit = 0;
            pthread_cond_broadcast(&visit_cond);
            pthread_mutex_unlock(&visit_mutex);

            pthread_mutex_lock(&medkits_mutex);
            if (medkits < HOSPITAL_CAPACITY)
            {
                pthread_mutex_unlock(&medkits_mutex);
                break;
            }
            pthread_mutex_unlock(&medkits_mutex);
        }
    }

    return NULL;
}

void say(char *name, int id, char *message)
{
    if (id == -1)
    {
        printf("[%ld] - %s: %s\n", time(NULL) - initial_time, name, message);
    }
    else
    {
        printf("[%ld] - %s(%d): %s\n", time(NULL) - initial_time, name, id, message);
    }
}

int rand_min_max(int low, int high)
{
    return low + (rand() % (high + 1 - low));
}
