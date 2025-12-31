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
    // SERVER SIDE
    mkfifo("versserveur", 0666);
    int fd_in;
    exit_if((fd_in = open("versserveur", O_RDONLY)) == -1, "open fifo for reading");

    // CLIENT SIDE
    mkfifo("versclient", 0666);
    int fd_out;
    exit_if((fd_out = open("versclient", O_WRONLY)) == -1, "open fifo for writing");

    // CHAT
    pid_t pid = fork();
    exit_if(pid == -1, "fork");

    if (pid == 0) {
        // Processus ENFANT : Il s'occupe de la RÉCEPTION
        // Il lit la FIFO et écrit sur l'écran
        transfer(fd_in, STDOUT_FILENO);
        // Si transfer revient, c'est que la FIFO est fermée
        exit(0);
    } else {
        // Processus PARENT : Il s'occupe de l'ENVOI
        // Il lit le clavier et écrit dans la FIFO
        transfer(STDIN_FILENO, fd_out);
        // Si on finit de taper (Ctrl+D), on tue l'enfant et on quitte
        kill(pid, SIGTERM);
    }

    close(fd_in);
    close(fd_out);
    return EXIT_SUCCESS;
}

/*
Chat à N clients : Le problème de la lecture
Si tu as 3 clients (A, B, C) et une seule FIFO versserveur :

Si A écrit "Salut", le serveur le reçoit.

Mais si le serveur écrit "Salut" dans une seule FIFO versclients lue par tout le monde, un seul client (le plus rapide sur le CPU) recevra le message. Le message est "consommé" par le premier read(). Les autres ne verront rien.

La solution "Étoile" (Star Topology) : Pour faire un chat à N, il faut un serveur central qui joue le rôle de relais (hub) :

1 FIFO d'entrée unique : Tous les clients écrivent dans versserveur.

N FIFOs de sortie : Chaque client possède sa propre FIFO (ex: client1_fifo, client2_fifo).

Le Serveur : Il lit versserveur en boucle. Dès qu'il reçoit un message, il fait une boucle pour ré-écrire ce message dans toutes les FIFOs des clients connectés.
*/
