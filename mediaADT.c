#include "mediaADT.h"

#include <stdlib.h>
#include <string.h>

#define MEDIATYPE_ERROR -1     // titleType inválido
#define MEM_ERROR 0
#define SUCCESS 1

typedef struct genre{
    char * genre;
    TMedia * series;
    TMedia * movies;
    size_t moviesCount;
    size_t seriesCount;
    struct genre * next;
} TGenre;

typedef TGenre * TList;

typedef struct year {
    unsigned short year;
    size_t moviesCount;
    size_t seriesCount;
    TMedia * bestMovie;
    TMedia * bestSeries;
    TList genres;
} TYear;

typedef struct mediaCDT{
    TYear * years;
    size_t currentYear;
    size_t minYear;
    size_t maxYear;
    size_t dim;
    size_t size;
}mediaCDT;

static unsigned int findTitleType(char * title){
    if (strcmp(title, "movies")==0){
        return MEDIATYPE_MOVIE;
    }
    else if ( strcmp(title, "tvSeries") == 0){
        return MEDIATYPE_SERIES;
    }
    return MEDIATYPE_ERROR;
}

static int copyStruct(TMedia * mediaVec, TMedia content, size_t index){
    mediaVec= realloc(mediaVec, sizeof(TMedia)*index);
    if ( mediaVec == NULL){
        return MEM_ERROR;
    }
    mediaVec[index]=content;
    return SUCCESS;
}

/**
 * Funcion auxiliar de addMediaByGenre_Rec que agrega película/serie a TGenre
 * @param list nodo al género a copiar
 * @param content película/serie que será copiada
 * @param title variable con MOVIE si content es una película o SERIES si es una serie
 * @return
 */
static int copyMedia(TList list, TMedia content, mediaType title){
    if (title == MEDIATYPE_MOVIE){
        list->moviesCount++;
        if ( copyStruct(list->movies, content, list->moviesCount) != MEM_ERROR)
            return MEDIATYPE_MOVIE;
    }
    else if ( title == MEDIATYPE_SERIES){
        list->seriesCount++;
        if ( copyStruct(list->series, content, list->seriesCount) != MEM_ERROR)
            return MEDIATYPE_SERIES;
    }
    return MEM_ERROR;
}


/**
 * Función auxiliar recursiva que agrega película/serie a la lista de géneros
 * @param list lista de géneros de películas/series
 * @param content película/serie que será copiada
 * @param genre género de la película/serie
 * @param title variable con MOVIE si content es una película o SERIES si es una serie
 * @param flag indica con MEM_ERROR si hubo un error de memoria al finalizar la ejecucion, MOVIE si se copió una
 * película o SERIES si se copió una serie.
 * @return
 */
static TList addMediaByGenre_Rec(TList list, TMedia content, char * genre, mediaType title, int * flag) {
    int c;
    if (list == NULL || (c = strcmp(genre, list->genre)) < 0) {
        TList newGenre = malloc(sizeof(TGenre));
        if (newGenre == NULL) {
            *flag = MEM_ERROR;
            return list;
        }
        *flag = copyMedia(list, content, title);
        return list;
    } else if (c == 0) {
        *flag = copyMedia(list, content, title);
        return list;
    }
    list->next = addMediaByGenre_Rec(list->next, content, genre, title, flag);
    return list;
}

mediaADT newMediaADT ( const size_t minYear )
{
    mediaADT new = calloc(1,sizeof (mediaCDT));
    new->minYear = new->maxYear = minYear;
    return new;
}


