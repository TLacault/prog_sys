#include "utils.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

/*
    struct dirent {
            ino_t d_ino;             // Inode number
            off_t d_off;             // Not an offset; see below
            unsigned short d_reclen; // Length of this record
            unsigned char d_type;    // Type of file; not supported by all filesystem types
            char d_name[256];        // Null-terminated filename
    };
*/

char type_to_char(uint8_t type) {
    switch (type) {
        case DT_BLK: return 'b';
        case DT_CHR: return 'c';
        case DT_DIR: return 'd';
        case DT_FIFO: return 'p';
        case DT_LNK: return 'l';
        case DT_REG: return '-';
        case DT_SOCK: return 's';
        default: return '?';
    }
}

int main(int ac, char** av) {
    DIR* dir = opendir((ac == 2) ? av[1] : ".");
    exit_if(!dir, "opendir");

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
        printf("%c\t%s\n", type_to_char(entry->d_type), entry->d_name);

    exit_if(errno != 0, "readdir");

    closedir(dir);
    return EXIT_SUCCESS;
}
