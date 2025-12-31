#include "utils.h"

void gen_segfault() {
    *((int*)0) = 42;
}

// -----------------------------------------------------------------------------

void print_command(int ac, char** av) {
    for (int i = 2; i < ac; i++) {
        write(STDOUT_FILENO, av[i], strlen(av[i]));
        write(STDOUT_FILENO, " ", 1);
    }
    write(STDOUT_FILENO, "> ", 2);
    write(STDOUT_FILENO, av[1], strlen(av[1]));
    write(STDOUT_FILENO, "\n", 1);
}

void redirect_output(char* output) {
    int fdout = open(output, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
    exit_if(fdout == -1, "open redirect output");
    dup2(fdout, STDOUT_FILENO);
    close(fdout);
}

void exec_command(char** av) {
    pid_t pid = fork();
    switch (pid) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            redirect_output(av[1]);
            // gen_segfault();
            execvp(av[2], &av[2]);
            _exit(EXIT_SUCCESS);
        default:
            break;
    }
}

int status_command(char* cmd) {
    int status;
    pid_t p2 = wait(&status);

    if (WIFEXITED(status)) {
        printf("command exited with status %d\n", WEXITSTATUS(status));
        return WEXITSTATUS(status);
    } else {
        printf("%s ends with error\n", cmd);
        return EXIT_FAILURE;
    }
}

int main(int ac, char** av) {
    exit_if(ac < 3, "usage: ./run <sortie> <command> [arg1] [arg2] ...");
    print_command(ac, av);
    exec_command(av);
    exit_if(status_command(av[2]) != EXIT_SUCCESS, "[error] after running command");
    return EXIT_SUCCESS;
}

// make 04-run && clear && ./04-run output.txt ls -la

/*
    Avec gen_segfault() dans le child process
    $ ./04-run output.txt ls -la
    ls -la > output.txt
    ls ends with error
    [error] after running command
*/
