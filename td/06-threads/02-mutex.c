#include "utils.h"

#include <sched.h>

#define NB_THREADS 8

static int ticket_to_sell = 100000;

// Mutex to protect ticket_to_sell
static pthread_mutex_t ticket_mutex = PTHREAD_MUTEX_INITIALIZER;

static void *start(void *arg) {
    int ticket_sold = 0;
    int thread_id = (int)(intptr_t)arg;

    for (;;) {
        sched_yield();  // let other threads run

        // Lock the mutex before accessing ticket_to_sell
        int err = pthread_mutex_lock(&ticket_mutex);
        exit_if(err != 0, "pthread_mutex_lock");

        if (ticket_to_sell > 0) {
            ticket_to_sell--;
            ticket_sold++;
        } else {
            // Unlock the mutex before breaking
            err = pthread_mutex_unlock(&ticket_mutex);
            exit_if(err != 0, "pthread_mutex_unlock");
            break;
        }

        // Unlock the mutex after accessing ticket_to_sell
        err = pthread_mutex_unlock(&ticket_mutex);
        exit_if(err != 0, "pthread_mutex_unlock");
    }

    printf("[%d] sold %d tickets\n", thread_id, ticket_sold);
    return (void *)(intptr_t)ticket_sold;
}

int main(int ac, char **av) {
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
    return EXIT_SUCCESS;
}

/*
    MUTEXES:
    Les mutexes (mutual exclusion) sont des mécanismes de synchronisation utilisés pour protéger l'accès
    à des ressources partagées dans un environnement multi-thread. Ils permettent de s'assurer qu'
    une seule thread à la fois peut accéder à une section critique du code, évitant ainsi les conditions de course.

    UTILISATION:
    Dans ce programme, un mutex (`ticket_mutex`) est utilisé pour protéger l'accès à la variable globale
    `ticket_to_sell`. Avant qu'un thread n'accède ou ne modifie cette variable, il verrouille le mutex avec `pthread_mutex_lock`.
    Cela garantit qu'aucun autre thread ne peut accéder à `ticket_to_sell` tant que le mutex est verrouillé.
    Après avoir terminé l'accès à la variable, le thread déverrouille le mutex avec `pthread_mutex_unlock`,
    permettant ainsi à d'autres threads d'accéder à la variable. Cela garantit que les modifications apportées à
    `ticket_to_sell` sont effectuées de manière atomique, évitant les incohérences et les erreurs.
*/
