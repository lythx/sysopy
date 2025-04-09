#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s <catcher_pid> <tryb_pracy>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t catcher_pid = atoi(argv[1]);
    int mode = atoi(argv[2]);

    printf("Sender [%d]: Wysyłam sygnał SIGUSR1 z trybem %d do catcher [%d]...\n", getpid(), mode, catcher_pid);

    union sigval sv;
    sv.sival_int = mode;
    if (sigqueue(catcher_pid, SIGUSR1, sv) == -1) {
        perror("Sender: sigqueue (wysłanie trybu)");
        exit(EXIT_FAILURE);
    }

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    printf("Sender [%d]: Czekam na potwierdzenie od catcher [%d]...\n", getpid(), catcher_pid);
    if (sigsuspend(&mask) == -1 && errno != EINTR) {
        perror("Sender: sigsuspend (oczekiwanie na potwierdzenie)");
        exit(EXIT_FAILURE);
    }

    printf("Sender [%d]: Otrzymałem potwierdzenie od catcher [%d]. Kończę pracę.\n", getpid(), catcher_pid);

    return 0;
}