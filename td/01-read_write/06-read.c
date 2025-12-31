#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 512

int main(int ac, char** av) {
    char buff[BUFFER_SIZE];
    ssize_t size_read = BUFFER_SIZE;

    while (size_read > 0) {
        size_read = read(STDIN_FILENO, &buff, BUFFER_SIZE);
        exit_if(size_read == -1, "read");

        if (size_read == 0) break;  // EOF sent (CTRL + D)

        ssize_t byte_written = write(STDOUT_FILENO, buff, size_read);
        exit_if(byte_written == -1, "write");
    }

    return EXIT_SUCCESS;
}
