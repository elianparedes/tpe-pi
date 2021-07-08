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

#define CHECK_MEM(PTR) { if( (PTR) == NULL)   \
                            return MEM_ERROR; }

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
    TContent bestMovie;
    TContent bestSeries;
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

static TContent * copyStruct(TContent * contentVec, TContent content, size_t index){
    contentVec= realloc(contentVec, sizeof(TContent)*(index+1));
    if ( contentVec == NULL){
        return NULL;
    }
    contentVec[index]=content;
    return contentVec;
}

static int copyContent(TList genre, TContent content, contentType title){
    if (title == CONTENTTYPE_MOVIE){
        if ( (genre->movies= copyStruct(genre->movies, content, genre->moviesCount)) != NULL) {
            genre->moviesCount++;
            return CONTENTTYPE_MOVIE;
        }
    }
    else{
        if ( (genre->series = copyStruct(genre->series, content, genre->seriesCount)) != NULL){
            genre->seriesCount++;
            return CONTENTTYPE_SERIES;
        }
    }
    return MEM_ERROR;
}

static TList addContentByGenre_Rec(TList listG, TContent content, char * genre, contentType title, int * flag) {
    int c;
    if (listG == NULL || (c = strcasecmp(genre, listG->genre)) < 0) {
        TList newGenre = calloc(1, sizeof(TGenre));
        if (newGenre == NULL) {
            *flag = MEM_ERROR;
            return NULL;
        }
        newGenre->genre=genre;
        *flag = copyContent(newGenre, content, title);
        newGenre->next = listG;
        return newGenre;
    } else if (c == 0) {
        *flag = copyContent(listG, content, title);
        return listG;
    }
    listG->next = addContentByGenre_Rec(listG->next, content, genre, title, flag);
    return listG;
}

static int isYearValid (mediaADT media , const unsigned short year)
{
    if (!IS_VALID_YEAR(year, media->minYear))
        return INVALIDYEAR_ERROR;
    if ( year > YEAR(media->size-1,media->minYear))
        return MEM_ERROR;
    return SUCCESS;
}

static void setNotOcuppied ( TYear * years , size_t from , size_t to ){
    while ( from <= to )
    {
        years[from++] = NULL;
    }
}

static void setRating(TYear year, TContent content, unsigned long numVotes, contentType title){
    if ( title == CONTENTTYPE_MOVIE){
        if ( numVotes > year->bestMovieRating){
            year->bestMovieRating= numVotes;
            year->bestMovie= content;
        }
    }
    else{
        if (numVotes > year->bestSeriesRating){
            year->bestSeriesRating= numVotes;
            year->bestSeries = content;
        }
    }
}

int addContent( mediaADT media , TContent content ,unsigned short year , char ** genre , unsigned long numVotes , contentType title){
    int c;
    if ( (c=isYearValid(media, year)) == INVALIDYEAR_ERROR){
        return INVALIDYEAR_ERROR;
    }
    if ( title != CONTENTTYPE_MOVIE && title != CONTENTTYPE_SERIES){
        return CONTENTTYPE_ERROR;
    }
    int index= POS(year, media->minYear);
    if ( media->size == 0 || c == MEM_ERROR){
        media->years=realloc(media->years, sizeof(TYear)*(index+1));
        CHECK_MEM(media->years);
        setNotOcuppied(media->years, media->size, index);
        media->size= index+1;
    }

    if (media->years[index] == NULL){
        media->years[index]= calloc(1, sizeof(struct year));
        CHECK_MEM(media->years[index]);
        media->dim++;
    }
    int flag;
    TContent * copy=NULL;
    for ( int i=0; genre[i] != NULL; i++) {
        media->years[index]->genres = addContentByGenre_Rec(media->years[index]->genres, content, genre[i], title, &flag);
        if (flag == MEM_ERROR){
            return MEM_ERROR;
        }
    }

    if ( title == CONTENTTYPE_MOVIE){
        (media->years[index]->moviesCount)++;
    }
    else {
        (media->years[index]->seriesCount++);
    }

    setRating(media->years[index], content, numVotes, title);

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
            mostVotedContent = media->years[POS(year,media->minYear)]->bestMovie;
            break;
        case CONTENTTYPE_SERIES:
            mostVotedContent = media->years[POS(year, media->minYear)]->bestSeries;
            break;
        default:
            break;
    }

    return mostVotedContent;
}

/**
 * Función auxiliar de iterador que permite coloca al currentIndex en el siguiente año ocupado/válido.
 * @param media ADT creado para el manejo de películas/series.
 * @param fromIndex Indice desde donde se comienza a buscar el siguiente año ocupado.
 */
static void nextOcuppiedYear(const mediaADT media, const size_t fromIndex){
    for (size_t i = fromIndex; i < media->size ; ++i) {
        if (media->years[i] != NULL){
            media->currentIndex = i;
            return;
        }
    }
}

void toBeginYear(const mediaADT media){
    nextOcuppiedYear(media, 0);
}

int hasNextYear(const mediaADT media){
    return media->currentIndex < media->size;
}

unsigned short nextYear(const mediaADT media){
    if (hasNextYear(media)){
        return INVALIDYEAR_ERROR;
    }

    unsigned short year = YEAR(media->currentIndex, media->minYear);
    nextOcuppiedYear(media, media->currentIndex + 1);
    return year;
}