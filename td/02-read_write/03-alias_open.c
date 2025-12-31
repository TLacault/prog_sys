#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

#define BUFFER_SIZE 256

int main(int ac, char** av) {
    int fd = open("donnees.txt", O_RDONLY);
    exit_if(fd == -1, "open");

    int fd2 = open("donnees.txt", O_RDONLY);
    exit_if(fd2 == -1, "open");

    ssize_t byte_read;
    char buff[BUFFER_SIZE];

    byte_read = read(fd2, &buff, 4);
    exit_if(byte_read == -1, "could not read");
    exit_if(byte_read != 4, "wrong size read");
    printf("buffer: [%s]\n", buff);

    close(fd);
    close(fd2);

    return EXIT_SUCCESS;
}
