#include "mediaADT.h"

#define POS(Y,MIN) ( (Y) - (MIN) )
#define YEAR(P,MIN) ( (P) + (MIN) )
#define IS_VALID_YEAR(Y,MIN) ((Y)>=(MIN))

typedef struct genre{
    char * genre;
    TMedia * series;
    TMedia * movies;
    size_t moviesCount;
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


mediaADT newMediaADT ( const size_t minYear )
{
    mediaADT new = calloc(1,sizeof (mediaCDT));
    //Se setean los extremos del vector dinamico. Inicialmente el extremo superior es igual al inferior
    new->minYear = new->maxYear = minYear;
    return new;
}

size_t countContentByYear( const mediaADT media , const unsigned short year , mediaType MEDIATYPE_ )
{
    if (!IS_VALID_YEAR(year, media->minYear) || media->years == NULL)
        return 0;
    size_t aux;
    switch (MEDIATYPE_) {
        case MEDIATYPE_MOVIE:
            aux = media->years[POS(year, media->minYear)].moviesCount;
            break;
        case MEDIATYPE_SERIES:
            aux = media->years[POS(year, media->minYear)].seriesCount;
            break;
    };
    return aux;
}