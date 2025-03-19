int collatz_conjecture(int input) {
  return (input % 2 == 0) ? (input / 2) : (3 * input + 1);
}

int test_collatz_convergence(int input, int max_iter, int *steps) {
  int iter = 0;
  while (iter < max_iter) {
    if (input == 1) {
      return iter;
    }
    input = collatz_conjecture(input);
    steps[iter] = input;
    iter++;
  }
  return 0;
}
