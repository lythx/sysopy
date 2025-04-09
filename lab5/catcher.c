#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int received_signals = 0;
pid_t sender_pid = 0;

void sigint_handler(int sig)
{
  printf("Wciśnięto CTRL+C\n");
}

void sigusr1_handler(int sig, siginfo_t *info, void* _)
{
  received_signals++;
  int mode = info->si_value.sival_int;
  printf("Catcher przyjal SIGUSER1 z trybem %d\n", mode);
  kill(sender_pid, SIGUSR1);
  if (mode == 1)
  {
    printf("Liczba żądań zmiany pracy: %d\n", received_signals);
  }
  else if (mode == 2)
  {
    int initial_received_signals = received_signals;
    for (int i = 0;; i++)
    {
      if (initial_received_signals != received_signals)
      {
        break;
      }
      printf("%d\n", i);
    }
  }
  else if (mode == 3)
  {
    signal(SIGINT, SIG_IGN);
  }
  else if (mode == 4)
  {
    signal(SIGINT, sigint_handler);
  }
  else if (mode == 5)
  {
    exit(0);
  }
}

int main()
{
  printf("Catcher PID: %d\n", getpid());

  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = sigusr1_handler;
  sigaction(SIGUSR1, &sa, NULL);

  return 0;
}