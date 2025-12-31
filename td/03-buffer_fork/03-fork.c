#include "utils.h"

int main(int ac, char** av) {
    pid_t pid;

    exit_if(signal(SIGCHLD, SIG_IGN) == SIG_ERR, "signal");
    printf("[Parent PID] %jd\n", (intmax_t)getpid());
    pid = fork();

    switch (pid) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            printf("[Child PID] %jd\n", (intmax_t)getpid());
            puts("Child exiting.");
            fflush(stdout);
            _exit(EXIT_SUCCESS);
        default:
            printf("[Child PID] %jd\n", (intmax_t)pid);
            puts("Parent exiting.");
            exit(EXIT_SUCCESS);
    }

    return EXIT_SUCCESS;
}

/*
    Dans le processus parent, après le fork, on a le PID du processus enfant dans `pid`.
    Dans le processus enfant, après le fork, on a 0 dans `pid`.
    En ignorant le signal SIGCHLD, on évite la création de processus zombies lorsque
    les processus enfants terminent leur exécution.
*/
