#include "utils.h"

void handler(int sig) {
    printf(">> received signal [%s]\n", strsignal(sig));
}

int main(int ac, char **av) {
    printf("[pid] %d\n", getpid());

    struct sigaction sa;
    sigaction(SIGUSR1, NULL, &sa);

    struct sigaction my_sa = sa;
    my_sa.sa_handler = handler;
    sigaction(SIGUSR1, &my_sa, NULL);

    sigset_t set;
    sigemptyset(&set);

    while (true) {
        printf("\nwaiting to receive SIGUSR1\n");
        sigsuspend(&set);
        printf("received\n");
    }

    return EXIT_SUCCESS;
}

/*
struct sigaction {
        void (*sa_handler)(int);
        void (*sa_sigaction)(int, siginfo_t *, void *);
        sigset_t sa_mask;
        int sa_flags;
        void (*sa_restorer)(void);
};
*/
