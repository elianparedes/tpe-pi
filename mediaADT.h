#ifndef TPEFINAL_MEDIAADT_H
#define TPEFINAL_MEDIAADT_H

#include <stdlib.h>

typedef enum {CONTENTTYPE_MOVIE=1, CONTENTTYPE_SERIES} contentType; //enum con constantes de títulos válidos

typedef struct mediaCDT * mediaADT;

typedef struct content{
    char * titleType;
    char * primaryTitle;
    char ** genres;
    unsigned short startYear;
    unsigned short endYear;
    unsigned short runtimeMinutes;
    unsigned long numVotes;
    float averageRating;
} TContent;

/**
 * Crea un nuevo mediaADT para el manejo de peliculas / series.
 * @param minYear el menor año de estreno para peliculas / series
 * @return
 */
mediaADT newMediaADT(const size_t minYear);

int addContent( mediaADT media , TContent content ,unsigned short year , char ** genre , unsigned long numVotes , contentType title);

/**
 * Funcion para obtener la cantidad de peliculas / series para un año.
 * @param media es el ADT creado para el manejo de peliculas / series
 * @param year es el año del cual se solicita la cantidad de peliculas o de series.
 * @param MEDIATYPE_ es el tipo de contenido del cual se desea conocer su cantidad para el año indicado.Distinguiendo entre pelicula o serie.
 * @return 0 si el año es invalido o si no hay registro de peliculas / series para ese año.
 */
size_t countContentByYear(const mediaADT media, const unsigned short year, contentType MEDIATYPE_);

/**
 * Funcion para obtener la cantidad de peliculas / series para un genero en especifico de un año en especifico
 * @param media es el ADT creado para el manejo de peliculas / series
 * @param year es el año del cual se solicita la cantidad de peliculas o de series.
 * @param genre es el genero del cual se solicita la cantidad de peliculas o de series para el año especificado.
 * @param MEDIATYPE_ es el tipo de contenido del cual se desea conocer su cantidad para el año y genero indicado.Distinguiendo entre pelicula o serie.
 * @return 0 si el año o el genero son invalido o si no hay registro de peliculas / series para ese genero.
 */
size_t countContentByGenre(const mediaADT media, const unsigned short year, const char * genre ,  contentType MEDIATYPE_ );

#endif //TPEFINAL_MEDIAADT_H
