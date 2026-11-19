#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>

#define TAILLE 8
#define TOTAL 131072
#define TRAITEMENT 10000
#define MAX_THREADS 1024

static int buffer[TAILLE];
static int index_ecriture = 0;
static int index_lecture = 0;
static pthread_mutex_t mutex;
static sem_t vide;
static sem_t plein;

typedef struct {
    int id;
    int nb;
} prodcons_arg_t;

// Simule un traitement qui utilise le CPU (boucle imposée par l'énoncé).
static void traitement(void) {
    for (int i = 0; i < TRAITEMENT; i++);
}

// Thread producteur : produit data->nb éléments, chacun valant son identifiant data->id.
// Il attend une case libre (sémaphore "vide"), prend le mutex pour écrire dans le buffer
// circulaire, puis signale qu'une case est pleine (sémaphore "plein").
// Le sémaphore est attendu AVANT de prendre le mutex : sinon un thread pourrait attendre
// en tenant le verrou et bloquer tous les autres. Le traitement se fait hors section critique.
void *producteur(void *arg) {
    const prodcons_arg_t *data = arg;

    for (int i = 0; i < data->nb; i++) {
        traitement();

        sem_wait(&vide);
        pthread_mutex_lock(&mutex);
        buffer[index_ecriture] = data->id;
        index_ecriture = (index_ecriture + 1) % TAILLE;
        pthread_mutex_unlock(&mutex);
        sem_post(&plein);
    }

    return NULL;
}

// Thread consommateur : consomme data->nb éléments.
// Même principe que le producteur, à l'envers : il attend une case pleine (sémaphore "plein"),
// lit dans le buffer sous mutex, puis signale qu'une case est libre (sémaphore "vide").
// La valeur lue n'est pas utilisée ensuite. Le traitement se fait hors section critique.
void *consommateur(void *arg) {
    const prodcons_arg_t *data = arg;

    for (int i = 0; i < data->nb; i++) {
        sem_wait(&plein);
        pthread_mutex_lock(&mutex);
        const int valeur = buffer[index_lecture];
        index_lecture = (index_lecture + 1) % TAILLE;
        pthread_mutex_unlock(&mutex);
        sem_post(&vide);

        (void)valeur;
        traitement();
    }

    return NULL;
}

// Convertit un argument de la ligne de commande en entier entre 1 et MAX_THREADS.
// Renvoie 0 si l'argument est valide, -1 sinon.
static int lire_entier(const char *texte, int *resultat) {
    char *endptr = NULL;
    errno = 0;
    long val = strtol(texte, &endptr, 10);
    if (errno != 0 || endptr == texte || *endptr != '\0' || val < 1 || val > MAX_THREADS) {
        return -1;
    }
    *resultat = (int)val;
    return 0;
}

// Lit le nombre de producteurs et de consommateurs, initialise le mutex et les sémaphores,
// lance tous les threads, attend qu'ils aient fini, puis libère les ressources.
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <nb_producteurs> <nb_consommateurs>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int nb_prod, nb_cons;
    if (lire_entier(argv[1], &nb_prod) != 0 || lire_entier(argv[2], &nb_cons) != 0) {
        fprintf(stderr, "Erreur: les nombres de threads doivent être des entiers entre 1 et %d.\n", MAX_THREADS);
        return EXIT_FAILURE;
    }

    pthread_t prod[nb_prod];
    pthread_t cons[nb_cons];
    prodcons_arg_t args_prod[nb_prod];
    prodcons_arg_t args_cons[nb_cons];

    int err = pthread_mutex_init(&mutex, NULL);
    if (err != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation du mutex.\n");
        return EXIT_FAILURE;
    }
    if (sem_init(&vide, 0, TAILLE) != 0 || sem_init(&plein, 0, 0) != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation des sémaphores.\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < nb_prod; i++) {
        args_prod[i].id = i;
        args_prod[i].nb = part(i, nb_prod, TOTAL);

        err = pthread_create(&prod[i], NULL, producteur, &args_prod[i]);
        if (err != 0) {
            fprintf(stderr, "Erreur lors de la création des threads producteurs.\n");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < nb_cons; i++) {
        args_cons[i].id = i;
        args_cons[i].nb = part(i, nb_cons, TOTAL);

        err = pthread_create(&cons[i], NULL, consommateur, &args_cons[i]);
        if (err != 0) {
            fprintf(stderr, "Erreur lors de la création des threads consommateurs.\n");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < nb_prod; i++) {
        err = pthread_join(prod[i], NULL);
        if (err != 0) {
            fprintf(stderr, "Erreur lors de l'attente des threads producteurs.\n");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < nb_cons; i++) {
        err = pthread_join(cons[i], NULL);
        if (err != 0) {
            fprintf(stderr, "Erreur lors de l'attente des threads consommateurs.\n");
            return EXIT_FAILURE;
        }
    }

    sem_destroy(&vide);
    sem_destroy(&plein);
    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}
