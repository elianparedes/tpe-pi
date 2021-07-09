#ifndef TPEFINAL_MEDIAADT_H
#define TPEFINAL_MEDIAADT_H

#include <stdlib.h>

/**
 * @brief Códigos de identificación para los tipos de contenido.
 */
typedef enum {
    CONTENTTYPE_MOVIE = 1, /**< @enum Identificador contenido tipo Película */
    CONTENTTYPE_SERIES     /**< @enum Identificador contenido tipo Serie    */
} contentType;

/**
 * @brief Códigos para manejo de errores.
 */
enum errorStates {
    CONTENTTYPE_ERROR = 200, /**< @enum Tipo de contenido inválido            */
    MEM_ERROR,               /**< @enum Error en asignación de memoria        */
    INVALIDYEAR_ERROR,       /**< @enum Año inexistente o fuera de rango      */
    NULLPOINTER_ERROR,       /**< @enum Acceso inválido                       */
    RANGE_ERROR              /**< @enum El iterador no puede avanzar          */
};

typedef struct mediaCDT * mediaADT;

/**
 * @brief El usuario deberá definir una estructura con información sobre los contenidos
 * que desea guardar.
 */
typedef struct content{
    char * titleType;               /**< Tipo de contenido (película, serie)             */
    char * primaryTitle;            /**< Título Original                                 */
    char ** genres;                 /**< Lista de generos correspondientes al contenido  */
    unsigned short startYear;       /**< El año de lanzamiento o comienzo de emisión     */
    unsigned short endYear;         /**< Si es una serie, el año de finalización         */
    unsigned short runtimeMinutes;  /**< Duración en minutos                             */
    unsigned long numVotes;         /**< Cantidad de votos que obtuvo                    */
    float averageRating;            /**< Numero decimal entre 0 y 10                     */
} TContent;

/**
 * @brief Crea un nuevo mediaADT para el manejo de peliculas/series.
 *
 * @param minYear el menor año de estreno para peliculas/series
 * @return
 */
mediaADT newMediaADT(const size_t minYear);

/**
 * @brief Función que añade película/serie a un media ADT.
 *
 * @param media Media ADT creado para el manejo de películas/series.
 * @param content Contenido que será añadido al ADT.
 * @param year Año en el que comenzó la película/serie.
 * @param genre Vector de generos de la película/serie.
 * @param numVotes Cantidad de votos de la película/ serie.
 * @param contentType Indica si se añadirá una película o una serie. @see enum contentTypes
 * @return 1 si "content" se añadió exitosamente, MEM_ERROR si se produjo un error de memoria, INVALIDYEAR_ERROR si variable
 * "year" es menor al año mínimo que acepta el TAD o CONTENTTYPE_ERROR si variable "title" no corresponde ni a una serie
 * ni a una película.
 *
 */
int addContent( mediaADT media , const TContent content ,const unsigned short year , const char ** genre , const unsigned long numVotes , const contentType title);

/**
 * @brief Función para obtener la cantidad de peliculas/series para un año.
 *
 * @param media ADT creado para el manejo de películas/series.
 * @param year es el año del cual se solicita la cantidad de peliculas o de series.
 * @param MEDIATYPE_ es el tipo de contenido del cual se desea conocer su cantidad para el año indicado.
 *        Distinguiendo entre pelicula o serie.
 * @return 0 si el año es invalido o si no hay registro de peliculas/series para ese año.
 */
size_t countContentByYear(const mediaADT media, const unsigned short year, contentType CONTENTTYPE_);

/**
 * @brief Función para obtener la cantidad de peliculas/series para un genero en especifico de un año en especifico
 *
 * @param media ADT creado para el manejo de películas/series.
 * @param year es el año del cual se solicita la cantidad de peliculas o de series.
 * @param genre es el genero del cual se solicita la cantidad de peliculas o de series para el año especificado.
 * @param MEDIATYPE_ es el tipo de contenido del cual se desea conocer su cantidad para el año y genero indicado.
 *        Distinguiendo entre pelicula o serie.
 * @return 0 si el año o el genero son invalido o si no hay registro de peliculas/series para ese género.
 */
size_t countContentByGenre(const mediaADT media, const unsigned short year, const char * genre, contentType CONTENTTYPE_ );

/**
 * @brief Función que devuelve la pelicula/serie con mayor cantidad de votos del año.
 *
 * @param media ADT creado para el manejo de películas/series.
 * @param year Año del cual se desea obtener la película/serie.
 * @param CONTENTTYPE_ Tipo del contenido que se desea obtener
 * @return TContent con los datos correspondientes a la película/serie
 */
TContent mostVoted(const mediaADT media, const unsigned short year, const contentType CONTENTTYPE_);

/*******************************************************************************
 *  @section Iteración por año
 *  @brief Funciones de iteración para que el usuario consulte años válidos
 *  en orden cronológico descendente (Desde el más actual hasta los anteriores).
 *
 *  @details Durante la iteración, el usuario recibirá años validos los cuales
 *  luego podrá utilizar para obtener la información correspondiente con las
 *  demas funciones de este contrato.
 *  Si se desea comenzar la iteración o no se puede avanzar en la misma,
 *  el usuario deberá volver a ejecutar la funcion toBeginYear()
 *  para comenzar el recorrido desde el inicio.
 *
 *  @see toBeginYear()
 *  @see hasNextYear()
 *  @see nextYear()
********************************************************************************/

/**
 * @brief Inicializa el iterador en el año más actual. Si no hay años válidos, la función no hace nada.
 *
 * @param media ADT creado para el manejo de películas/series.
 */
void toBeginYear(const mediaADT media);

/**
 * @brief Consulta si existe un año válido siguiente.
 *
 * @param media ADT creado para el manejo de películas/series.
 * @return 1 si existe un año valido siguiente del iterador.
 */
int hasNextYear(const mediaADT media);

/**
 * @brief Pasa al siguiente año en el iterador
 *
 * @details Se debe utilizar en combinacion de la funcion hasNextYear . En caso
 * de que no se verifique si hay un año siguiente valido en el iterador y se llame
 * a esta funcion, retornara RANGE_ERROR indicando que no hay un año valido.
 *
 * @param media ADT creado para el manejo de películas/series.
 * @return Año válido siguiente del iterador.
 * @return RANGE_ERROR Si no es un año valido siguiente del iterador
 */
unsigned short nextYear(const mediaADT media);

/**
 * @brief Función que libera los recursos reservados por mediaADT.
 *
 * @param Media ADT creado para el manejo de películas/series.
 */
void freeMediaADT(mediaADT media);

/*******************************************************************************
 *  @section Iteración por género
 *  @brief Funciones de iteración para que el usuario consulte generos válidos para un año,
 *  ordenados alfabeticamente.
 *
 *  @details Durante la iteración, el usuario recibirá generos validos los cuales
 *  luego podrá utilizar para obtener la información correspondiente con las
 *  demas funciones de este contrato.
 *  Si se desea comenzar la iteración o no se puede avanzar en la misma,
 *  el usuario deberá volver a ejecutar la funcion toBeginGenre()
 *  para comenzar el recorrido desde el inicio , indicando en ella el año en que
 *  se deasea iterar por genero.
 *
 *  @see toBeginGenre()
 *  @see hasNextGenre()
 *  @see nextGenre()
********************************************************************************/

/**
 * @brief Inicializa el iterador en el primer genero en orden alfabetico.
 *
 * @param media ADT creado para el manejo de películas/series.
 * @param year año para el cual se desea iterar los generos validos de series y peliculas
 * @return INVALIDYEAR_ERROR Si el año pasado como argumento es invalido
 * @return 1 Si el iterador fue seteado correctamente
 */
int toBeginGenre (const mediaADT media , const unsigned short year );

/**
 * @brief Consulta si existe un genero valido siguiente.
 *
 * @param media ADT creado para el manejo de películas/series.
 * @return 1 si existe un genero valido siguiente del iterador.
 */
int hasNextGenre ( const mediaADT media );

/**
 * @brief Pasa al siguiente genero en el iterador
 *
 * @details Se debe utilizar en combinacion de la funcion hasNextGenre . En caso de que no
 * se verifique si hay un genero siguiente valido en el iterador y se llame a esta funcion ,
 *  etornara NULL indicando que no hay un genero valido.
 *
 * @param media ADT creado para el manejo de películas/series.
 * @return genero valido siguiente del iterador.
 * @return NULL Si no hay un genero valido en el iterador.
 */
char * nextGenre ( const mediaADT media );

#endif //TPEFINAL_MEDIAADT_H
