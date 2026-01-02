#include "utils.h"

int main(int ac, char** av) {
    int* data = mmap(NULL, sizeof(int), PROT, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    exit_if(data == MAP_FAILED, "mmap failed");

    fprintf(stderr, "[ptr] %p\n", data);
    fprintf(stderr, "[ptr] %d\n", *data);
    *data = 42;
    fprintf(stderr, "[ptr] %d\n", *data);

    return EXIT_SUCCESS;
}

/*
    $ make map_anonymous CFLAGS+='-DPROT=PROT_NONE'  && mv map_anonymous map_none
    $ ./map_none

    [ptr] 0x7f499d644000
    [1]    355051 segmentation fault (core dumped)  ./map_none

    Ici on a mappé la mémoire avec aucune permission (PROT_NONE).
    Donc on ne peut ni lire ni écrire ni exécuter quoi que ce soit à cette adresse.
*/

/*
    $ make map_anonymous CFLAGS+='-DPROT=PROT_READ'  && mv map_anonymous map_read
    $ ./map_read

    [ptr] 0x7fef3faf0000
    [ptr] 0
    [1]    355169 segmentation fault (core dumped)  ./map_read

    Ici on a mappé la mémoire avec seulement la permission de lire (PROT_READ).
    Donc on peut lire la valeur initiale (0) mais pas écrire (segfault).
*/

/*
    $ make map_anonymous CFLAGS+='-DPROT=PROT_WRITE' && mv map_anonymous map_write
    $ ./map_write

    [ptr] 0x7f564269e000
    [ptr] 0
    [ptr] 42

    Ici on a mappé la mémoire avec seulement la permission d'écrire (PROT_WRITE).
    Donc on peut écrire une valeur (42) mais pas lire avant d'avoir écrit (0).
*/

/*
    $ make map_anonymous CFLAGS+='-DPROT=PROT_EXEC'  && mv map_anonymous map_exec
    $ ./map_exec

    [ptr] 0x7ff5d3a33000
    [1]    355320 segmentation fault (core dumped)  ./map_exec

    Ici on a mappé la mémoire avec seulement la permission d'exécuter (PROT_EXEC).
    Donc on ne peut ni lire ni écrire (segfault).
*/

/*
    $ make map_anonymous CFLAGS+='-DPROT="PROT_EXEC | PROT_WRITE"' && mv map_anonymous map_ew
    $ ./map_ew

    [ptr] 0x7f21d96e9000
    [ptr] 0
    [ptr] 42

    Ici on a mappé la mémoire avec les permissions d'écrire et d'exécuter (PROT_EXEC | PROT_WRITE).
    Donc on peut écrire une valeur (42) mais pas lire avant d'avoir écrit (0).
*/
