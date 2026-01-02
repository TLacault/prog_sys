#include "utils.h"

int add(int a, int b) {
    return a + b;
}

int main(int ac, char** av) {
    // création (ou ouverture) du fichier libadd.a
    int fd = open("libadd.a", O_CREAT | O_RDWR, 0666);
    exit_if(fd == -1, "open failed");

    // étendre le fichier à 2048 bytes
    int res = ftruncate(fd, 2048);
    exit_if(res == -1, "ftruncate failed");

    // AUTRE METHODE POSSIBLE
    // char data[2048];
    // write(fd, data, 2048);
    // lseek(fd, 0, SEEK_SET);

    // mapper le fichier en mémoire avec les permissions d'écriture et d'exécution
    void* data = mmap(NULL, 2048, PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0);
    exit_if(data == MAP_FAILED, "mmap failed");

    // copier le code de la fonction add dans la mémoire mappée
    memcpy(data, (void*)add, 2048);

    // cleanup
    munmap(data, 2048);
    close(fd);

    return EXIT_SUCCESS;
}
