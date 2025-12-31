#include "utils.h"

enum {
    READ,
    WRITE
};

#define CHAINE "Hello World!"
#define SIZE (strlen(CHAINE))

int main(int ac, char** av) {
    int tube[2];
    pipe(tube);

    pid_t pid = fork();
    exit_if(pid == -1, "fork failed");
    if (pid == 0) {
        close(tube[WRITE]);

        char buffer;
        ssize_t byte_read;
        ssize_t total = 0;
        while ((byte_read = read(tube[READ], &buffer, 1)) > 0) {
            total++;
            char upper = (buffer >= 'a' && buffer <= 'z') ? (buffer - ('a' - 'A')) : buffer;
            write(STDOUT_FILENO, &upper, 1);
        }
        fprintf(stderr, "[child] TOTAL read %jd bytes\n", total);

        close(tube[READ]);
        return EXIT_SUCCESS;
    } else {
        close(tube[READ]);

        dup2(tube[WRITE], STDOUT_FILENO);
        execlp("ps", NULL);

        close(tube[WRITE]);
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}

/*
    PID TTY          TIME CMD
  25897 PTS/0    00:00:03 ZSH
  ...
  48584 PTS/0    00:00:00 03-UPPER
[child] TOTAL read 251 bytes
*/
