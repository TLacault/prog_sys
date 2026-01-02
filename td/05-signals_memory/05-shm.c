#include "utils.h"

int main(int ac, char** av) {
    // Crée une clé unique pour la mémoire partagée
    key_t key = ftok("shared.txt", 'A');
    exit_if(key == -1, "ftok failed");

    // utiliser une taille de 1024 octets, pour le flag IPC_CREAT | 0644
    int shmid = shmget(key, 1024, IPC_CREAT | 0644);
    exit_if(shmid == -1, "shmget failed");

    // Attache le segment à l’espace virtuel du processus comme un tableau d’entiers
    int* data = shmat(shmid, NULL, 0);
    exit_if(data == (void*)-1, "shmat failed");

    // Incrémente le premier entier et l’affiche
    data[0] += 1;
    printf("First integer: %d\n", data[0]);

    // Détache le segment de la mémoire partagée
    int res = shmdt(data);
    exit_if(res == -1, "shmdt failed");

    return EXIT_SUCCESS;
}

/*
    A chaque exécution du programme, le premier entier dans la mémoire partagée est
    incrémenté de 1 et affiché.
*/

// J'avoue à partir de la question 3 j'ai eu la big flemme de ssh sur l'enseirb
