#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int ac, char** av) {
    long number = 5;
    ssize_t byte_written = write(STDOUT_FILENO, &number, sizeof(long));
    exit_if(byte_written == -1, "write");
    // printf("\nWrote %zd bytes\n", byte_written);
    return EXIT_SUCCESS;
}

/*
    OUTPUT

    Wrote 8 bytes
*/

/*
    OUTPUT: od -x
    // on voit que le nombre 5 est bien écrit
    // en hexadécimal : 0005
    // suivi de 6 octets à 0 pour compléter le long (8 octets)
    // puis le texte "Wrote 8 bytes\n"
    0000000 0005 0000 0000 0000 570a 6f72 6574 3820
    0000020 6220 7479 7365 000a
    0000027
*/

// perror
// -> pas un syscall mais une fonction de la libc
// -> elle utilise le syscall write pour afficher le message d'erreur sur stderr
