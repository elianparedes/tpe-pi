#include "mediaADT.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define POS(Y,MIN) ((Y) - (MIN))            /**< @def Macro para obtener posicion en vector de punteros a TYear */
#define YEAR(P,MIN) ((P) + (MIN))           /**< @def Macro para obtener el año a partir de un indice */
#define IS_VALID_YEAR(Y,MIN) ((Y) >= (MIN)) /**< @def Macro que devuelve 1 si el año es valido para operar en el TAD o 0 si no lo es */

#define SUCCESS 100   /**< @def Constante numerica para indicar que una operacion se realizo exitosamente */

/**< @def Macro que devuelve MEM_ERROR si el puntero no se asigno correctamente (si es NULL) */
#define CHECK_MEM(PTR) { if( (PTR) == NULL)   \
                            return MEM_ERROR; }

/**
 *  @brief Struct para manejar películas y series en un genero determinado
 */
typedef struct genre{
    char genre[MAX_GENRE_SIZE]; /**< Nombre del genero                                     */
    TContent * series;          /**< Vector que contiene las series añadidas               */
    TContent * movies;          /**< Vector que contiene las peliculas añadidas            */
    size_t moviesCount;         /**< Cantidad de peliculas añadidas en el genero           */
    size_t seriesCount;         /**< Cantidad de series añadidas en el genero              */
    struct genre * next;        /**< Puntero al siguiente struct genre (para formar lista) */
} TGenre;

typedef TGenre * TList;

/**
 * @brief Struct para manejar peliculas y series en un año determinado
 */
struct year {
    TList genres;              /**< Lista ordenada alfabeticamente por géneros de películas y series */
    TContent bestMovie;        /**< Pelicula con mayor cantidad de votos en el año                   */
    TContent bestSeries;       /**< Serie con mayor cantidad de votos en el año                      */
    size_t bestMovieRating;    /**< Cantidad de votos de bestMovie                                   */
    size_t bestSeriesRating;   /**< Cantidad de votos de bestSeries                                  */
    size_t moviesCount;        /**< Cantidad de películas añadidas                                   */
    size_t seriesCount;        /**< Cantidad de series añadidas                                      */
};

typedef struct year * TYear;

/**
 * @brief TAD para el manejo de peliculas y series
 */
typedef struct mediaCDT{
    TYear * years;              /**< Vector de punteros a TYear para guardar las películas y series por año             */
    TList currentGenre;         /**< Iterador por genero                                                                */
    size_t currentIndex;        /**< Iterador por años                                                                  */
    size_t minYear;             /**< Año minimo de comienzo de pelicula/serie que aceptara el TAD para añadir contenido */
    size_t dim;                 /**< Cantidad de años ocupados (es decir, que contienen al menos una película/serie)    */
    size_t size;                /**< Cantidad total de años reservados en memoria                                       */
} mediaCDT;

mediaADT newMediaADT (const size_t minYear)
{
    mediaADT new = calloc(1,sizeof (mediaCDT));
    /// Se setean los extremos del vector dinamico. Inicialmente el extremo superior es igual al inferior.
    new->minYear = minYear;
    return new;
}

/**
 * @brief Funcion auxiliar que copia en un vector una pelicula/serie.
 *
 * @details Se reserva memoria a bloques para evitar reallocs cada vez que se ejecute la funcion. A su vez, se le permite
 * al usuario elegir si desea un bloque mas grande o mas chico mediante MEM_BLOCK. De esta forma, si se añade una gran cantidad de
 * peliculas y series no se incrementara el tiempo de ejecucion en gran manera.
 * Al finalizar la carga, algunas posiciones del vector podrian quedar vacias. En este caso, se priorizo tiempo de ejecucion
 * sobre memoria debido a que podria haber una gran carga de datos.
 *
 * @param contentVec Vector de tipo TContent en el que sera copiado al final la nueva película/serie.
 * @param content Pelicula/serie que sera copiada.
 * @param index Indice del vector en el que sera copiado la pelicula/serie.
 * @return Puntero al comienzo del vector.
 */
static TContent * copyStruct(TContent * contentVec, const TContent content, const size_t index){
    /// Si el indice llega al bloque de memoria, se expandirá el vector
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
 * @brief Funcion auxiliar que añade pelicula/serie dentro de un genero, actualizando struct genre para reflejar el
 * contenido añadido.
 *
 * @param genre Puntero a struct genre (TList)
 * @param content Pelicula/serie que será añadida.
 * @param title Indica si el contenido a añadir es una película o una serie.
 * @return CONTENTTYPE_MOVIE si fue añadida una película.
 * @return CONTENTTYPE_SERIES si fue añadida una serie.
 * @return MEM_ERROR si se produjo un error de memoria.
 */
static int copyContent(TList genre, const TContent content, const contentType title){
    /// Evalua si es una pelicula o una serie y llama a copyStruct
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
 * @brief Funcion auxiliar recursiva que añade película/serie en un mediaADT en un año y genero determinado.
 *
 * @param listG Puntero a struct genre (TList) que contiene las peliculas/series añadidas dentro de un genero en
 * especifico, junto a la cantidad añadida de las mismas.
 * @param content Pelicula/serie que sera añadida.
 * @param genre Genero de la pelicula/serie.
 * @param title Indica si el contenido a añadir es una pelicula o una serie.
 * @param flag Al finalizar la ejecución de la función apuntará a MEM_ERROR si hubo un error de asignacion de memoria,
 * CONTENTTYPE_MOVIE si fue añadida una pelicula o CONTENTTYPE_SERIES si fue añadida una serie.
 * @return Puntero a struct genre (TList) al ser una funcion recursiva.
 */
static TList addContentByGenre_Rec(TList listG, const TContent content, const char * genre, const contentType title, int * flag) {
    int c;
    if (listG == NULL || (c = strcasecmp(genre, listG->genre)) < 0) {
        TList newGenre = calloc(1, sizeof(TGenre)); /// Si el genero no existia, se crea un nuevo nodo.
        if (newGenre == NULL) {
            *flag = MEM_ERROR;
            return NULL;
        }
        strcpy(newGenre->genre, genre);
        *flag = copyContent(newGenre, content, title);
        newGenre->next = listG;
        return newGenre;

    } else if (c == 0) {                                  /// Si el genero existia, llama directamente a addContent
        *flag = copyContent(listG, content, title);
        return listG;
    }
    listG->next = addContentByGenre_Rec(listG->next, content, genre, title, flag);
    return listG;
}

/**
 * @brief Funcion auxiliar que indica mediante distintas constantes si un año es "valido" o no con respecto a un mediaADT.
 *
 * @param media ADT creado para el manejo de peliculas/series.
 * @param year Año que sera evaluado.
 * @return INVALIDYEAR_ERROR si el año es menor al año mínimo indicado en el mediaADT.
 * @return MEM_ERROR si el año es mayor a la memoria que fue asignada en el mediaADT.
 * @return SUCCESS si no se cumple ninguna de las 2 condiciones mencionadas anteriormente.
 */
static int isYearValid (mediaADT media , const unsigned short year)
{
    if (!IS_VALID_YEAR(year, media->minYear))
        return INVALIDYEAR_ERROR;
    if ( year > YEAR(media->size-1,media->minYear))
        return MEM_ERROR;
    return SUCCESS;
}

int addContent( mediaADT media , const TContent content , const unsigned short year , char ** genre , const unsigned long numVotes , const contentType title){
    int c;
    /// Se valida si el año pasado como parametro es válido dentro del mediaADT
    if ( (c=isYearValid(media, year)) == INVALIDYEAR_ERROR){
        return INVALIDYEAR_ERROR;
    }

    /// Se valida si el contenido a añadir es una pelicula o serie
    if ( title != CONTENTTYPE_MOVIE && title != CONTENTTYPE_SERIES){
        return CONTENTTYPE_ERROR;
    }
    int index= POS(year, media->minYear);

    /// Si es la primera vez que se añade una pelicula/serie, o el año pasado como parametro es mayor al que puede
    /// acceder el vector de años en el ADT (no se puede acceder al indice), se reserva memoria para los años y se
    /// inicializa con ceros los campos de bestMovieRating, bestSeriesRating, moviesCount y seriesCount para los mismos
    if ( media->size == 0 || c == MEM_ERROR){
        media->years=realloc(media->years, sizeof(TYear)*(index+1));
        CHECK_MEM(media->years);
        memset(media->years + media->size, 0, (index - media->size + 1) * sizeof (TYear));
        media->size= index+1;
    }

    /// Si no fue añadida una pelicula/serie en el año, se reserva espacio.
    /// Luego de la carga de todas las series y peliculas, podrian quedar posiciones vacias dentro del vector years.
    /// En este caso, se priorizo tiempo de ejecucion sobre memoria debido a que podria haber una gran carga de datos.
    if (media->years[index] == NULL){
        media->years[index]= calloc(1, sizeof(struct year));
        CHECK_MEM(media->years[index]);
        media->dim++;
    }
    int flag;

    /// Se añade la película/serie en sus generos correspondientes
    for ( int i=0; genre[i] != NULL; i++) {
        media->years[index]->genres = addContentByGenre_Rec(media->years[index]->genres, content, genre[i], title, &flag);
        if (flag == MEM_ERROR){
            return MEM_ERROR;
        }
    }

    /// Se actualiza la cantidad de películas/series añadidas. A pesar de que la misma película/serie se añadio a varios
    /// generos (si es que tiene mas de uno), se contabilizara una sola vez. Ademas, se actualiza la mejor serie/pelicula
    /// con su cantidad de votos.
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
 * @brief Función auxiliar para buscar un genero en la lista de generos
 *
 * @param first Puntero al primer genero de la lista
 * @param genre Genero a buscar en la lista
 * @return NULL si el genero no esta en la lista.
 * @return Puntero al nodo si esta en la lista.
 */
static TList searchGenre ( const TList first , const char * genre )
{
    int c;
    if ( first == NULL || (c = strcasecmp(first->genre,genre) ) > 0 )
        return NULL;       /// Como los generos están ordenados alfabéticamente, deja de buscar si se "paso"
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

    /**
     * Se crea un struct vacío, el cual la función retornará en caso de error
     *
     * @see mediaADT.h
     */
    TContent mostVotedContent  = {0};

    /// Se verifica si el año es valido
    if (isYearValid(media, year) != SUCCESS)
        return mostVotedContent;

    /// Se verifica de que tipo de contenido se desea obtener el más votado
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
 * @brief Funcion auxiliar de iterador que coloca al currentIndex en el siguiente año ocupado/valido.
 *
 * @param ADT creado para el manejo de peliculas/series.
 * @param fromIndex Indice desde donde se comienza a buscar el siguiente año ocupado.
 */
static void nextOcuppiedYear(const mediaADT media, const size_t fromIndex) {
    for (size_t i = fromIndex; i > 0; --i) {
        /// Si el año encontrado no está vacio, setea el indice del iterador a la posición del mismo
        if (media->years[i] != NULL) {
            media->currentIndex = i;
            return;
        }
    }
    media->currentIndex = media->size;
}

void toBeginYear(const mediaADT media){
    /// Se busca el siguiente año valido.
    nextOcuppiedYear(media, media->size - 1);
}

int hasNextYear(const mediaADT media){
    /// La iteración se realizará siempre y cuando se esté dentro del rango correspondiente
    return media->currentIndex < media->size;
}

unsigned short nextYear(const mediaADT media){

    /**
     * Si no quedan años validos por recorrer, la función devuelve RANGE_ERROR. El cual podrá ser manejado
     * desde el frontend.
     *
     * @see mediaADT.h
     */

    if (!hasNextYear(media)){
        return RANGE_ERROR;
    }

    /// Se obtiene el año actual
    unsigned short year = YEAR(media->currentIndex, media->minYear);

    ///Se busca el siguiente año valido desde la posicion actual
    nextOcuppiedYear(media, media->currentIndex - 1);
    return year;
}

int toBeginGenre (const mediaADT media , const unsigned short year )
{
    /// Se verifica que el año sea valido
    if (isYearValid(media,year) != SUCCESS )
        return INVALIDYEAR_ERROR;

    TYear aux = media->years[POS(year,media->minYear)];
    /// Se verifica que el año pedido tenga contenido
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

/**
 * @brief Funcion auxiliar que libera los recursos reservados por una lista de structs genre.
 *
 * @param genre Puntero a struct genre (TList) que contiene las peliculas/series añadidas dentro de un género en
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
 * @brief Funcion auxiliar que libera los recursos reservados por struct year y su puntero.
 *
 * @param year Puntero a struct year que contiene las peliculas/series añadidas a mediaADT con comienzo en un año
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
