#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define FIFO_NAME "./fifo2"

struct domain
{
  double a;
  double b;
};

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Podaj przedział całkowania\n");
    return 1;
  }

  double a = atof(argv[1]);
  double b = atof(argv[2]);

  mkfifo(FIFO_NAME, S_IRWXU);

  int fifo;
  struct domain d;
  d.a = a;
  d.b = b;

  fifo = open(FIFO_NAME, O_RDWR);
  write(fifo, &d, sizeof(d));
  fflush(stdin);

  double result;
  read(fifo, &result, sizeof(double));
  printf("Wynik: %f\n", result);

  close(fifo);

  return 0;
}
