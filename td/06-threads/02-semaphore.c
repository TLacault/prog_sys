#include "utils.h"

#include <sched.h>
#include <semaphore.h>

#define NB_THREADS 8

static int ticket_to_sell = 100000;

// Semaphore to protect ticket_to_sell
static sem_t ticket_sem;

static void *start(void *arg) {
    int ticket_sold = 0;
    int thread_id = (int)(intptr_t)arg;

    for (;;) {
        sched_yield();  // let other threads run

        // Semaphore : wait (decrement)
        exit_if(sem_wait(&ticket_sem) == -1, "sem_wait");

        if (ticket_to_sell > 0) {
            ticket_to_sell--;
            ticket_sold++;
            // Semaphore : signal (increment)
            exit_if(sem_post(&ticket_sem) == -1, "sem_post");
        } else {
            // Semaphore : signal (increment)
            exit_if(sem_post(&ticket_sem) == -1, "sem_post");
            break;
        }
    }

    printf("[%d] sold %d tickets\n", thread_id, ticket_sold);
    return (void *)(intptr_t)ticket_sold;
}

int main(int ac, char **av) {
    // Initialize semaphore
    exit_if(sem_init(&ticket_sem, 0, 1) == -1, "sem_init");

    pthread_t threads[NB_THREADS];

    for (int i = 0; i < NB_THREADS; i++) {
        int err = pthread_create(&threads[i], NULL, start, (void *)(intptr_t)i);
        exit_if(err != 0, "pthread_create");
    }

    int total_sold = 0;

    for (int i = 0; i < NB_THREADS; i++) {
        void *ret = NULL;
        int err = pthread_join(threads[i], &ret);
        exit_if(err != 0, "pthread_join");
        total_sold += (int)(intptr_t)ret;
    }

    printf("\n>> Total tickets sold: %d\n", total_sold);

    // Destroy semaphore
    exit_if(sem_destroy(&ticket_sem) == -1, "sem_destroy");
    return EXIT_SUCCESS;
}

/*
    SEMAPHORE:
    Un Semaphore est une primitive de synchronisation qui permet de contrôler l'accès à une ressource partagée
    par plusieurs threads. Il maintient un compteur qui indique le nombre de ressources disponibles.
    Lorsqu'un thread veut accéder à la ressource, il effectue une opération "wait" (ou "P") qui décrémente le compteur.
    Si le compteur est supérieur à zéro, le thread peut continuer. Sinon, il est bloqué jusqu'à ce qu'une ressource soit libérée.
    Lorsqu'un thread a fini d'utiliser la ressource, il effectue une opération "signal" (ou "V") qui incrémente le compteur,
    libérant ainsi une ressource pour d'autres threads.

    UTILISATION:
    Dans ce programme, un sémaphore est utilisé pour protéger l'accès à la variable globale `ticket_to_sell`.
    Avant de décrémenter `ticket_to_sell`, un thread effectue une opération `sem_wait` pour s'assurer qu'il a l'accès exclusif à la variable.
    Après avoir modifié `ticket_to_sell`, le thread effectue une opération `sem_post` pour libérer l'accès à la variable.
    Cela garantit qu'un seul thread à la fois peut modifier `ticket_to_sell`, évitant ainsi les conditions de course.
*/
