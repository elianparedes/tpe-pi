#include "mediaADT.h"
#include <stdlib.h>
#include <string.h>

#define POS(Y,MIN) ((Y) - (MIN))            /**< @def Macro para obtener posición en vector de punteros a TYear */
#define YEAR(P,MIN) ((P) + (MIN))           /**< @def Macro para obtener el año a partir de un índice */
#define IS_VALID_YEAR(Y,MIN) ((Y) >= (MIN)) /**< @def Macro que devuelve 1 si el año es válido para operar en el TAD o 0 si no lo es */

#define SUCCESS 100   /**< @def Constante númerica para indicar que una operación se realizó exitosamente */

#define CHECK_MEM(PTR) { if( (PTR) == NULL)   \
                            return MEM_ERROR; }

/**
 *  @brief Struct para manejar películas y series en un género determinado
 */
typedef struct genre{
    char genre[MAX_GENRE_SIZE]; /**< Nombre del género                                     */
    TContent * series;          /**< Vector que contiene las series añadidas               */
    TContent * movies;          /**< Vector que contiene las películas añadidas            */
    size_t moviesCount;         /**< Cantidad de películas añadidas en el género           */
    size_t seriesCount;         /**< Cantidad de series añadidas en el género              */
    struct genre * next;        /**< Puntero al siguiente struct genre (para formar lista) */
} TGenre;

typedef TGenre * TList;

/**
 * @brief Struct para manejar películas y series en un año determinado
 */
struct year {
    TList genres;              /**< Lista ordenada alfabéticamente por géneros de películas y series */
    TContent bestMovie;        /**< Película con mayor cantidad de votos en el año                   */
    TContent bestSeries;       /**< Serie con mayor cantidad de votos en el año                      */
    size_t bestMovieRating;    /**< Cantidad de votos de bestMovie                                   */
    size_t bestSeriesRating;   /**< Cantidad de votos de bestSeries                                  */
    size_t moviesCount;        /**< Cantidad de películas añadidas                                   */
    size_t seriesCount;        /**< Cantidad de series añadidas                                      */
};

typedef struct year * TYear;

/**
 * @brief TAD para el manejo de películas y series
 */
typedef struct mediaCDT{
    TYear * years;              /**< Vector de punteros a TYear para guardar las películas y series por año             */
    TList currentGenre;         /**< Iterador por género                                                                */
    size_t currentIndex;        /**< Iterador por años                                                                  */
    size_t minYear;             /**< Año mínimo de comienzo de película/serie que aceptará el TAD para añadir contenido */
    size_t dim;                 /**< Cantidad de años ocupados (es decir, que contienen al menos una película/serie)    */
    size_t size;                /**< Cantidad total de años reservados en memoria                                       */
} mediaCDT;

mediaADT newMediaADT (const size_t minYear)
{
    mediaADT new = calloc(1,sizeof (mediaCDT));
    //Se setean los extremos del vector dinamico. Inicialmente el extremo superior es igual al inferior
    new->minYear = minYear;
    return new;
}

/**
 * @brief Función auxiliar que copia en un vector una película/serie.
 *
 * @param contentVec Vector de punteros a TContent en el que será copiado al final la nueva película/serie.
 * @param content Película/serie que será copiada.
 * @param index Índice del vector en el que será copiado la película/serie.
 * @return Puntero al comienzo del vector.
 */
static TContent * copyStruct(TContent * contentVec, const TContent content, const size_t index){
    if (index % MEM_BLOCK == 0){
        contentVec = realloc(contentVec, sizeof(TContent)*(index + MEM_BLOCK));
        if ( contentVec == NULL){
            return NULL;
        }
    }
    contentVec[index]=content;
    return contentVec;
}

/**
 * @brief Función auxiliar que añade pelicula/serie dentro de un género, actualizando struct genre para reflejar el
 * contenido añadido.
 *
 * @param genre Puntero a struct genre (TList)
 * @param content Película/serie que será añadida.
 * @param title Indica si el contenido a añadir es una película o una serie.
 * @return CONTENTTYPE_MOVIE si fue añadida una película, CONTENTTYPE_SERIES si fue añadida una serie.
 */
static int copyContent(TList genre, const TContent content, const contentType title){
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

/**
 * @brief Función auxiliar recursiva que añade película/serie en un mediaADT en un año y género determinado.
 *
 * @param listG Puntero a struct genre (TList) que contiene las películas/series añadidas dentro de un género en
 * específico, junto a la cantidad añadida de las mismas.
 * @param content Película/serie que será añadida.
 * @param genre Género de la película/serie.
 * @param title Indica si el contenido a añadir es una película o una serie.
 * @param flag Al finalizar la ejecución de la función apuntará a MEM_ERROR si hubo un error de asignación de memoria,
 * CONTENTTYPE_MOVIE si fue añadida una película o CONTENTTYPE_SERIES si fue añadida una serie.
 * @return Puntero a struct genre (TList) al ser una función recursiva.
 */
static TList addContentByGenre_Rec(TList listG, const TContent content, const char * genre, const contentType title, int * flag) {
    int c;
    if (listG == NULL || (c = strcasecmp(genre, listG->genre)) < 0) {
        TList newGenre = calloc(1, sizeof(TGenre));
        if (newGenre == NULL) {
            *flag = MEM_ERROR;
            return NULL;
        }
        strcpy(newGenre->genre, genre);
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

/**
 * @brief Función auxiliar que indica mediante distintas constantes si un año es "valido" o no con respecto a un mediaADT.
 *
 * @param media Media ADT creado para el manejo de películas/series.
 * @param year Año que será evaluado.
 * @return INVALIDYEAR_ERROR si el año es menor al año mínimo indicado en el mediaADT, MEM_ERROR si el
 * año es mayor a la memoria que fue asignada en el mediaADT y SUCCESS si no se cumple ninguna de las 2 condiciones
 * mencionadas anteriormente.
 */
static int isYearValid (mediaADT media , const unsigned short year)
{
    if (!IS_VALID_YEAR(year, media->minYear))
        return INVALIDYEAR_ERROR;
    if ( year > YEAR(media->size-1,media->minYear))
        return MEM_ERROR;
    return SUCCESS;
}

/**
 * @brief Función auxiliar que llena con NULL un vector de tipo TYear * para indicar que no tiene contenido dentro.
 *
 * @param years Vector que será recorrido y llenado con NULL.
 * @param from Indica el comienzo de recorrido.
 * @param to Indica el final de recorrido.
 */
static void setNotOcuppied ( TYear * years , size_t from , const size_t to ){
    while ( from <= to )
    {
        years[from++] = NULL;
    }
}

/**
 * @brief Función auxiliar que actualiza película/serie con mayor cantidad de votos e indica la nueva cantidad en mediaADT.
 *
 * @param year Puntero a struct year que contiene las películas/series añadidas a mediaADT con comienzo en un año
 * determinado.
 * @param content Película/serie que será evaluada con el mediaADT.
 * @param numVotes Cantidad de votos de la película/serie que será evaluada.
 * @param title Indica si el contenido a evaluar es una película o una serie.
 */
static void setRating(TYear year, const TContent content, const unsigned long numVotes, const contentType title){
    if ( title == CONTENTTYPE_MOVIE){
        if ( numVotes > year->bestMovieRating){
            year->bestMovieRating = numVotes;
            year->bestMovie = content;
        }
    }
    else{
        if (numVotes > year->bestSeriesRating){
            year->bestSeriesRating= numVotes;
            year->bestSeries = content;
        }
    }
}

int addContent( mediaADT media , const TContent content , const unsigned short year , const char ** genre , const unsigned long numVotes , const contentType title){
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
        memset(media->years + media->size, 0, (index - media->size + 1) * sizeof (TYear));
        media->size= index+1;
    }

    if (media->years[index] == NULL){
        media->years[index]= calloc(1, sizeof(struct year));
        CHECK_MEM(media->years[index]);
        media->dim++;
    }
    int flag;

    for ( int i=0; genre[i] != NULL; i++) {
        media->years[index]->genres = addContentByGenre_Rec(media->years[index]->genres, content, genre[i], title, &flag);
        if (flag == MEM_ERROR){
            return MEM_ERROR;
        }
    }

    if ( title == CONTENTTYPE_MOVIE){
        (media->years[index]->moviesCount)++;
        if ( numVotes > media->years[index]->bestMovieRating){
            media->years[index]->bestMovieRating = numVotes;
            media->years[index]->bestMovie = content;
        }
    }
    else {
        (media->years[index]->seriesCount++);
        if (numVotes > media->years[index]->bestSeriesRating){
            media->years[index]->bestSeriesRating= numVotes;
            media->years[index]->bestSeries = content;
        }
    }

    return 1;
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
    return searchGenre(first->next , genre);
}

size_t countContentByGenre(const mediaADT media, const unsigned short year, const char * genre ,  contentType CONTENTTYPE_ )
{
    if (isYearValid(media, year) != SUCCESS)
        return 0;

    TList auxGenre = searchGenre(media->years[POS(year, media->minYear)]->genres , genre );
    if ( auxGenre == NULL )
        return 0;

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
 * Función auxiliar de iterador que coloca al currentIndex en el siguiente año ocupado/válido.
 * @param media ADT creado para el manejo de películas/series.
 * @param fromIndex Indice desde donde se comienza a buscar el siguiente año ocupado.
 */
static void nextOcuppiedYear(const mediaADT media, const size_t fromIndex) {
    for (size_t i = fromIndex; i > 0; --i) {
        if (media->years[i] != NULL) {
            media->currentIndex = i;
            return;
        }
    }
    media->currentIndex = media->size;
}

void toBeginYear(const mediaADT media){
    nextOcuppiedYear(media, media->size - 1);
}

int hasNextYear(const mediaADT media){
    return media->currentIndex < media->size;
}

unsigned short nextYear(const mediaADT media){
    if (!hasNextYear(media)){
        return RANGE_ERROR;
    }

    unsigned short year = YEAR(media->currentIndex, media->minYear);
    nextOcuppiedYear(media, media->currentIndex - 1);
    return year;
}

/**
 * @brief Función auxiliar que libera los recursos reservados por una lista de structs genre.
 *
 * @param genre Puntero a struct genre (TList) que contiene las películas/series añadidas dentro de un género en
 * específico, junto a la cantidad añadida de las mismas.
 */
static void freeGenres_Rec(TList genre){
    if ( genre == NULL){
        return;
    }
    freeGenres_Rec(genre->next);
    free(genre->movies);
    free(genre->series);
    free(genre);
}

/**
 * @brief Función auxiliar que libera los recursos reservados por struct year y su puntero.
 *
 * @param year Puntero a struct year que contiene las películas/series añadidas a mediaADT con comienzo en un año
 * determinado.
 */
static void freeYear(TYear year){
    freeGenres_Rec(year->genres);
    free(year);
}

void freeMediaADT(mediaADT media){
    if ( media->size == 0){
        return;
    }
    for (size_t i=0; i < media->size; i++){
        if (media->years[i] != NULL){
            freeYear(media->years[i]);
        }
    }
    free(media->years);
    free(media);
}

int toBeginGenre (const mediaADT media , const unsigned short year )
{
    // Se verifica que el año sea valido
    if (isYearValid(media,year) != SUCCESS )
        return INVALIDYEAR_ERROR;

    TYear aux = media->years[POS(year,media->minYear)];
    //Se verifica que el año pedido tenga contenido
    CHECK_MEM(aux)

    media->currentGenre = aux->genres;
    return 1;
}

int hasNextGenre ( const mediaADT media )
{
    return media->currentGenre != NULL;
}

char * nextGenre ( const mediaADT media )
{
    if ( !hasNextGenre(media) )
        return NULL;
    char * aux = media->currentGenre->genre;
    media->currentGenre = media->currentGenre->next;
    return aux;
}
