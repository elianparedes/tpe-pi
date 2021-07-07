#include "mediaADT.h"
#include <stdlib.h>
#include <string.h>

#define POS(Y,MIN) ((Y) - (MIN))
#define YEAR(P,MIN) ((P) + (MIN))
#define IS_VALID_YEAR(Y,MIN) ((Y)>=(MIN))

#define SUCCESS 100

enum errorStates {CONTENTTYPE_ERROR = 200 , MEM_ERROR , INVALIDYEAR_ERROR, NULLPOINTER_ERROR };

typedef struct genre{
    char * genre;
    TContent * series;
    TContent * movies;
    size_t moviesCount;
    size_t seriesCount;
    struct genre * next;
} TGenre;

typedef TGenre * TList;

struct year {
    TList genres;
    TContent * bestMovie;
    TContent * bestSeries;
    size_t bestMovieRating;
    size_t bestSeriesRating;
    size_t moviesCount;
    size_t seriesCount;
};

typedef struct year * TYear;

typedef struct mediaCDT{
    TYear * years;
    size_t currentYear; //Iterador por año
    size_t minYear;     //Minimo año posible
    size_t dim;   // cant de memoria ocupada
    size_t size; // cant memoria reservada
} mediaCDT;

mediaADT newMediaADT (const size_t minYear)
{
    mediaADT new = calloc(1,sizeof (mediaCDT));
    //Se setean los extremos del vector dinamico. Inicialmente el extremo superior es igual al inferior
    new->minYear = minYear;
    return new;
}

static unsigned int findTitleType(char * title){
    if (strcmp(title, "movies") == 0){
        return CONTENTTYPE_MOVIE;
    }
    else if ( strcmp(title, "tvSeries") == 0){
        return CONTENTTYPE_SERIES;
    }
    return CONTENTTYPE_ERROR;
}

static int copyStruct(TContent * mediaVec, TContent content, size_t index){
    mediaVec= realloc(mediaVec, sizeof(TContent)*index);
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
static int copyMedia(TList list, TContent content, contentType CONTENTTYPE_){
    if (CONTENTTYPE_ == CONTENTTYPE_MOVIE){
        list->moviesCount++;
        if ( copyStruct(list->movies, content, list->moviesCount) != MEM_ERROR)
            return CONTENTTYPE_MOVIE;
    }
    else if (CONTENTTYPE_ == CONTENTTYPE_SERIES){
        list->seriesCount++;
        if ( copyStruct(list->series, content, list->seriesCount) != MEM_ERROR)
            return CONTENTTYPE_SERIES;
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
static TList addMediaByGenre_Rec(TList list, TContent content, char * genre, contentType title, int * flag) {
    int c;
    if (list == NULL || (c = strcasecmp(genre, list->genre)) < 0) {
        TList newGenre = malloc(sizeof(TGenre));
        if (newGenre == NULL) {
            *flag = MEM_ERROR;
            return list;
        }
        *flag = copyMedia(list, content, title);
        newGenre->next = list;
        return newGenre;
    } else if (c == 0) {
        *flag = copyMedia(list, content, title);
        return list;
    }
    list->next = addMediaByGenre_Rec(list->next, content, genre, title, flag);
    return list;
}

static int isYearValid (mediaADT media , const unsigned short year)
{
    if (IS_VALID_YEAR(year, media->minYear))
        return INVALIDYEAR_ERROR;
    if ( year > YEAR(media->size-1,media->minYear))
        return MEM_ERROR;
    return SUCCESS;
}

size_t countContentByYear(const mediaADT media, const unsigned short year, contentType CONTENTTYPE_ )
{
    if (isYearValid(media, year) != SUCCESS )
        return 0;

    size_t aux;
    switch (CONTENTTYPE_) {
        case CONTENTTYPE_MOVIE:
            aux = media->years[POS(year, media->minYear)]->moviesCount;
            break;
        case CONTENTTYPE_SERIES:
            aux = media->years[POS(year, media->minYear)]->seriesCount;
            break;
    }
    return aux;
}

/**
 * Funcion para buscar un genero en la lista de Generos
 * @param first es el puntero al primer genero de la lista
 * @param genre es el genero a bsucar en la lista
 * @return NULL si el genero no esta en la lista. Si esta en la lista, retorna el puntero al nodo.
 */

static TList searchGenre ( const TList first , const char * genre )
{
    int c;
    if ( first == NULL || (c = strcasecmp(first->genre,genre) ) > 0 )
        return NULL;
    if ( c == 0 )
        return first;
    return searchGenre(first->next , genre) ;
}

size_t countContentByGenre(const mediaADT media, const unsigned short year, const char * genre ,  contentType CONTENTTYPE_ )
{
    if (isYearValid(media, year) != SUCCESS)
        return INVALIDYEAR_ERROR;

    TList auxGenre = searchGenre(media->years[POS(year, media->minYear)]->genres , genre );
    if ( auxGenre == NULL )
        return NULLPOINTER_ERROR;

    size_t aux;
    switch (CONTENTTYPE_) {
        case CONTENTTYPE_MOVIE:
            aux = auxGenre->moviesCount;
            break;
        case CONTENTTYPE_SERIES:
            aux = auxGenre->seriesCount;
            break;
    }

    return aux;
}

TContent mostVoted(const mediaADT media, const unsigned short year, contentType CONTENTTYPE_){
    TContent mostVotedContent  = {0};

    if (isYearValid(media, year) != SUCCESS)
        return mostVotedContent;

    switch (CONTENTTYPE_) {
        case CONTENTTYPE_MOVIE:
            mostVotedContent = *(media->years[POS(year,media->minYear)]->bestMovie);
            break;
        case CONTENTTYPE_SERIES:
            mostVotedContent = *(media->years[POS(year, media->minYear)]->bestSeries);
            break;
    }

    return mostVotedContent;
}