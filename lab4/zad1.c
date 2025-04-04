#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("Prosze podac ilosc procesow do utworzenia\n");
    return 1;
  }
  int process_count = atoi(argv[1]);
  for (int i = 0; i < process_count; i++) {
    pid_t child_pid = fork();
    // if (child_pid == 0) {
      printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
    //   return 0;
    // }
    // wait(NULL);
  }
  printf("argv1: %s\n", argv[1]);
  return 0;
}
