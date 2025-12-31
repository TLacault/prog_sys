#include "utils.h"

enum {
    READ,
    WRITE
};

#define BUFFER_SIZE 500

#define CHAINE "Hello World!"
#define SIZE (strlen(CHAINE))

int main(int ac, char** av) {
    int tube[2];
    pipe(tube);

    pid_t pid = fork();
    exit_if(pid == -1, "fork failed");
    if (pid == 0) {
        close(tube[WRITE]);

        char buffer[BUFFER_SIZE];
        ssize_t byte_read;
        ssize_t total = 0;
        while ((byte_read = read(tube[READ], &buffer, BUFFER_SIZE)) > 0) {
            total += byte_read;
            write(STDOUT_FILENO, &buffer, byte_read);
            fprintf(stderr, "[child] read %jd bytes\n", byte_read);
        }
        fprintf(stderr, "[child] TOTAL read %jd bytes\n", total);

        close(tube[READ]);
        return EXIT_SUCCESS;
    } else {
        close(tube[READ]);

        ssize_t total = 0;
        for (int i = 0; i < 10; i++) {
            ssize_t byte_write = write(tube[WRITE], CHAINE, (ssize_t)SIZE);
            // sleep(1);
            exit_if(byte_write == -1, "can't write inside pipe");
            exit_if(byte_write != (ssize_t)SIZE, "wrong size write");
            total += byte_write;
        }
        fprintf(stderr, "[parent] wrote %jd bytes\n", total);

        close(tube[WRITE]);
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
