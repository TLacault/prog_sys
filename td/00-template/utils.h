#ifndef _UTILS_H
#define _UTILS_H

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * print a linebreak to standard output
 */
void linebreak();

/**
 * exit printing error prefixed by `prefix` if `condition` is true (non zero)
 */
void exit_if(int condition, const char *prefix);

/**
 * Compute fibonnaci of `n` (takes time around 40 on my computer)
 */
unsigned long long fibo(int n);

/**
 * splits `argv` between `args1` or `args2`
 * if they are before or after the separator "--".
 * Returns -1 on error
 */
int split_args(char *argv[], char *args1[], char *args2[]);

int create_waiting_child(int time);

const char *signame(int signal);
void print_sigset(int fd, const sigset_t *sigset);

void print_signals(int fd);

#endif
