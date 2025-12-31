#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int ac, char** av) {
    struct nopad n;

    ssize_t byte_read = read(STDIN_FILENO, &n, sizeof(struct nopad));
    exit_if(byte_read == -1, "read");
    exit_if(byte_read != sizeof(struct nopad), "wrong size read");

    printf("%c\n", n.c1);
    printf("%ld\n", n.l);
    printf("%c\n", n.c2);

    return EXIT_SUCCESS;
}

/*  USAGE

    $ ./08-write_struct | ./08-read_struct
    A
    42
    B
*/

/*
    Sans CFLAGS -O3 :
    Wrote 24 bytes

    Avec CFLAGS -O3 :
    make CFLAGS+=-O3 ./08-write_struct
    make CFLAGS+=-O3 ./08-read_struct
    Wrote 24 bytes


    La Taille ne change pas a moins de réorganiser les champs dans la struct
    On place le Long avant les 2 char pour aligner les bytes
    Le compilateur ajoute du padding pour aligner les champs en mémoire
    Le type long (8 bytes) doit être aligné sur une adresse multiple de 8
    Le type char (1 byte) peut être à n'importe quelle adresse
    Après réorganisation, la taille de la struct devient 16 bytes

    Avant
    | Char                      | Long                      | Char  |

    Après
    | Long                      | Char  | Char  |

    struct nopad {
        long l;
        char c1;
        char c2;
    };
*/
