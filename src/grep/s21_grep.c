// Copyright [2022] <Quicksilver Powder>
#include "s21_grep.h"

#include <fcntl.h>
#include <getopt.h>
#include <pcre.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct options {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
} GrepOpt;

void flag_f(char patterns[][strLength], int **count_tmp) {
  FILE *ptrFile = fopen(optarg, "r");
  size_t buf = 0UL;
  ssize_t length_ar = 0L;
  char *line = NULL;
  if (!ptrFile) {
    perror("File not open");
    exit(1);
  }
  while ((length_ar = getline(&line, &buf, ptrFile)) != EOF) {
    int m = 0;
    while (line[m]) {
      if (line[m] == '\n') line[m] = '\0';
      patterns[**count_tmp][m] = line[m];
      m++;
    }
    **count_tmp = **count_tmp + 1;
  }
  free(line);
  fclose(ptrFile);
}

void Parser(char patterns[][strLength], GrepOpt *opt, int argc, char *argv[],
            int *count_tmp) {
  const struct option long_options[] = {
      {"regexp", 1, NULL, 'e'},
      {"file", 1, NULL, 'f'},
      {0, 0, 0, 0},
  };
  int rez = 0;
  int option_index = 0;
  while ((rez = getopt_long(argc, argv, "ivclnhsof:e:", long_options,
                            &option_index)) != EOF) {
    switch (rez) {
      case 'i':
        opt->i = 1;
        break;
      case 'v':
        opt->v = 1;
        break;
      case 'c':
        opt->c = 1;
        break;
      case 'h':
        if (!(opt->l)) opt->h = 1;
        break;
      case 'l':
        opt->l = 1;
        opt->h = 0;
        break;
      case 'n':
        opt->n = 1;
        break;
      case 's':
        opt->s = 1;
        break;
      case 'o':
        opt->o = 1;
        break;
      case 'f': {
        flag_f(patterns, &count_tmp);
        opt->f = 1;
        break;
      };
      case 'e': {
        int m = 0;
        while (optarg[m]) {
          patterns[*count_tmp][m] = optarg[m];
          m++;
        }
        *count_tmp = *count_tmp + 1;
        opt->e = 1;
        break;
      };
      case '?':
      default: {
        printf("Try 'grep --help' for more information.\n");
      };
    }
  }
}

void PrintLine(char *line) {
  for (int i = 0; line[i] != '\0'; i++) {
    if ((line[i] != '\n') && (line[i + 1] == '\0')) {
      line[i + 1] = '\n';
      line[i + 2] = '\0';
    }
  }
  printf("%s", line);
}

void PrintFileNum(int file_count, GrepOpt opt, int str_count, char *argv[]) {
  if (file_count > 1 && !(opt.h)) printf("%s:", argv[optind]);
  if (opt.n) printf("%d:", str_count);
}

void TemplateCompile(GrepOpt *opt, char templates[][strLength], int i,
                     pcre **re) {
  int options = 0;
  const char *error = NULL;
  int erroffset = 0;
  if (opt->i) options = PCRE_CASELESS;
  *re = pcre_compile(templates[i], options, &error, &erroffset, NULL);
  if (!(*re)) {
    pcre_free(*re);
    perror("Template compile error");
    exit(1);
  }
}

void PrintC(GrepOpt *opt, int file_count, int count_matches,
            int count_not_matches, char *argv[]) {
  if (opt->c > 0) {
    if (file_count > 1 && opt->h == 0) {
      printf("%s:", argv[optind]);
    }

    if (opt->l > 0) {
      if (count_matches) {
        printf("%d\n", 1);
      } else if (!(count_matches)) {
        printf("%d\n", 0);
      }
    } else if (opt->v > 0) {
      printf("%d\n", count_not_matches);
    } else if (opt->v == 0) {
      printf("%d\n", count_matches);
    }
  }
}

void PrintL(char *argv[], int count_matches, int count_not_matches,
            GrepOpt *opt) {
  if (opt->l) {
    if (count_matches && !(opt->v)) {
      printf("%s\n", argv[optind]);
    } else if (count_not_matches && opt->v) {
      printf("%s\n", argv[optind]);
    }
  }
}

void TamplateSearch(int str_count, int file_count, char *argv[], char *line,
                    int count_tmp, GrepOpt opt, char templates[][strLength],
                    int *count_matches) {
  int len = strlen(line);
  int match = 0;
  for (int i = 0; i < count_tmp && !(match); i++) {
    pcre *re = NULL;
    TemplateCompile(&opt, templates, i, &re);
    int count = 0;
    int ovector[30] = {0};
    count = pcre_exec(re, NULL, line, len, 0, 0, ovector, 30);
    if (count > 0) {
      *count_matches = *count_matches + 1;
      match = 1;
    }
    if (match && !(opt.v)) {
      if (opt.l || opt.c) {
        pcre_free(re);
        continue;
      } else if (!(opt.o)) {
        PrintFileNum(file_count, opt, str_count, argv);
        PrintLine(line);
      } else {
        PrintFileNum(file_count, opt, str_count, argv);
        do {
          for (int i = ovector[0]; i < ovector[1]; i++) {
            printf("%c", line[i]);
          }
          printf("\n");
        } while ((count = pcre_exec(re, NULL, line, len, ovector[1], 0, ovector,
                                    30)) != EOF);
        match = 0;
      }
    } else if (opt.v) {
      if (match) {
        pcre_free(re);
        break;
      } else if (i + 1 == count_tmp) {
        if (opt.l || opt.c) {
          pcre_free(re);
          continue;
        } else {
          PrintFileNum(file_count, opt, str_count, argv);
          PrintLine(line);
        }
      }
    }
    pcre_free(re);
  }
}

void GrepPerform(int argc, char *argv[]) {
  GrepOpt opt = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int count_tmp = 0;
  char templates[strLength][strLength] = {0};
  Parser(templates, &opt, argc, argv, &count_tmp);
  int str_count, count_matches, count_not_matches, file_count = 0;
  if (!(opt.e) && !(opt.f)) {
    strcpy(templates[0], argv[optind]);
    count_tmp++;
    optind++;
  }
  for (int m = optind; argv[m]; m++) {
    file_count++;
  }
  for (int i = 0; i < file_count; i++) {
    FILE *file = fopen(argv[optind], "r");
    if (!(file) && !(opt.s)) {
      perror("grep: No such file or directory\n");
      optind++;
      continue;
    }
    count_matches = 0;
    count_not_matches = 0;
    str_count = 0;
    size_t buf = 0UL;
    ssize_t length_ar = 0L;
    char *line = NULL;
    while ((length_ar = getline(&line, &buf, file)) != EOF) {
      if (line) {
        str_count++;
        TamplateSearch(str_count, file_count, argv, line, count_tmp, opt,
                       templates, &count_matches);
      } else {
        free(line);
        perror("not enogh memmory");
        exit(1);
      }
    }
    fclose(file);
    free(line);
    count_not_matches = str_count - count_matches;
    PrintC(&opt, file_count, count_matches, count_not_matches, argv);
    PrintL(argv, count_matches, count_not_matches, &opt);
    optind++;
  }
}

int main(int argc, char *argv[]) {
  (void)argv;
  if (argc == 1) {
    perror(
        "usage: grep [-abcDEFGHhIiJLlmnOoqRSsUVvwxZ] [-A num] [-B num] "
        "[-C[num]]\n"
        "\t[-e pattern] [-f file] [--binary-files=value] [--color=when]\n"
        "\t[--context[=num]] [--directories=action] [--label] "
        "[--line-buffered]\n"
        "\t[--null] [pattern] [file ...]\n");

  } else if (argc == 2 && **argv != '-') {
    exit(1);
  } else {
    GrepPerform(argc, argv);
  }
  return 0;
}
