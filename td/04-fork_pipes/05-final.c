#include "utils.h"

enum {
    READ,
    WRITE
};

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s cmd1 [args...] -- cmd2 [args...] -- ... -- cmdN [args...]\n", prog);
}

static int run_pipeline(char ***commandes, int nb_commandes) {
    if (nb_commandes <= 0) {
        fprintf(stderr, "No command to run\n");
        return EXIT_FAILURE;
    }

    if (nb_commandes == 1) {
        execvp(commandes[0][0], commandes[0]);
        perror(commandes[0][0]);
        return 127;
    }

    int tubes[nb_commandes - 1][2];
    pid_t pids[nb_commandes];

    for (int i = 0; i < nb_commandes - 1; i++) {
        exit_if(pipe(tubes[i]) == -1, "pipe");
    }

    for (int i = 0; i < nb_commandes; i++) {
        pid_t pid = fork();
        exit_if(pid == -1, "fork");

        if (pid == 0) {
            if (i > 0) {
                exit_if(dup2(tubes[i - 1][READ], STDIN_FILENO) == -1, "dup2(stdin)");
            }
            if (i < nb_commandes - 1) {
                exit_if(dup2(tubes[i][WRITE], STDOUT_FILENO) == -1, "dup2(stdout)");
            }

            // Important: fermer tous les FDs de pipes (sinon EOF jamais vu / blocages).
            for (int j = 0; j < nb_commandes - 1; j++) {
                close(tubes[j][READ]);
                close(tubes[j][WRITE]);
            }

            fprintf(stderr, "[child %d] executing command [%s]\n", getpid(), commandes[i][0]);
            execvp(commandes[i][0], commandes[i]);
            perror(commandes[i][0]);
            _exit(127);
        }

        pids[i] = pid;
    }

    // Parent: fermer aussi tous les pipes.
    for (int i = 0; i < nb_commandes - 1; i++) {
        close(tubes[i][READ]);
        close(tubes[i][WRITE]);
    }

    // Attendre tous les enfants, et propager le status du dernier.
    int last_status = 0;
    exit_if(waitpid(pids[nb_commandes - 1], &last_status, 0) == -1, "waitpid(last)");

    for (int i = 0; i < nb_commandes - 1; i++) {
        int status = 0;
        exit_if(waitpid(pids[i], &status, 0) == -1, "waitpid");
    }

    if (WIFEXITED(last_status)) {
        return WEXITSTATUS(last_status);
    }
    if (WIFSIGNALED(last_status)) {
        int sig = WTERMSIG(last_status);
        fprintf(stderr, "Last command terminated by signal %s (%d)\n", signame(sig), sig);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "Last command terminated abnormally\n");
    return EXIT_FAILURE;
}

static void free_commands(char ***commandes, int nb_commandes) {
    if (commandes == NULL) return;
    for (int i = 0; i < nb_commandes; i++) {
        free(commandes[i]);
    }
    free(commandes);
}

static int parse_commands(char *argv[], char ****out_commandes, int *out_nb_commandes) {
    // argv: liste d'arguments NULL-terminée (sans le nom du programme)
    if (argv == NULL || argv[0] == NULL) return -1;

    int max_tokens = 0;
    for (char **p = argv; *p != NULL; p++) max_tokens++;

    // Pire cas: "a -- b -- c" => ~ (max_tokens+1)/2 commandes
    char ***commandes = malloc((size_t)(max_tokens + 1) * sizeof(*commandes));
    if (commandes == NULL) return -1;

    int nb = 0;
    char *remaining[max_tokens + 1];
    for (int i = 0; i < max_tokens; i++) remaining[i] = argv[i];
    remaining[max_tokens] = NULL;

    while (1) {
        char *args1[max_tokens + 1];
        char *args2[max_tokens + 1];

        int rc = split_args(remaining, args1, args2);
        if (rc == 0) {
            // On a trouvé un "--" et donc une commande complète dans args1
            int n = 0;
            while (args1[n] != NULL) n++;
            if (n == 0) {
                free_commands(commandes, nb);
                return -1;
            }
            char **cmd = malloc((size_t)(n + 1) * sizeof(*cmd));
            if (cmd == NULL) {
                free_commands(commandes, nb);
                return -1;
            }
            for (int i = 0; i <= n; i++) cmd[i] = args1[i];
            commandes[nb++] = cmd;

            // Continuer avec le reste
            if (args2[0] == NULL) {
                free_commands(commandes, nb);
                return -1;
            }
            int k = 0;
            while (args2[k] != NULL) {
                remaining[k] = args2[k];
                k++;
            }
            remaining[k] = NULL;
            continue;
        }

        // Pas de séparateur: remaining est la dernière commande
        int n = 0;
        while (remaining[n] != NULL) n++;
        if (n == 0) {
            free_commands(commandes, nb);
            return -1;
        }
        char **cmd = malloc((size_t)(n + 1) * sizeof(*cmd));
        if (cmd == NULL) {
            free_commands(commandes, nb);
            return -1;
        }
        for (int i = 0; i <= n; i++) cmd[i] = remaining[i];
        commandes[nb++] = cmd;
        break;
    }

    *out_commandes = commandes;
    *out_nb_commandes = nb;
    return 0;
}

int main(int ac, char **av) {
    if (ac < 2) {
        usage(av[0]);
        return EXIT_FAILURE;
    }

    char ***commandes = NULL;
    int nb_commandes = 0;
    if (parse_commands(&av[1], &commandes, &nb_commandes) == -1) {
        usage(av[0]);
        return EXIT_FAILURE;
    }

    int rc = run_pipeline(commandes, nb_commandes);
    free_commands(commandes, nb_commandes);
    return rc;
}
