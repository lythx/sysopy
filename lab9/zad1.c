#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct
{
    double x;
    double h;
    double b;
    int i;
    int k;
} calculate_params_t;

#define MAX_THREADS 256

double results[MAX_THREADS] = {0};
int ready[MAX_THREADS] = {0};

double function(double x)
{
    return 4 / (x * x + 1);
}

void *calculate(void *arg);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Podaj szerokość prostokąta i ilość wątków\n");
        return 1;
    }

    double a = 0.0;
    double b = 1.0;

    double h = atof(argv[1]);
    int k = atoi(argv[2]);

    int thread_count = 0;
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    double x = a;
    for (int i = 0; i < k && x < b; i++)
    {
        pthread_t thread_id;
        calculate_params_t *params = malloc(sizeof(calculate_params_t));
        if (params == NULL)
        {
            printf("Params malloc failed");
            return 1;
        }
        params->x = x;
        params->b = b;
        params->h = h;
        params->i = i;
        params->k = k;
        if (pthread_create(&thread_id, NULL, calculate, (void *)params) != 0)
        {
            printf("Failed to create thread %d. errno: %d", i, errno);
            return errno;
        }
        x += h;
        thread_count++;
    }

    while (1)
    {
        int all_ready = 1;
        for (int i = 0; i < thread_count; i++)
        {
            if (!ready[i])
            {
                all_ready = 0;
                break;
            }
        }
        if (all_ready)
        {
            break;
        }
    }

    double result = 0.0;
    for (int i = 0; i < thread_count; i++)
    {
        result += results[i];
    }

    gettimeofday(&end_time, NULL);
    long long int elapsed_time_us = (end_time.tv_sec - start_time.tv_sec) * 1000000LL + end_time.tv_usec - start_time.tv_usec;
    printf("Wynik: %f, Czas wykonania: %lld ms\n", result, elapsed_time_us / 1000);
    return 0;
}

void *calculate(void *arg)
{
    calculate_params_t *params = (calculate_params_t *)arg;
    double x = params->x;
    double b = params->b;
    double h = params->h;
    int k = params->k;
    int i = params->i;
    double result = 0.0;
    while (x + h / 2 < b)
    {
        result += function(x + h / 2) * h;
        x += k * h;
    }
    results[i] = result;
    ready[i] = 1;
    free(arg);
    return NULL;
}