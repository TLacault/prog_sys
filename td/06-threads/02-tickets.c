#include "utils.h"

#define NB_THREADS 8

int ticket_to_sell = 100000;

void *start(void *arg) {
    int ticket_sold = 0;
    int thread_id = (int)(intptr_t)arg;

    while (ticket_to_sell > 0) {
        sched_yield();  // let other threads run
        ticket_to_sell--;
        ticket_sold++;
    }

    printf("[%d] sold %d tickets\n", thread_id, ticket_sold);
    pthread_exit((void *)(intptr_t)ticket_sold);

    return NULL;
}

int main(int ac, char **av) {
    pthread_t threads[NB_THREADS];

    // Create threads
    for (int i = 0; i < NB_THREADS; i++) {
        int err = pthread_create(&threads[i], NULL, start, (void *)(intptr_t)i);
        exit_if(err != 0, "pthread_create");
    }

    int total_sold = 0;

    // Wait for threads to finish
    for (int i = 0; i < NB_THREADS; i++) {
        void *ret;
        int err = pthread_join(threads[i], &ret);
        exit_if(err != 0, "pthread_join");
        total_sold += (int)(intptr_t)ret;
    }

    printf("\n>> Total tickets sold: %d\n", total_sold);

    return EXIT_SUCCESS;
}

/*
    Dans ce programme, plusieurs threads vendent des billets à partir d'un compteur global `ticket_to_sell`.
    Chaque thread décrémente ce compteur lorsqu'il vend un billet.

    Problème :
    Comme plusieurs threads accèdent et modifient la variable globale `ticket_to_sell` simultanément sans synchronisation,
    cela peut entraîner des conditions de course. Par exemple, deux threads peuvent lire la même valeur de `ticket_to_sell`
    avant que l'un d'eux ne la décrémente, ce qui conduit à vendre plus de billets que disponible.

    Conséquence :
    Le total des billets vendus peut dépasser le nombre initial de billets disponibles (100000 dans ce cas),
    ce qui est incorrect.

    Solution possible :
    Utiliser des mécanismes de synchronisation comme des mutex pour protéger l'accès à la variable `ticket_to_sell`
    afin qu'un seul thread puisse la modifier à la fois.
*/
