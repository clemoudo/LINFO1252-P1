#ifndef P1_READ_WRITE_UTILS_H
#define P1_READ_WRITE_UTILS_H

#endif //P1_READ_WRITE_UTILS_H

// Nombre d'éléments que doit traiter le thread numéro i parmi nb_threads :
// TOTAL est partagé équitablement, et le thread 0 récupère le reste de la division.
static int part(int i, int nb_threads, int total) {
    return total / nb_threads + (i == 0 ? total % nb_threads : 0);
}
