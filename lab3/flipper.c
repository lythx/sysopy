#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_NAME_LEN 256
#define MAX_FILE_LEN 65536

int main() {
  char in_dirname[MAX_NAME_LEN], out_dirname[MAX_NAME_LEN];

  printf("Podaj katalog zrodlowy:\n");
  fgets(in_dirname, MAX_NAME_LEN, stdin);
  in_dirname[strcspn(in_dirname, "\n")] = 0;

  printf("Podaj katalog wynikowy:\n");
  fgets(out_dirname, MAX_NAME_LEN, stdin);
  out_dirname[strcspn(out_dirname, "\n")] = 0;

  DIR* in_dir = opendir(in_dirname);
  if (in_dir == NULL) {
    printf("Nie udalo sie otworzyc folderu zrodlowego");
    return 1;
  }

  struct stat st = {0};
  if (stat(out_dirname, &st) == -1) {
    mkdir(out_dirname, 0700);
  }

  char in_path[2 * MAX_NAME_LEN + 1], out_path[2 * MAX_NAME_LEN + 1], chunk[MAX_FILE_LEN];
  struct dirent *df;
  FILE *in_file, *out_file;
  char *dot;
  while ((df = readdir(in_dir))) {
    dot = strrchr(df->d_name, '.');
    if (!dot || strcmp(dot, ".txt")) {
      continue;
    }

    strcpy(in_path, in_dirname);
    strcat(in_path, "/");
    strcat(in_path, df->d_name);
    in_file = fopen(in_path, "r+");
    if (in_file == NULL) {
      printf("Nie udalo sie otworzyc pliku zrodlowego %s\n", in_path);
      continue;
    }

    strcpy(out_path, out_dirname);
    strcat(out_path, "/");
    strcat(out_path, df->d_name);
    out_file = fopen(out_path, "w+");
    if (out_file == NULL) {
      printf("Nie udalo sie utworzyc pliku docelowego %s\n", out_path);
      continue;
    }
  
    size_t file_size = fread(chunk, sizeof(char), MAX_FILE_LEN, in_file);
    size_t i = 0, j = 0, start, end;
    char temp;
    while (i < file_size) {
      while (j < file_size) {
        if (chunk[j] == '\n') {
          break;
        }
        j++;
      }

      start = i;
      end = j;
      j--;
      while (i < end - (end - start) / 2) {
        temp = chunk[i];
        chunk[i] = chunk[j];
        chunk[j] = temp;
        i++;
        j--;
      }
      j = end + 1;
      i = j;
    }

    fwrite(chunk, sizeof(char), file_size, out_file);
    fclose(in_file);
    fclose(out_file);
  }

  closedir(in_dir);

  return 0;
}
