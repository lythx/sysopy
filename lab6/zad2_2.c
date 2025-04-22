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

double function(double x)
{
  return 4 / (x * x + 1);
}

int main(int argc, char *argv[])
{
  struct domain d;
  int fifo = open(FIFO_NAME, O_RDWR);
  read(fifo, &d, sizeof(d));

  fflush(stdout);

  double result = 0;
  int n = 1000;
  double h = (d.b - d.a) / n;

  for (int i = 0; i < n; i++)
  {
    result += function(d.a + h * i) * h;
  }

  write(fifo, &result, sizeof(double));
  close(fifo);
  return 0;
}
