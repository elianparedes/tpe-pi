#include "mediaADT.h"
#include <stdlib.h>
#include <string.h>

#define MEM_ERROR 0
#define SUCCESS 1

enum validTypes {WRONG_TITLETYPE=2, MOVIE, SERIES}; //enum con constantes de títulos válidos para utilizar en la inserción

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

struct mediaCDT{
    TYear * years;
    size_t currentYear;
    size_t minYear;
    size_t maxYear;
    size_t dim;
    size_t size;
};

static unsigned int findTitleType(char * title){
    if (strcmp(title, "movies")==0){
        return MOVIE;
    }
    else if ( strcmp(title, "tvSeries") == 0){
        return SERIES;
    }
    return WRONG_TITLETYPE;
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
static int copyMedia(TList list, TMedia content, unsigned int title){
    if (title == MOVIE){
        list->moviesCount++;
        if ( copyStruct(list->movies, content, list->moviesCount) != MEM_ERROR)
            return MOVIE;
    }
    else if ( title == SERIES){
        list->seriesCount++;
        if ( copyStruct(list->series, content, list->seriesCount) != MEM_ERROR)
            return MOVIE;
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
static TList addMediaByGenre_Rec(TList list, TMedia content, char * genre, unsigned int title, int * flag){
    int c;
    if (list == NULL || (c=strcmp(genre, list->genre)) < 0){
        TList newGenre= malloc(sizeof(TGenre));
        if (newGenre == NULL){
            *flag=MEM_ERROR;
            return list;
        }
        *flag= copyMedia(list, content, title);
        return list;
    }
    else if (c==0){
        *flag= copyMedia(list, content, title);
        return list;
    }
    list->next= addMediaByGenre_Rec(list->next, content, genre, title, flag);
    return list;
}


