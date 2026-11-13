#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CYCLES 1000000

typedef struct {
    int id;
    int philosophes;
    pthread_mutex_t *baguette;
} philo_arg_t;

void mange(void) {}

void *philosophe(void *arg) {
    philo_arg_t *data = arg;
    const int id = data->id;
    const int philosophes = data->philosophes;
    pthread_mutex_t *baguette = data->baguette;

    const int left = id;
    const int right = (left + 1) % philosophes;

    for (int i = 0; i < CYCLES; i++) {
        // philosophe pense
        if (left < right) {
            pthread_mutex_lock(&baguette[left]);
            pthread_mutex_lock(&baguette[right]);
        } else {
            pthread_mutex_lock(&baguette[right]);
            pthread_mutex_lock(&baguette[left]);
        }

        mange();

        pthread_mutex_unlock(&baguette[left]);
        pthread_mutex_unlock(&baguette[right]);
    }

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <nb_philosophes>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *endptr = NULL;
    errno = 0;
    long val = strtol(argv[1], &endptr, 10);
    if (errno != 0 || *endptr != '\0' || endptr == argv[1] || val < 2) {
        fprintf(stderr, "Erreur: le nombre de philosophes doit être un entier supérieur ou égal à 2.\n");
        return EXIT_FAILURE;
    }

    const int philosophes = (int)val;

    pthread_t phil[philosophes];
    pthread_mutex_t baguette[philosophes];
    philo_arg_t args[philosophes];

    int err;
    for (int i = 0; i < philosophes; i++) {
        err = pthread_mutex_init(&baguette[i], NULL);
        if (err != 0) {
            fprintf(stderr, "Erreur lors de l'initialisation des mutex.\n");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < philosophes; i++) {
        args[i].id = i;
        args[i].philosophes = philosophes;
        args[i].baguette = baguette;

        err = pthread_create(&phil[i], NULL, philosophe, &args[i]);
        if (err != 0) {
            fprintf(stderr, "Erreur lors de la création des threads.\n");
            return EXIT_FAILURE;
        }
    }

    for (int i = philosophes - 1; i >= 0; i--) {
        err = pthread_join(phil[i], NULL);
        if (err != 0) {
            fprintf(stderr, "Erreur lors de la récupération des valeurs de retour.\n");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < philosophes; i++) {
        err = pthread_mutex_destroy(&baguette[i]);
        if (err != 0) {
            fprintf(stderr, "Erreur lors de la destruction des mutex.\n");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
