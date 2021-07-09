#ifndef TPEFINAL_MEDIAADT_H
#define TPEFINAL_MEDIAADT_H

#include <stdlib.h>

/**
 * @brief Codigos de identificacion para los tipos de contenido.
 */
typedef enum {
    CONTENTTYPE_MOVIE = 1, /**< @enum Identificador contenido tipo Película */
    CONTENTTYPE_SERIES     /**< @enum Identificador contenido tipo Serie    */
} contentType;

/**
 * @brief Códigos para manejo de errores.
 */
enum errorStates {
    CONTENTTYPE_ERROR = 200, /**< @enum Tipo de contenido invalido            */
    MEM_ERROR,               /**< @enum Error en asignacion de memoria        */
    INVALIDYEAR_ERROR,       /**< @enum Año inexistente o fuera de rango      */
    NULLPOINTER_ERROR,       /**< @enum Acceso invalido                       */
    RANGE_ERROR              /**< @enum El iterador no puede avanzar          */
};

typedef struct mediaCDT * mediaADT;

/**
 * @brief Tamaño de bloque para alocar memoria.
 * @details Varia el tamaño dependiendo del tamaño del proyecto.
 *
 * @see MEM_BLOCK
 * */
#define MEM_BLOCK 512

#define MAX_TITLE_SIZE 256 /**< @def Tamaño maximo de titulo del contenido */
#define MAX_TYPE_SIZE 32   /**< @def Tamaño maximo de tipo del contenido   */
#define MAX_GENRE_SIZE 64  /**< @def Tamaño maximo de genero del contenido */

/**
 * @brief El usuario debera definir una estructura con información sobre los contenidos
 * que desea guardar.
 */
typedef struct content{
    char titleType[MAX_TYPE_SIZE];        /**< Tipo de contenido (película, serie)             */
    char primaryTitle[MAX_TITLE_SIZE];    /**< Titulo Original                                 */
    char ** genres;                       /**< Lista de generos correspondientes al contenido  */
    unsigned short startYear;             /**< El año de lanzamiento o comienzo de emisión     */
    unsigned short endYear;               /**< Si es una serie, el año de finalizacion         */
    unsigned short runtimeMinutes;        /**< Duracion en minutos                             */
    unsigned long numVotes;               /**< Cantidad de votos que obtuvo                    */
    float averageRating;                  /**< Numero decimal entre 0 y 10                     */
} TContent;

/**
 * @brief Funcion que crea un nuevo mediaADT para el manejo de peliculas/series.
 *
 * @param minYear Menor año de estreno para peliculas/series.
 * @return MediaADT creado.
 */
mediaADT newMediaADT(const size_t minYear);

/**
 * @brief Función que añade pelicula/serie a un media ADT.
 *
 * @details La lista de generos ingresados sera ordenada alfabeticamente, mientras que las peliculas y series dentro de los generos
 * no tendran ningun orden (seran metidas al final de los vectores a medida que se ingresan)
 *
 * @param media Media ADT creado para el manejo de películas/series.
 * @param content Contenido que sera añadido al ADT.
 * @param year Año en el que comenzo la pelicula/serie.
 * @param genre Vector de generos de la pelicula/serie.
 * @param numVotes Cantidad de votos de la pelicula/ serie.
 * @param contentType Indica si se añadira una pelicula o una serie. @see enum contentTypes
 * @return 1 si "content" se añadio exitosamente.
 * @return MEM_ERROR si se produjo un error de memoria.
 * @return INVALIDYEAR_ERROR si variable "year" es menor al año mínimo que acepta el TAD.
 * @return CONTENTTYPE_ERROR si variable "title" no corresponde ni a una serie ni a una pelicula.
 */
int addContent( mediaADT media , const TContent content ,const unsigned short year , char ** genre , const unsigned long numVotes , const contentType title);

/**
 * @brief Funcion para obtener la cantidad de peliculas/series para un año.
 *
 * @param Media ADT creado para el manejo de peliculas/series.
 * @param year Año del cual se solicita la cantidad de peliculas o de series.
 * @param CONTENTTYPE_ es el tipo de contenido del cual se desea conocer su cantidad para el año indicado.
 * Distinguiendo entre pelicula o serie.
 * @return 0 si el año es invalido o si no hay registro de peliculas/series para ese año.
 */
size_t countContentByYear(const mediaADT media, const unsigned short year, contentType CONTENTTYPE_);

/**
 * @brief Funcion para obtener la cantidad de peliculas/series para un genero en especifico de un año en especifico
 *
 * @param Media ADT creado para el manejo de peliculas/series.
 * @param year Año del cual se solicita la cantidad de peliculas o de series.
 * @param genre Genero del cual se solicita la cantidad de peliculas o de series para el año especificado.
 * @param CONTENTTYPE_ es el tipo de contenido del cual se desea conocer su cantidad para el año y genero indicado.
 * Distinguiendo entre pelicula o serie.
 * @return INVALIDYEAR_ERROR si el año ingresado es invalido.
 * @return NULLPOINTER_ERROR si no hay registro de peliculas para ese genero.
 * @return Cantidad de peliculas para el genero especificado.
 */
size_t countContentByGenre(const mediaADT media, const unsigned short year, const char * genre, contentType CONTENTTYPE_ );

/**
 * @brief Funcion que devuelve la pelicula/serie con mayor cantidad de votos del año.
 *
 * @param Media ADT creado para el manejo de películas/series.
 * @param year Año del cual se desea obtener la pelicula/serie.
 * @param CONTENTTYPE_ Tipo del contenido que se desea obtener
 * @return TContent con los datos correspondientes a la pelicula/serie
 */
TContent mostVoted(const mediaADT media, const unsigned short year, const contentType CONTENTTYPE_);

/*******************************************************************************
 *  @section Iteracion por año
 *  @brief Funciones de iteracion para que el usuario consulte años validos
 *  en orden cronologico descendente (Desde el mas actual hasta los anteriores).
 *
 *  @details Durante la iteracion, el usuario recibira años validos los cuales
 *  luego podra utilizar para obtener la información correspondiente con las
 *  demas funciones de este contrato.
 *  Si se desea comenzar la iteracion o no se puede avanzar en la misma,
 *  el usuario debera volver a ejecutar la funcion toBeginYear()
 *  para comenzar el recorrido desde el inicio.
 *
 *  @see toBeginYear()
 *  @see hasNextYear()
 *  @see nextYear()
********************************************************************************/

/**
 * @brief Funcion que inicializa el iterador en el año más actual. Si no hay años validos, la funcion no hace nada.
 *
 * @param media ADT creado para el manejo de películas/series.
 */
void toBeginYear(const mediaADT media);

/**
 * @brief Funcion que consulta si existe un año valido siguiente.
 *
 * @param Media ADT creado para el manejo de peliculas/series.
 * @return 1 si existe un año valido siguiente del iterador.
 */
int hasNextYear(const mediaADT media);

/**
 * @brief Funcion que pasa al siguiente año en el iterador.
 *
 * @details Se debe utilizar en combinacion de la funcion hasNextYear . En caso
 * de que no se verifique si hay un año siguiente valido en el iterador y se llame
 * a esta funcion, retornara RANGE_ERROR indicando que no hay un año valido.
 *
 * @param Media ADT creado para el manejo de peliculas/series.
 * @return Año valido siguiente del iterador.
 * @return RANGE_ERROR Si no es un año valido siguiente del iterador
 */
unsigned short nextYear(const mediaADT media);

/**
 * @brief Funcion que libera los recursos reservados por mediaADT.
 *
 * @param Media ADT creado para el manejo de peliculas/series.
 */
void freeMediaADT(mediaADT media);

/*******************************************************************************
 *  @section Iteracion por genero
 *  @brief Funciones de iteracion para que el usuario consulte generos validos para un año,
 *  ordenados alfabeticamente.
 *
 *  @details Durante la iteracion, el usuario recibira generos validos los cuales
 *  luego podrá utilizar para obtener la información correspondiente con las
 *  demas funciones de este contrato.
 *  Si se desea comenzar la iteracion o no se puede avanzar en la misma,
 *  el usuario deberá volver a ejecutar la funcion toBeginGenre()
 *  para comenzar el recorrido desde el inicio , indicando en ella el año en que
 *  se desea iterar por genero.
 *
 *  @see toBeginGenre()
 *  @see hasNextGenre()
 *  @see nextGenre()
********************************************************************************/

/**
 * @brief Funcion que inicializa el iterador en el primer genero en orden alfabetico.
 *
 * @param Media ADT creado para el manejo de peliculas/series.
 * @param year Año para el cual se desea iterar los generos validos de series y peliculas.
 * @return INVALIDYEAR_ERROR Si el año pasado como argumento es invalido.
 * @return 1 Si el iterador fue seteado correctamente.
 */
int toBeginGenre (const mediaADT media , const unsigned short year );

/**
 * @brief Funcion que consulta si existe un género válido siguiente.
 *
 * @param Media ADT creado para el manejo de películas/series.
 * @return 1 si existe un género válido siguiente del iterador.
 */
int hasNextGenre ( const mediaADT media );

/**
 * @brief Funcion que pasa al siguiente genero en el iterador
 *
 * @details Se debe utilizar en combinación de la funcion hasNextGenre . En caso de que no
 * se verifique si hay un genero siguiente valido en el iterador y se llame a esta funcion,
 * retornara NULL indicando que no hay un genero valido.
 *
 * @param Media ADT creado para el manejo de películas/series.
 * @return Genero valido siguiente del iterador.
 * @return NULL Si no hay un genero valido en el iterador.
 */
char * nextGenre ( const mediaADT media );

#endif //TPEFINAL_MEDIAADT_H
