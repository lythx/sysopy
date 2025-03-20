#include <stdio.h>
#include <dirent.h>

#define MAX_DIRNAME_LEN 200
#define MAX_LINE_LEN 2000

int main() {
  char rpath[MAX_DIRNAME_LEN], wpath[MAX_DIRNAME_LEN];
  char line[MAX_LINE_LEN];
  printf("Podaj katalog zrodlowy:\n");
  fgets(rpath, MAX_DIRNAME_LEN, stdin);
  rpath[strcspn(rpath, "\n")] = 0;
  printf("Podaj katalog wynikowy:\n");
  fgets(wpath, MAX_DIRNAME_LEN, stdin);
  wpath[strcspn(wpath, "\n")] = 0;

  DIR* rdir = opendir(rpath);

  if (rdir == NULL) {
    printf("Nie udalo sie otworzyc folderu zrodlowego");
    return 1;
  }

  struct dirent* df;
  FILE* file;
  while (df = readdir(rdir)) {
    file = fopen(df->d_name, "r+");
    
  }



  return 0;
}