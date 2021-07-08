#include "mediaADT.h"
#include <stdlib.h>
#include <string.h>

#define POS(Y,MIN) ((Y) - (MIN))
#define YEAR(P,MIN) ((P) + (MIN))
#define IS_VALID_YEAR(Y,MIN) ((Y) >= (MIN))

#define SUCCESS 100

enum errorStates {
    CONTENTTYPE_ERROR = 200,
    MEM_ERROR,
    INVALIDYEAR_ERROR,
    NULLPOINTER_ERROR
};

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
    size_t currentIndex; //Iterador por año
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

static int copyStruct(TContent * mediaVec, TContent content, size_t index, TContent * copy){
    mediaVec= realloc(mediaVec, sizeof(TContent)*index);
    if ( mediaVec == NULL){
        return MEM_ERROR;
    }
    mediaVec[index]=content;
    *copy= content;
    return SUCCESS;
}

static int copyContent(TList list, TContent content, contentType title, TContent * copy){
    if (title == CONTENTTYPE_MOVIE){
        list->moviesCount++;
        if ( copyStruct(list->movies, content, list->moviesCount, copy) != MEM_ERROR)
            return CONTENTTYPE_MOVIE;
    }
    else if (title == CONTENTTYPE_SERIES){
        list->seriesCount++;
        if ( copyStruct(list->series, content, list->seriesCount, copy) != MEM_ERROR)
            return CONTENTTYPE_SERIES;
    }
    return MEM_ERROR;
}

static TList addContentByGenre_Rec(TList list, TContent content, char * genre, contentType title, int * flag, TContent * copy) {
    int c;
    if (list == NULL || (c = strcasecmp(genre, list->genre)) < 0) {
        TList newGenre = calloc(1, sizeof(TGenre));
        if (newGenre == NULL) {
            *flag = MEM_ERROR;
            return NULL;
        }
        *flag = copyContent(list, content, title, copy);
        newGenre->next = list;
        return newGenre;
    } else if (c == 0) {
        *flag = copyContent(list, content, title, copy);
        return list;
    }
    list->next = addContentByGenre_Rec(list->next, content, genre, title, flag, copy);
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

int addContent( mediaADT media , TContent content ,unsigned short year , char ** genre , unsigned long numVotes , contentType title){
    int c, t;
    if ( (c=isYearValid(media, year)) == INVALIDYEAR_ERROR){
        return INVALIDYEAR_ERROR;
    }
    if ( title != CONTENTTYPE_MOVIE && title != CONTENTTYPE_SERIES){
        return CONTENTTYPE_ERROR;
    }
    int index= POS(year, media->minYear);
    if ( c == MEM_ERROR){
        media->size= index+1;
        media->years=realloc(media->years, sizeof(TYear)*media->size);
        if (media->years == NULL){
            return MEM_ERROR;
        }
        media->dim++;
    }
    media->years[index]= calloc(1, sizeof(struct year));
    if (media->years[index] == NULL){
        return MEM_ERROR;
    }
    int flag;
    TContent * copy= malloc(sizeof(TContent));
    if ( copy == NULL){
        return MEM_ERROR;
    }
    for ( int i=0; genre[i] != NULL; i++) {
        media->years[index]->genres = addContentByGenre_Rec(media->years[index]->genres, content, genre[i], title, &flag, copy);
        if (flag == MEM_ERROR){
            return MEM_ERROR;
        }
    }
    if ( title == CONTENTTYPE_MOVIE){
        media->years[index]->moviesCount++;
        if ( numVotes> media->years[index]->bestMovieRating ){
            media->years[index]->bestMovieRating = numVotes;
            media->years[index]->bestMovie = copy;
        }

    }
    else {
        media->years[index]->seriesCount++;
        if (numVotes > media->years[index]->bestSeriesRating) {
            media->years[index]->bestSeriesRating = numVotes;
            media->years[index]->bestSeries = copy;
        }
    }
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
        default:
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
        default:
            break;
    }

    return aux;
}

TContent mostVoted(const mediaADT media, const unsigned short year, const contentType CONTENTTYPE_){
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
        default:
            break;
    }

    return mostVotedContent;
}

/**
 * Función auxiliar de iterador que coloca al currentIndex en el siguiente año ocupado/válido.
 * @param media ADT creado para el manejo de películas/series.
 * @param fromIndex Indice desde donde se comienza a buscar el siguiente año ocupado.
 */
static void nextOcuppiedYear(const mediaADT media, const size_t fromIndex){
    for (size_t i = fromIndex; i >= 0 ; --i) {
        if (media->years[i] != NULL){
            media->currentIndex = i;
            return;
        }
    }
}

void toBeginYear(const mediaADT media){
    nextOcuppiedYear(media, media->size);
}

int hasNextYear(const mediaADT media){
    return media->currentIndex > 0;
}

unsigned short nextYear(const mediaADT media){
    if (hasNextYear(media)){
        return INVALIDYEAR_ERROR;
    }

    unsigned short year = YEAR(media->currentIndex, media->minYear);
    nextOcuppiedYear(media, media->currentIndex - 1);
    return year;
}