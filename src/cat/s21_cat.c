// Copyright [2022] <Quicksilver Powder>
#include "s21_cat.h"

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

typedef struct options {
  int b;
  int e;
  int s;
  int n;
  int t;
  int v;
} CatOpt;

void CatNoArgs(int fd) {
  char buf[strLength] = {0};
  int bytes_read = 0;

  bytes_read = read(fd, buf, strLength);
  while (bytes_read > 0) {
    printf("%.*s", bytes_read, buf);
    bytes_read = read(fd, buf, strLength);
  }
}

void Parser(CatOpt *opt, int argc, char *argv[]) {
  const char *short_options = "sbnetv";
  const struct option long_options[] = {
      {"number-nonblanklp", 0, NULL, 'b'},
      {"number", 0, NULL, 'n'},
      {"squeeze-blank", 0, NULL, 's'},
      {0, 0, 0, 0},
  };
  int rez = 0;
  int option_index = 0;

  while ((rez = getopt_long(argc, argv, short_options, long_options,
                            &option_index)) != EOF) {
    switch (rez) {
      case 'b':
        opt->b = 1;
        break;
      case 'n':
        opt->n = 1;
        break;
      case 's':
        opt->s = 1;
        break;
      case 'v':
        opt->v = 1;
        break;
      case 'E':
        opt->e = 1;
        break;
      case 'T':
        opt->t = 1;
        break;
      case 't': {
        opt->t = 1;
        opt->v = 1;
        break;
      };
      case 'e': {
        opt->e = 1;
        opt->v = 1;
        break;
      };
      case '?':
      default: {
        printf("Try 'cat --help' for more information.\n");
      };
    }
  }
}

char const *print_un(int c) {
  char const *code[33] = {"^@", "^A",  "^B", "^C", "^D", "^E", "^F", "^G", "^H",
                          "^I", "^J",  "^K", "^L", "^M", "^N", "^O", "^P", "^Q",
                          "^R", "^S",  "^T", "^U", "^V", "^W", "^X", "^Y", "^Z",
                          "^[", "^\\", "^]", "^^", "^_", " "};
  return (code[c]);
}

void func_flags_f_t_n(CatOpt opt, int cur) {
  if (opt.v) {
    if (cur < 33 && cur != '\n' && cur != '\t') {
      printf("%s", print_un(cur));
    } else if (cur == 127) {
      printf("^?");
    } else if (cur == '\t' && opt.t) {
      printf("^I");
    } else if (cur == '\n' && opt.e) {
      printf("$");
      printf("%c", cur);
    } else {
      printf("%c", cur);
    }
  } else {
    printf("%c", cur);
  }
}

void CatArgs(int argc, char *argv[]) {
  CatOpt opt = {0, 0, 0, 0, 0, 0};
  if (*argv[1] == '-') Parser(&opt, argc, argv);
  while (optind != argc) {
    FILE *ptrFile = fopen(argv[optind], "r");
    optind++;
    if (ptrFile) {
      int cur = 0;
      int prev = 0;
      int n_count = 0;
      int str_count = 0;
      while ((cur = fgetc(ptrFile)) != EOF) {
        if (opt.s && cur == '\n') {
          n_count++;
          if (n_count > 2) {
            continue;
          }
        } else {
          n_count = 0;
        }
        if (opt.b) {
          if (prev == '\n' || str_count == 0) {
            if (cur != '\n') printf("%6d\t", ++str_count);
          }
        } else if (opt.n) {
          if (prev == '\n' || str_count == 0) printf("%6d\t", ++str_count);
        }
        func_flags_f_t_n(opt, cur);
        prev = cur;
      }
      fclose(ptrFile);
    } else {
      perror("cat: no_file.txt");
    }
  }
}

int main(int argc, char *argv[]) {
  (void)argv;
  if (argc == 1) {
    CatNoArgs(STDIN_FILENO);
  } else {
    CatArgs(argc, argv);
  }
  return 0;
}
