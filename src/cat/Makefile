CC=gcc
FLAGS= -Wall -Werror -Wextra 

all: s21_cat

s21_grep:
	$(CC) $(FLAGS) -D_GNU_SOURCE s21_cat.c -o s21_cat

rebuild: 
	make clean
	make all

clean:
	rm -rf s21_cat