#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int ac, char** av) {
    struct nopad n = { 'A', 42, 'B' };

    ssize_t byte_write = write(STDOUT_FILENO, &n, sizeof(struct nopad));
    exit_if(byte_write == -1, "write");
    exit_if(byte_write != sizeof(struct nopad), "wrong size write");

    printf("\nWrote %zd bytes\n", byte_write);
    return EXIT_SUCCESS;
}
