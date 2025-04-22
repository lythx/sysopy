#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

#define BUF_SIZE 50

double function(double x)
{
  return 4 / (x * x + 1);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Podaj szerokość prostokąta i ilość procesów\n");
    return 1;
  }

  double a = 0.0;
  double b = 1.0;

  double h = atof(argv[1]);
  int n = atoi(argv[2]);

  int *read_streams = malloc(n * sizeof(int));

  char buf[BUF_SIZE];
  int write_stream = 0;

  for (int k = 1; k <= n; k++)
  {
    int stream_count = 0;
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    double x = a;
    for (int i = 0; i < k && x < b; i++)
    {
      int fd[2];
      pipe(fd);
      pid_t pid = fork();
      if (pid == 0)
      { // Child wychodzi z petli
        close(fd[0]);
        write_stream = fd[1];
        break;
      }
      else
      { // Parent zapisuje read_stream i kontynuuje petle
        close(fd[1]);
        read_streams[i] = fd[0];
      }
      x += h;
      stream_count++;
    }

    double result = 0.0;
    // Child oblicza wartosc funkcji dla punktow x, x + kh, x + 2kh, ...
    if (write_stream != 0)
    {
      while (x + h / 2 < b)
      {
        result += function(x + h / 2) * h;
        x += k * h;
      }
      int buf_size = sprintf(buf, "%f", result);
      write(write_stream, buf, buf_size);
      close(write_stream);
      return 0;
    }
    // Parent zlicza sume calkowa
    for (int i = 0; i < stream_count; i++)
    {
      read(read_streams[i], buf, BUF_SIZE);
      result += atof(buf);
      close(read_streams[i]);
    }

    gettimeofday(&end_time, NULL);
    long long int elapsed_time_us = (end_time.tv_sec - start_time.tv_sec) * 1000000LL + end_time.tv_usec - start_time.tv_usec;
    printf("Wynik: %f, Czas wykonania: %lld ms\n", result, elapsed_time_us / 1000);
  }
  return 0;
}