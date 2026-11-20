#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TOTAL_WRITES 6400
#define TOTAL_READS  25400
#define TRAITEMENT   10000

typedef struct {
    int id;
    int nb_operations;
} rw_arg_t;

pthread_mutex_t mutex_write;
pthread_mutex_t mutex_read;
sem_t wsem;
sem_t rsem;
sem_t z;
int writecount = 0; // nombre de writers
int readcount = 0; // nombre de readers

// Nombre d'éléments que doit traiter le thread numéro i parmi nb_threads :
// total est partagé équitablement, et le thread 0 récupère le reste de la division.
static int part(int i, int nb_threads, int total) {
    return total / nb_threads + (i == 0 ? total % nb_threads : 0);
}

void db_access() {
    for (int i = 0; i < TRAITEMENT; i++);
}

void *writer(void *args) {
    rw_arg_t *data = args;

    for (int i = 0; i < data->nb_operations; i++) {
        pthread_mutex_lock(&mutex_write);
        writecount++;
        if (writecount == 1) {
            sem_wait(&rsem);
        }
        pthread_mutex_unlock(&mutex_write);

        sem_wait(&wsem);
        db_access();
        sem_post(&wsem);

        pthread_mutex_lock(&mutex_write);
        writecount--;
        if (writecount == 0) {
            sem_post(&rsem);
        }
        pthread_mutex_unlock(&mutex_write);
    }

    return NULL;
}

void *reader(void *args) {
    rw_arg_t *data = args;

    for (int i = 0; i < data->nb_operations; i++) {
        sem_wait(&z);
        sem_wait(&rsem);

        pthread_mutex_lock(&mutex_read);
        readcount++;
        if (readcount == 1) {
            sem_wait(&wsem);
        }
        pthread_mutex_unlock(&mutex_read);

        sem_post(&rsem);
        sem_post(&z);

        db_access();

        pthread_mutex_lock(&mutex_read);
        readcount--;
        if (readcount == 0) {
            sem_post(&wsem);
        }
        pthread_mutex_unlock(&mutex_read);
    }

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <nb_writers> <nb_readers>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *endptr = NULL;
    errno = 0;
    long val_nb_writers = strtol(argv[1], &endptr, 10);
    if (errno != 0 || *endptr != '\0' || endptr == argv[1] || val_nb_writers < 1) {
        fprintf(stderr, "Erreur: le nombre d'écrivains doit être un entier supérieur ou égal à 1.\n");
        return EXIT_FAILURE;
    }
    const int nb_writers = (int)val_nb_writers;

    long val_nb_readers = strtol(argv[2], &endptr, 10);
    if (errno != 0 || *endptr != '\0' || endptr == argv[2] || val_nb_readers < 1) {
        fprintf(stderr, "Erreur: le nombre de lecteurs doit être un entier supérieur ou égal à 1.\n");
        return EXIT_FAILURE;
    }
    const int nb_readers = (int)val_nb_readers;

    pthread_t writers[nb_writers];
    pthread_t readers[nb_readers];
    rw_arg_t wargs[nb_writers];
    rw_arg_t rargs[nb_readers];

    // --- SEM INIT ---
    if (sem_init(&wsem, 0, 1) != 0 || sem_init(&rsem, 0, 1) != 0 || sem_init(&z, 0, 1) != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation des sémaphores.\n");
        return EXIT_FAILURE;
    }

    // --- MUTEX INIT ---
     if (pthread_mutex_init(&mutex_write, NULL) != 0 || pthread_mutex_init(&mutex_read, NULL) != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation des mutex.\n");
        return EXIT_FAILURE;
    }

    // --- THREADS CREATE ---
    int err;
    for (int i = 0; i < nb_writers; i++) {
        wargs[i].id = i;
        wargs[i].nb_operations = part(i, nb_writers, TOTAL_WRITES);

        err = pthread_create(&writers[i], NULL, writer, &wargs[i]);
        if (err != 0) {
            fprintf(stderr, "Erreur lors de la création des threads écrivains.\n");
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < nb_readers; i++) {
        rargs[i].id = i;
        rargs[i].nb_operations = part(i, nb_readers, TOTAL_READS);

        err = pthread_create(&readers[i], NULL, reader, &rargs[i]);
        if (err != 0) {
            fprintf(stderr, "Erreur lors de la création des threads lecteurs.\n");
            return EXIT_FAILURE;
        }
    }

    // --- THREADS JOIN ---
    for (int i = nb_writers - 1; i >= 0; i--) {
        err = pthread_join(writers[i], NULL);
        if (err != 0) {
            fprintf(stderr, "Erreur lors de la récupération des valeurs de retour des écrivains.\n");
            return EXIT_FAILURE;
        }
    }
    for (int i = nb_readers - 1; i >= 0; i--) {
        err = pthread_join(readers[i], NULL);
        if (err != 0) {
            fprintf(stderr, "Erreur lors de la récupération des valeurs de retour des lecteurs.\n");
            return EXIT_FAILURE;
        }
    }

    // --- SEM DESTROY ---
    if (sem_destroy(&wsem) != 0 || sem_destroy(&rsem) != 0 || sem_destroy(&z) != 0) {
        fprintf(stderr, "Erreur lors de la destruction des sémaphores.\n");
        return EXIT_FAILURE;
    }

    // --- MUTEX DESTROY ---
    if (pthread_mutex_destroy(&mutex_write) != 0 || pthread_mutex_destroy(&mutex_read) != 0) {
        fprintf(stderr, "Erreur lors de la destruction des mutex.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
