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
        int count = (ac == 2) ? atoi(av[1]) : 10;
        for (int i = 0; i < count; i++) {
            ssize_t byte_write = write(tube[WRITE], CHAINE, (ssize_t)SIZE);
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

/*
    Ici l'enfant ne peut lire que 500 bytes à la fois
    donc si le parent écrire plus de 500 bytes l'enfant devra les lire en plusieurs fois.

    Par exemple si le parent écrit 55 * strlen("Hello World!") = 660 bytes
    l'enfant devra faire 2 lectures :
        - une première de 500 bytes
        - une seconde de 160 bytes
*/
