#include "utils.h"

#define NB_THREADS 8

int global_counter = 0;

void *start(void *arg) {
    int local_counter = 0;
    int thread_id = (int)(intptr_t)arg;
    printf("\n[thread %d] pid %d\n", thread_id, getpid());
    local_counter++;
    global_counter++;
    printf("(local_counter)  %d @ %p\n", local_counter, &local_counter);
    printf("(global_counter) %d @ %p\n", global_counter, &global_counter);
    return NULL;
}

int main(int ac, char **av) {
    pthread_t threads[NB_THREADS];

    // Create threads
    for (int i = 0; i < NB_THREADS; i++) {
        int err = pthread_create(&threads[i], NULL, start, (void *)(intptr_t)i);
        exit_if(err != 0, "pthread_create");
    }

    // Wait for threads to finish
    for (int i = 0; i < NB_THREADS; i++) {
        int err = pthread_join(threads[i], NULL);
        exit_if(err != 0, "pthread_join");
    }

    printf("\n>>>> All threads completed successfully.\n");

    return EXIT_SUCCESS;
}

/*
    La variable locale `local_counter` est stockée dans la pile de chaque thread.
    Chaque thread a sa propre pile, donc chaque thread a sa propre copie de `local_counter`.
    Ainsi, les modifications apportées à `local_counter` par un thread n'affectent pas les autres threads.
    >> Sont adresse est différente pour chaque thread car elles sont allouées dans des piles différentes.

    La variable globale `global_counter` est stockée dans la section de données du programme.
    Tous les threads partagent cette section de données, donc ils accèdent tous à la même copie de `global_counter`.
    Ainsi, les modifications apportées à `global_counter` par un thread affectent tous les autres threads.
    >> Son adresse est la même pour tous les threads car ils partagent la même section de données.
*/
