#include "create_cpp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int print_help() {
  printf("Create a cpp file unit\n");
  printf("Usage : create_cpp <filename>\n");

  return 0;
}

int create_cpp_file_unit(const char* name) {
  printf("Creating cpp file unit named : %s\n", name);

  struct stat st = {0};

  if (stat("out", &st) == -1) {
      mkdir("out", 0700);
  }
  {
    char buffer[100];
    int cx;  // FIXME Check return value.
    cx = snprintf(buffer, 100, "out/%s.cpp", name);

    FILE* fp;

    fp = fopen(buffer, "w+");
    fprintf(fp, "#include \"%s.h\"\n", name);

    fclose(fp);
  }
  {
    char file_open_buf[100];
    int cx;  // FIXME Check return value.
    cx = snprintf(file_open_buf, 100, "out/%s.h", name);

    char file_name_buf[100];
    cx = snprintf(file_name_buf, 100, "%s", name);
    // put name to uppercase
    int j = 0;
    while (file_name_buf[j]) {
      file_name_buf[j] = toupper(file_name_buf[j]);
      j++;
    }

    FILE* fp;

    fp = fopen(file_open_buf, "w+");
    fprintf(fp, "#ifndef %s_H\n", file_name_buf);
    fprintf(fp, "#define %s_H\n\n", file_name_buf);
    fprintf(fp, "#endif //%s_H\n", file_name_buf);

    fclose(fp);
  }
  return 0;
}

int run_from_args(int argc, char** argv) {
  if (argc == 2) {
    if (strcmp(argv[1], "-h") == 0) {
      return print_help();
    }

    return create_cpp_file_unit(argv[1]);
  }

  print_help();
  return 1;
}
