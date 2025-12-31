#include "utils.h"

enum {
    READ,
    WRITE
};

#ifdef SIMPLER_VERSION
char *ls[] = { "ls", "-l", NULL };
char *tr[] = { "tr", "-d", "[:blank:]", NULL };
char **cmd[] = { ls, tr };
#else
char *ls[] = { "ls", "-l", NULL };
char *grep[] = { "grep", ".c", NULL };
char *wc[] = { "wc", "-l", NULL };
char **cmd[] = { ls, grep, wc };
#endif

int NB_COMMAND = sizeof(cmd) / sizeof(char *);
int NB_TUBE = sizeof(cmd) / sizeof(char *) - 1;
int tubes[sizeof(cmd) / sizeof(char *) - 1][2];
int current_command = 0;

void debug_input() {
    if (current_command == 0) return;

    char buffer;
    ssize_t byte_read;
    fprintf(stderr, ">>>>  SHOW INPUT:\n");
    while ((byte_read = read(STDIN_FILENO, &buffer, 1)) > 0) {
        fprintf(stderr, "%c", buffer);
    }
    lseek(STDIN_FILENO, 0, SEEK_SET);  // reset stdin
}

void redirect_output() {
    fprintf(stderr, ">>>> current_command %d NB_TUBE %d\n", current_command, NB_TUBE);

    if (current_command > 0) {
        fprintf(stderr, ">>>> TUBE[%d] : READ  -> STDIN\n", current_command - 1);
        dup2(tubes[current_command - 1][READ], STDIN_FILENO);
        // if (current_command == 2) debug_input();
    }

    if (current_command < NB_TUBE) {
        fprintf(stderr, ">>>> TUBE[%d] : WRITE -> STDOUT\n", current_command);
        dup2(tubes[current_command][WRITE], STDOUT_FILENO);
    }
}

void close_pipe() {
    if (current_command > 0)
        close(tubes[current_command - 1][READ]);

    if (current_command < NB_TUBE)
        close(tubes[current_command][WRITE]);
}

int status_command() {
    int status;
    pid_t pid = wait(&status);
    (void)pid;

    if (WIFEXITED(status)) {
        fprintf(stderr, ">>>> command exited with status %d\n", WEXITSTATUS(status));
        return WEXITSTATUS(status);
    } else {
        fprintf(stderr, ">>>> %s ends with error\n", cmd[current_command][0]);
        return EXIT_FAILURE;
    }
}

void exec_command() {
    pid_t pid = fork();
    exit_if(pid == -1, "fork");

    if (pid == 0) {
        fprintf(stderr, "\n[child %d] executing command [%s]\n", getpid(), cmd[current_command][0]);
        redirect_output();
        execvp(cmd[current_command][0], &cmd[current_command][0]);
        _exit(EXIT_SUCCESS);
    }
}

int main(int ac, char **av) {
    // open the pipes for sub process
    for (int i = 0; i < NB_COMMAND - 1; ++i)
        exit_if(pipe(tubes[i]) == -1, "pipe");

    // exec command
    for (int i = 0; i < NB_COMMAND; ++i) {
        exec_command();
        close_pipe();
        exit_if(status_command() != EXIT_SUCCESS, "[error] after running command %d");
        current_command++;
    }

    return EXIT_SUCCESS;
}
