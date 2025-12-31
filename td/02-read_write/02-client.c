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
    // Verification de la création du FIFO
    struct stat sb;
    exit_if(stat("versserveur", &sb) == -1, "stat fifo");

    // Verification que c'est bien un FIFO
    exit_if((sb.st_mode & S_IFMT) != S_IFIFO, "not a fifo");

    // Ouverture du FIFO en écriture
    int fd;
    exit_if((fd = open("versserveur", O_WRONLY)) == -1, "open fifo for writing");

    // Client prêt à écrire dans le FIFO
    printf("FIFO versserveur opened for writing\n");

    // Transfert des données de stdin vers le FIFO
    transfer(STDIN_FILENO, fd);

    // Fermeture du FIFO
    close(fd);

    return EXIT_SUCCESS;
}

/*
    Blocage:
    L'appel open sur une FIFO est bloquant.
    Le client restera bloqué sur open tant qu'un serveur n'aura pas ouvert la FIFO en lecture (et inversement).
*/

/*
    Localisation:
    Une FIFO ne fonctionne que sur une même machine (ou un système de fichiers partagé comme NFS).
    C'est pourquoi le chat ne marche pas entre votre PC et le serveur SSH de l'école
    sans passer par une connexion SSH sur la même machine.
*/
