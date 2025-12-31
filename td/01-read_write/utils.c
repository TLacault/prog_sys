#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void linebreak() {
    write(STDOUT_FILENO, "\n", 1);
}

void exit_if(int condition, const char *prefix) {
    if (condition) {
        if (errno != 0) {
            perror(prefix);
        } else {
            fprintf(stderr, "%s\n", prefix);
        }
        exit(1);
    }
}

long long fibo(int n) {
    if (n < 2) return n;
    return fibo(n - 1) + fibo(n - 2);
}

int index_of(const char *str, const char *table[]) {
    int i = 0;
    while (table[i]) {
        if (strcmp(str, table[i]) == 0)
            return 0;
        i++;
    }
    return -1;
}

int split_args(char *argv[], char *args1[], char *args2[]) {
    char **current = args1;
    while (*argv && strcmp("--", *argv)) {
        *current++ = *argv++;
    }
    *current = NULL;
    if (*argv == NULL || args1 == current)
        return -1;

    argv++;  // On saute "--"

    current = args2;
    while (*argv) {
        *current++ = *argv++;
    }
    *current = NULL;
    return args2 == current ? -1 : 0;
}

int create_waiting_child(int time) {
    int pid = fork();
    exit_if(pid == -1, "fork");
    if (pid == 0) {
        sleep(10);
        exit(0);
    }
    return pid;
}

static const char *signames[] = {
    "SIGHUP",
    "SIGINT",
    "SIGQUIT",
    "SIGILL",
    "SIGTRAP",
    "SIGABRT",
    "SIGEMT",
    "SIGFPE",
    "SIGKILL",
    "SIGBUS",
    "SIGSEGV",
    "SIGSYS",
    "SIGPIPE",
    "SIGALRM",
    "SIGTERM",
    "SIGURG",
    "SIGSTOP",
    "SIGTSTP",
    "SIGCONT",
    "SIGCHLD",
    "SIGTTIN",
    "SIGTTOU",
    "SIGIO",
    "SIGXCPU",
    "SIGXFSZ",
    "SIGVTALRM",
    "SIGPROF",
    "SIGWINCH",
    "SIGINFO",
    "SIGUSR1",
    "SIGUSR2",
};

const char *signame(int signal) {
    if (signal >= SIGHUP && signal <= SIGUSR2)
        return signames[signal - SIGHUP];
    return "SIG???";
}

#define SIG_BUFFER_SIZE 512

void print_sigset(int fd, const sigset_t *sigset) {
    int i;
    char buffer[SIG_BUFFER_SIZE + 1];
    buffer[SIG_BUFFER_SIZE] = 0;
    char *current = buffer;
#define REMAINING (SIG_BUFFER_SIZE - (current - buffer))

    current += snprintf(current, REMAINING, "signals: ");
    for (i = SIGHUP; i <= SIGUSR2; i++) {
        if (sigismember(sigset, i))
            current += snprintf(current, REMAINING,
                                "%s, ", signame(i));
    }
    current += snprintf(current, REMAINING, "\n");
    write(fd, buffer, current - buffer);
#undef REMAINING
}

void print_signals(int fd) {
    sigset_t buffer;

    sigpending(&buffer);
    write(fd, "pending ", 8);
    print_sigset(fd, &buffer);

    sigprocmask(0, NULL, &buffer);  // read suspended signals
    write(fd, "blocked ", 8);
    print_sigset(fd, &buffer);
}
