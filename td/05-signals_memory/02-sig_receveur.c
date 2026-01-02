#include "utils.h"

void handler(int sig) {
    printf(">> received signal [%s]\n", strsignal(sig));
}

int main(int ac, char **av) {
    exit_if(ac != 2, "[usage] ./02-sig_expediteur <file>");

    pid_t pid = getpid();
    printf("[receiver pid] %d\n", pid);

    // ------------------------------------------------------------------------- FILE HANDLING
    int fd = open(av[1], O_CREAT | O_RDWR);
    exit_if(fd == -1, "file does not exist");

    size_t size = lseek(fd, 0, SEEK_END);  // get file size
    lseek(fd, 0, SEEK_SET);                // reset file offset - set cursor to beginning of file

    void *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    exit_if(data == MAP_FAILED, "mmap failed");

    pid_t *t = data;  // write pid at the beginning of the file
    *t = pid;         // write receiver pid into mapped memory

    // ------------------------------------------------------------------------- SIGNAL HANDLING
    struct sigaction sa;
    sigaction(SIGUSR1, NULL, &sa);
    struct sigaction my_sa = sa;
    my_sa.sa_handler = handler;
    sigaction(SIGUSR1, &my_sa, NULL);
    sigset_t set;
    sigemptyset(&set);

    // while (true) {
    printf("\nwaiting to receive SIGUSR1\n");
    sigsuspend(&set);
    printf("DATA: %s\n", (char *)t);
    // }

    // ------------------------------------------------------------------------- CLEANUP

    munmap(data, size);
    close(fd);

    return EXIT_SUCCESS;
}

/*
    L'interet de l'exo est un peu bizarre.
    Quand on recoit le signal on vien bien écrire le PID dans le fichier mmapé.
    Mais le fichier est corrompu (car on ecrit des données binaires dans un fichier texte)
    Avec la commande     $ hexdump -C data.txt    on peut voir le contenu hexa du fichier.

    Prend ton PID (par exemple 336081) et converti le en hexadécimal : 0x000520b1
    Ensuite regarde le hexdump du fichier data.txt
    Tu devrais voir au début du fichier les 4 octets suivants : b1 20 05 00
    C'est bien le PID écrit en binaire dans le fichier mais en format little-endian (ordre des octets inversé)
*/
