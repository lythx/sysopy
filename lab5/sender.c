#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Podaj PID catchera i tryb pracy\n");
    return 1;
  }

  pid_t catcher_pid = atoi(argv[1]);
  int mode = atoi(argv[2]);

  printf("Sender wysyła SIGUSR1 z trybem %d\n", mode);

  union sigval sv;
  sv.sival_int = mode;
  sigqueue(catcher_pid, SIGUSR1, sv);

  sigset_t suspend_mask;
  sigfillset(&suspend_mask);
  sigdelset(&suspend_mask, SIGUSR1);
  sigdelset(&suspend_mask, SIGINT);
  sigsuspend(&suspend_mask);

  printf("Sender otrzymał potwierdzenie\n");

  return 0;
}
