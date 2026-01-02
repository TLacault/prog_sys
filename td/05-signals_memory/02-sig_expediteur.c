#include "utils.h"

int main(int ac, char **av) {
    exit_if(ac != 2, "[usage] ./02-sig_expediteur <file>");

    // ------------------------------------------------------------------------- FILE HANDLING
    int fd = open(av[1], O_CREAT | O_RDWR, 0666);
    exit_if(fd == -1, "file does not exist");

    size_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    void *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    exit_if(data == MAP_FAILED, "mmap failed");

    pid_t receiver_pid = *((pid_t *)data);  // read receiver pid from mapped memory
    read(STDIN_FILENO, data, size);         // read data from standard input into mapped memory

    // ------------------------------------------------------------------------- SIGNAL HANDLING
    kill(receiver_pid, SIGUSR1);

    // ------------------------------------------------------------------------- CLEANUP
    munmap(data, size);
    close(fd);

    return EXIT_SUCCESS;
}
