#include "utils.h"

int main(int ac, char** av) {
    // ouvrir le fichier libadd.a en lecture seule
    int fd = open("libadd.a", O_RDONLY);
    exit_if(fd == -1, "open failed");

    // mapper le fichier en mémoire avec les permissions d'exécution
    void* data = mmap(NULL, 2048, PROT_EXEC, MAP_SHARED, fd, 0);
    exit_if(data == MAP_FAILED, "mmap failed");

    // pointeur de fonction sur la zone mémoire mappée - 2048 bytes du fichier libadd.a - aka fonction add
    int (*add_func)(int, int) = data;

    // appeler la fonction add avec des arguments
    int result = add_func(3, 5);
    printf("Result of add(3, 5): %d\n", result);

    // cleanup
    munmap(data, 2048);
    close(fd);

    return EXIT_SUCCESS;
}
