#include <stdio.h>

int main() {
  int steps[200] = {0};
  int result = test_collatz_convergence(27, 96, steps);
  for (int i = 0; i < result; i++) {
    printf("%d\n", steps[i]);
  }
}
