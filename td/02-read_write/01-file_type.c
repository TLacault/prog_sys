#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

int main(int ac, char** av) {
    struct stat statbuf;
    fstat(STDIN_FILENO, &statbuf);

    switch (statbuf.st_mode & __S_IFMT) {
        case __S_IFBLK: printf("block device\n"); break;
        case __S_IFCHR: printf("character device\n"); break;
        case __S_IFDIR: printf("directory\n"); break;
        case __S_IFIFO: printf("FIFO/pipe\n"); break;
        case __S_IFLNK: printf("symlink\n"); break;
        case __S_IFREG: printf("regular file\n"); break;
        case __S_IFSOCK: printf("socket\n"); break;

        default: printf("unknown?\n"); break;
    }

    return EXIT_SUCCESS;
}

/*
    ./01-file_type
    character device

    ./01-file_type < /etc
    directory

    ./01-file_type < ./01-file_type
    regular file
*/
