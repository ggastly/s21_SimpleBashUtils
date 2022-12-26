#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#define strLength 1024

typedef struct options CatOpt;
void CatNoArgs(int fd);
void Parser(CatOpt *opt, int argc, char *argv[]);
char const *print_un(int c);
void func_flags_f_t_n(CatOpt opt, int cur);
void CatArgs(int argc, char *argv[]);
int main(int argc, char *argv[]);

#endif  // SRC_CAT_S21_CAT_H_
