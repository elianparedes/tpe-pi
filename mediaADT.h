#ifndef TPEFINAL_MEDIAADT_H
#define TPEFINAL_MEDIAADT_H

#include <stdlib.h>

typedef struct mediaCDT * mediaADT;

typedef struct media{
    char * titleType;
    char * primaryTitle;
    char ** genres;
    unsigned short startYear;
    unsigned short endYear;
    unsigned short runtimeMinutes;
    unsigned long numVotes;
    float averageRating;
} TMedia;

/**
 * Crea un nuevo mediaADT para el manejo de peliculas / series.
 * @param minYear el menor año de estreno para peliculas / series
 * @return
 */
mediaADT newMediaADT ( const size_t minYear);

#endif //TPEFINAL_MEDIAADT_H
