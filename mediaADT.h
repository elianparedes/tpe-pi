#ifndef TPEFINAL_MEDIAADT_H
#define TPEFINAL_MEDIAADT_H

#include <stdlib.h>

typedef enum {MEDIATYPE_MOVIE=2, MEDIATYPE_SERIES} mediaType; //enum con constantes de títulos válidos

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
mediaADT newMediaADT(const size_t minYear);

/**
 * Funcion para obtener la cantidad de peliculas / series para un año.
 * @param media es el ADT creado para el manejo de peliculas / series
 * @param year es el año del cual se solicita la cantidad de peliculas o de series.
 * @param MEDIATYPE_ es el tipo de contenido del cual se desea conocer su cantidad para el año indicado.
 * Distinguiendo entre pelicula o serie.
 * @return 0 si el año es invalido o si no hay registro de peliculas / series para ese año.
 */
size_t countContentByYear(const mediaADT media, const unsigned short year, mediaType MEDIATYPE_);

#endif //TPEFINAL_MEDIAADT_H
