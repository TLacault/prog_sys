#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int ac, char** av) {
    long number = 0;

    ssize_t byte_read = read(STDIN_FILENO, &number, sizeof(long));
    exit_if(byte_read == -1, "read");
    exit_if(byte_read != sizeof(long), "wrong size read");

    printf("%ld\n", number);

    return EXIT_SUCCESS;
}

/*  USAGE

    $ ./04-write_number | ./07-read_number
    5
*/
