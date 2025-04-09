#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int received_signals = 0;
int current_mode = 1;
pid_t sender_pid = 0;
int ctrl_c_pressed = 0;

void sigusr1_handler(int sig, siginfo_t *info, void *ucontext) {
    if (info->si_pid > 0) {
        sender_pid = info->si_pid;
    }
    received_signals++;
    printf("Catcher [%d]: Otrzymałem SIGUSR1 od %d. Potwierdzam.\n", getpid(), sender_pid);
    kill(sender_pid, SIGUSR1);
}

void sigint_handler(int sig) {
    if (current_mode == 4) {
        printf("Catcher [%d]: Wciśnięto CTRL+C\n", getpid());
    }
    ctrl_c_pressed = 1;
}

int main() {
    printf("Catcher PID: %d\n", getpid());

    struct sigaction sa_usr1;
    sa_usr1.sa_flags = SA_SIGINFO;
    sa_usr1.sa_sigaction = sigusr1_handler;
    sigemptyset(&sa_usr1.sa_mask);
    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("Catcher: sigaction (SIGUSR1)");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa_int;
    sa_int.sa_handler = sigint_handler;
    sa_int.sa_flags = 0;
    sigemptyset(&sa_int.sa_mask);
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("Catcher: sigaction (SIGINT)");
        exit(EXIT_FAILURE);
    }

    while (current_mode != 5) {
        printf("Catcher [%d]: Aktualny tryb pracy: %d\n", getpid(), current_mode);
        if (current_mode == 1) {
            printf("Catcher [%d]: Liczba otrzymanych żądań zmiany trybu: %d\n", getpid(), received_signals - (current_mode == 5 ? 1 : 0));
        } else if (current_mode == 2) {
            for (int i = 1; current_mode == 2; ++i) {
                printf("Catcher [%d]: Kolejna liczba: %d\n", getpid(), i);
                sleep(1);
            }
        } else if (current_mode == 3) {
            sa_int.sa_handler = SIG_IGN;
            if (sigaction(SIGINT, &sa_int, NULL) == -1) {
                perror("Catcher: sigaction (SIGINT - ignore)");
                exit(EXIT_FAILURE);
            }
            printf("Catcher [%d]: Ignoruję CTRL+C.\n", getpid());
        } else if (current_mode == 4) {
            sa_int.sa_handler = sigint_handler;
            if (sigaction(SIGINT, &sa_int, NULL) == -1) {
                perror("Catcher: sigaction (SIGINT - handler)");
                exit(EXIT_FAILURE);
            }
            printf("Catcher [%d]: Obsługuję CTRL+C.\n", getpid());
        }

        sigset_t empty_mask;
        sigemptyset(&empty_mask);
        sigsuspend(&empty_mask); // Czekaj na sygnały

        if (sender_pid > 0) {
            sigval_t received_value;
            siginfo_t info;
            if (sigwaitinfo(&empty_mask, &info) > 0 && info.si_signo == SIGUSR1) {
                current_mode = info.si_value.sival_int;
                printf("Catcher [%d]: Otrzymałem żądanie zmiany trybu na: %d od %d.\n", getpid(), current_mode, sender_pid);
                if (current_mode == 5) {
                    printf("Catcher [%d]: Kończę pracę.\n", getpid());
                    break;
                }
            }
            sender_pid = 0; // Resetuj PID sendera po odebraniu sygnału zmiany trybu
        }
    }

    return 0;
}