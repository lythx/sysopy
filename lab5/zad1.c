#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

void handler()
{
    printf("Otrzymano sygnał SIGUSR1");
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Podaj argument ignore/handler/mask/none\n");
        return 1;
    }

    const char *action = argv[1];

    if (strcmp(action, "ignore") == 0)
    {
        signal(SIGUSR1, SIG_IGN);
    }
    else if (strcmp(action, "handler") == 0)
    {
        signal(SIGUSR1, handler);
    }
    else if (strcmp(disposition_str, "mask") == 0)
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        pthread_sigmask(SIG_BLOCK, &mask, NULL);

        sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending, SIGUSR1))
        {
            printf("SIGUSR1 jest widoczny.\n");
        }
        else
        {
            printf("SIGUSR1 nie jest widoczny.\n");
        }
    }
    else if (strcmp(disposition_str, "none") != 0)
    {
        printf("Podaj argument ignore/handler/mask/none\n");
        return 1;
    }

    raise(SIGUSR1);
    printf("Sygnał SIGUSR1 został wysłany.\n");
    return 0;
}