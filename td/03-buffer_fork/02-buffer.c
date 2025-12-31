#include "utils.h"

#define CHAINE1 "Hello\n"
#define CHAINE2 " World!"

int main(int ac, char** av) {
    FILE* sortie1;
#ifdef UTILISER_SORTIE_ERREUR1
    sortie1 = stderr;
#else
    sortie1 = stdout;
#endif

#ifdef UTILISER_FPRINTF1
    fprintf(sortie1, CHAINE1);
#else
    write(fileno(sortie1), CHAINE1, 6);
#endif

    sleep(1);

    FILE* sortie2;
#ifdef UTILISER_SORTIE_ERREUR2
    sortie2 = stderr;
#else
    sortie2 = stdout;
#endif

#ifdef UTILISER_FPRINTF2
    fprintf(sortie2, CHAINE2);
#else
    write(fileno(sortie2), CHAINE2, 7);
#endif

#ifdef UTILISER_SORTIE_ERREUR2
    _exit(EXIT_SUCCESS);
#else
    exit(EXIT_SUCCESS);
#endif

    return EXIT_SUCCESS;
}

// fflush(stdout) permet de forcer l'écriture du buffer de stdout sur le terminal.
// Par défaut, stdout est en mode "line buffered" quand il est relié à un terminal,
// et en mode "fully buffered" quand il est relié à un fichier ou un pipe.
// Dans le premier cas, le buffer est écrit à chaque fois qu'un '\n' est rencontré,
// ou quand le buffer est plein.
// Dans le second cas, le buffer est écrit seulement quand il est plein.
// On peut forcer l'écriture du buffer en appelant fflush(stdout).;

/*
    $ rm -f 02-buffer && make 02-buffer CFLAGS+="-DUTILISER_FPRINTF1"
    World!Hello%

    $ rm -f 02-buffer && make 02-buffer CFLAGS+="-DUTILISER_FPRINTF2 -DUTILISER_FPRINTF1"
    Hello World!%

    $ rm -f 02-buffer && make 02-buffer CFLAGS+="-DUTILISER_FPRINTF1 -DUTILISER_FPRINTF2 -DUTILISER_SORTIE_ERREUR2"
    World!%
*/

// Après rajout \n dans CHAINE1

/*
    $ rm -f 02-buffer && make 02-buffer CFLAGS+="-DUTILISER_FPRINTF1"
    Hello
     World!%

    $ rm -f 02-buffer && make 02-buffer CFLAGS+="-DUTILISER_FPRINTF2 -DUTILISER_FPRINTF1"
    Hello
     World!%

    $ rm -f 02-buffer && make 02-buffer CFLAGS+="-DUTILISER_FPRINTF1 -DUTILISER_FPRINTF2 -DUTILISER_SORTIE_ERREUR2"
    Hello
     World!%
*/

// Dans les 3 cas la CHAINE1 est affichée avant CHAINE2 car le \n force l'écriture du buffer de stdout.
