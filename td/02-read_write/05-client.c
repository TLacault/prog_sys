#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

#define BUFFER_SIZE 256

void transfer(int fd_in, int fd_out) {
    char buffer[BUFFER_SIZE];
    ssize_t byte_read, byte_write, total_write;

    while (1) {
        byte_read = read(fd_in, buffer, BUFFER_SIZE);
        exit_if(byte_read == -1, "read");

        if (byte_read == 0) return;  // EOF reached

        /*
        * Si jamais write n'écrit pas assez, on doit boucler pour
        * tout écrire avant de repasser à read
        */
        total_write = 0;
        do {
            byte_write = write(fd_out, buffer + total_write, byte_read - total_write);
            exit_if(byte_write == -1, "write");
            total_write += byte_write;
        } while (total_write < byte_read);
    }
}

// -----------------------------------------------------------------------------

int main(int ac, char** av) {
    struct stat sb;

    // SEND TO SERVER
    exit_if(stat("versserveur", &sb) == -1, "stat fifo");
    exit_if((sb.st_mode & S_IFMT) != S_IFIFO, "not a fifo");

    int fd_out;
    exit_if((fd_out = open("versserveur", O_WRONLY)) == -1, "open fifo for writing");

    // RECEIVE FROM SERVER
    exit_if(stat("versclient", &sb) == -1, "stat fifo");
    exit_if((sb.st_mode & S_IFMT) != S_IFIFO, "not a fifo");

    int fd_in;
    exit_if((fd_in = open("versclient", O_RDONLY)) == -1, "open fifo for reading");

    // CHAT
    // printf("[client is connected]\n");
    transfer(STDIN_FILENO, fd_out);
    transfer(fd_in, STDOUT_FILENO);

    close(fd_in);
    close(fd_out);
    return EXIT_SUCCESS;
}
