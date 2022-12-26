#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_
#include <fcntl.h>
#include <getopt.h>
#include <pcre.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define strLength 1024

typedef struct options GrepOpt;
void GrepNoArgs(int fd);
void Parser(char patterns[][1024], GrepOpt *opt, int argc, char *argv[],
            int *count_tmp);
void PrintLine(char *line);
void PrintFileNum(int file_count, GrepOpt opt, int str_count, char *argv[]);
void TemplateCompile(GrepOpt *opt, char templates[][1024], int i, pcre **re);
void PrintC(GrepOpt *opt, int file_count, int count_matches,
            int count_not_matches, char *argv[]);
void PrintL(char *argv[], int count_matches, int count_not_matches,
            GrepOpt *opt);
void GrepPerform(int argc, char *argv[]);
void TamplateSearch(int str_count, int file_count, char *argv[], char *line,
                    int count_tmp, GrepOpt opt, char templates[][1024],
                    int *count_matches);

#endif  // SRC_GREP_S21_GREP_H_
