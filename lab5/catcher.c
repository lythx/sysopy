#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int received_signals = 0;
int mode = 0;

void sigint_handler(int sig)
{
  printf("Wciśnięto CTRL+C\n");
}

void sigusr1_handler(int sig, siginfo_t *info, void *_)
{
  received_signals++;
  int new_mode = info->si_value.sival_int;
  if (new_mode < 1 || new_mode > 5)
  {
    return;
  }
  mode = new_mode;
  kill(info->si_pid, SIGUSR1);
}

int main()
{
  printf("Catcher PID: %d\n", getpid());

  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = sigusr1_handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGUSR1, &sa, NULL);

  int prev_mode = mode;

  pause();
  while (1)
  {
    if (prev_mode != mode) {
      
      continue;
    }
    printf("Catcher przyjal SIGUSR1 z trybem %d\n", mode);
    if (mode == 1)
    {
      printf("Liczba żądań zmiany pracy: %d\n", received_signals);
      pause();
    }
    else if (mode == 2)
    {
      for (int i = 1; mode == 2; i++)
      {
        printf("%d\n", i);
        sleep(1);
      }
    }
    else if (mode == 3)
    {
      signal(SIGINT, SIG_IGN);
      pause();
    }
    else if (mode == 4)
    {
      signal(SIGINT, sigint_handler);
      pause();
    }
    else if (mode == 5)
    {
      return 0;
    }
  }
}