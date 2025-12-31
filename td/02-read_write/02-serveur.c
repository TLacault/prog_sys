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

int main(int ac, char** av) {
    // Cration du FIFO versserveur
    mkfifo("versserveur", 0666);

    // Ouverture du FIFO en écriture
    int fd;
    exit_if((fd = open("versserveur", O_RDONLY)) == -1, "open fifo for reading");

    // Transfert des données de stdin vers le FIFO
    transfer(fd, STDOUT_FILENO);

    return EXIT_SUCCESS;
}

/*
    Creer une FIFO en CLI :
    $ mkfifo versserveur
    ou
    $ mknod versserveur p

    Creer une FIFO en C :
    mkfifo("versserveur", 0666);
*/
