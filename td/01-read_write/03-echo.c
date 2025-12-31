#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int ac, char** av) {
    // Affiche chaque argument sur une nouvelle lignex
    for (int i = 0; i < ac; i++) {
        ssize_t byte_written = write(STDOUT_FILENO, av[i], strlen(av[i]));
        exit_if(byte_written == -1, "write");
        write(STDOUT_FILENO, "\n", 1);
    }
    return EXIT_SUCCESS;
}
