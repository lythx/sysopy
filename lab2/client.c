#include <stdio.h>
#include <dlfcn.h>

#ifndef DYNAMIC
  #define DYNAMIC 0
#endif

#if !DYNAMIC
  extern int collatz_conjecture(int input);
  extern int test_collatz_convergence(int input, int max_iter, int *steps);
#endif

#define LIMIT 70

int main() {
  int (*test_collatz_convergence_dynamic)(int input, int max_iter, int *steps);
  void* handle;
  if (DYNAMIC) {
    handle = dlopen("./libcollatz.so.1", RTLD_LAZY);
    if (!handle) {
      printf("dlopen error\n");
      return 1;
    }

    
    test_collatz_convergence_dynamic = dlsym(handle, "test_collatz_convergence");

    if (dlerror() != NULL) {
      printf("dlerror");
      return 1;
    }
  }

  int steps[LIMIT] = {0};
  int nums[4] = {145, 17, 19, 43458439};
  for (int i = 0; i < 4; i++) {
    printf("liczba: %d, limit krokow: %d\n", nums[i], LIMIT);
    int step_num;
    #if DYNAMIC
      step_num = test_collatz_convergence_dynamic(nums[i], LIMIT, steps);
    #else
      step_num = test_collatz_convergence(nums[i], LIMIT, steps);
    #endif
    if (step_num == 0) {
      printf("wyczerpano limit krokow\n");
    } 
    else {
      for (int i = 0; i < step_num; i++) {
        printf("%d\n", steps[i]);
      }
    }
  }

  #if DYNAMIC
    dlclose(handle);
  #endif

  return 0;
}
