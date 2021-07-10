#include "mediaADT.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>


#define MIN_YEAR 1850         /**< @def Minimo año que aceptara el TAD de pelicula/serie                    */
#define MAX_GENRES 15         /**< @def Maxima cantidad de generos que aceptara el TAD por pelicula/serie   */
#define BUFFER_SIZE 512       /** @def  Maxima cantidad de caracteres por linea que se obtendra del archivo */

#define INVALID_PATH (-1)       /** @def  Codigo definido para indicar error de un Path que es invalido */

/**< Macro que determina si S1 es del tipo pasado como parametro TYPE */
#define COMPARE_TYPES(S1,S2,TYPE) { if (strcasecmp((S1),(S2))==0) \
                                                  return TYPE;}

/**< Macro que determina si TO_CHECK es un cierto ERROR , en cuyo caso es enviado a la funcion errorManager
 * que recibe como parametros el ADT y el tipo de error ERROR_TYPE*/
#define ERROR_MANAGER(TO_CHECK,ERROR,ADT,ERROR_TYPE){ if ((TO_CHECK) == (ERROR)) \
                                                  errorManager((ERROR_TYPE),(ADT));}

/**< Macro que determina si E es un error FATAL que debe abortar la ejecucion del programa
 * , esto es , RANGE_ERROR , MEM_ERROR o INVALID_PATH */
#define IS_FATALERROR(E) ( (E) == RANGE_ERROR || (E) == MEM_ERROR || (E) == INVALID_PATH )

const char * UNDEFINED_SYMBOL = "\\N"; /**< String que se colocara en campos vacios durante la impresion */

int getDataFromFile(mediaADT media, const char * filePath);

/**
 * @brief Funcion que llena un vector de char * pasado como parametro con los generos especificados por parametro "string".
 * El ultimo elemento del vector tendra "NULL"
 *
 * @details El string pasado como parametro debe tener el caracter "," entre generos para poder separarlos en el vector.
 * Si el string tuviera "\\N", el primer elemento del vector tendra "Genero no especificado" y la funcion ignorara el
 * resto.
 *
 * @param vec Vector de char * que se llenara con los generos.
 * @param string String que contiene los generos.
 * @return El vector llenado que fue pasado como parametro.
 */
char ** createGenresVec(char ** vec, char * string);


/**
 * @brief Funcion que recibe una linea del csv y categoriza los datos en una estructura.
 *
 * @param line linea con datos a categorizar
 * @param delim delimitador de datos de la linea
 * @return TContent con los datos recopilados de la linea.
 */
TContent createContent(char * line, const char * delim );

/**
 * @brief Funcion que administra los errores de la implementacion
 *
 * @details Los errores categorizados como FATAL_ERRORS son aquellos que impiden la ejecucion del programa
 * por lo que la funcion imprime un mensaje de error correspondiente y aborta. Liberando previamente
 * los recursos utilizados por el ADT
 * Los errores que no entran en esta categoria, permiten continuar la ejecucion ignorando
 * la instancia en la que surjen , es decir , sin abortar.
 *
 * @param error  error que se desea administrar.
 * @param media  ADT creado para el manejo de peliculas/series.
 */
void errorManager ( int error , mediaADT media );

/**
 * @brief Funcion que determina si el contenido es una pelicula , serie u otro.
 *
 * @details Si no es una serie o una pelicula , devuelve CONTENTTYPE_ERROR indicando que no es
 * un contentType esperado.
 *
 * @param content es el contenido del cual se determinara su contentType.
 * @return CONTENTTYPE_MOVIE si es un pelicula.
 * @return CONTENTTYPE_SERIES si es una serie.
 * @return CONTENTTYPE_ERROR si no es serie o pelicula.
 */
contentType getContentType ( TContent content );

/**
 * @brief Funcion que consulta la cantidad de peliculas y series de cada año. Crea un archivo en el directorio especificado
 * y escribe el mismo con la informacion obtenida.
 *
 * @param media ADT creado para el manejo de peliculas/series.
 * @param filePath Directorio destino del archivo.
 */
void query1(mediaADT media, char * filePath);


/**
 * @brief Funcion que consulta la cantidad de peliculas y series de cada año y genero. Crea un archivo en el directorio
 * especificado y escribe en el mismo con la iformacion obtenida.
 *
 * @param media ADT creado para el manejo de peliculas/series.
 * @param filePath Directorio destino del archivo.
 */
void query2(mediaADT media, char * filePath);

/**
 * @brief Funcion que consulta las peliculas y series más votadas de cada año. Crea un archivo en el directorio especificado
 * y escribe el mismo con la informacion obtenida.
 *
 * @param media ADT creado para el manejo de películas/series.
 * @param filePath Directorio destino del archivo.
 */
void query3(mediaADT media, char * filePath);

int main(int argc, char *argv[]) {

    mediaADT media = newMediaADT(MIN_YEAR);

    getDataFromFile(media, argv[1]);

    query1(media, "query1.csv");
    query2(media, "query2.csv");
    query3(media, "query3.csv");

    freeMediaADT(media);

    return 0;
}

contentType getContentType ( TContent content )
{
    COMPARE_TYPES(content.titleType,"movie",CONTENTTYPE_MOVIE)
    COMPARE_TYPES(content.titleType,"tvSeries",CONTENTTYPE_SERIES)
    return (contentType)CONTENTTYPE_ERROR;
}

int getDataFromFile(mediaADT media, const char * filePath){

    /// Se crea el buffer donde se almacenará temporalmente la linea obtenida durante la copia
    char buffer[BUFFER_SIZE];

    ///Se abre el archivo en modo "read" para leer el contenido del mismo.
    FILE *file = fopen(filePath, "r");
    ERROR_MANAGER(file,NULL,media,INVALID_PATH) /// Se verifica que la operacion anterior se haya realizado correctamente

    /// Se obtiene la primera linea del archivo, la cual se espera que sea el encabezado por lo que es ignorada
    fgets(buffer, BUFFER_SIZE, file);
    int out;
    while (fgets(buffer, BUFFER_SIZE, file)){ /// Se obtienen las demas lineas del archivo

        /// Se utiliza un dato tipo TContent auxiliar, para almacenar la información y luego enviarla a la función addContent 
        TContent new = createContent(buffer, ";");

        /// Se obtiene el tipo de contenido, de lo obtenido anteriormente 
        contentType aux = getContentType(new);


        if ( (enum errorStates)aux != CONTENTTYPE_ERROR)
        {
            /// Mientras no hayan errores, se añade el contenido.
            out = addContent(media, new, new.startYear, new.genres, new.numVotes, aux);
            free(new.genres); /// Se libera el vector de generos auxiliar creado previamente en createContent
            if (out != 1 )
            {
                errorManager(out,media);
            }
        }
        else{
            free(new.genres);
            errorManager((enum errorStates)aux,media);
        }
    }

    ///Se finaliza la lectura del archivo.
    fclose(file);

    return 1;
}

char ** createGenresVec(char ** vec, char * string){
    char * token;
    token = strtok(string, ","); /// La funcion "tokeniza" el string para poder separarlo con el delimitador ","
    unsigned int i=0;
    if (strcmp(token, "\\N")==0){
        vec[i++]= "Género no identificado";
    }
    else {
        while (token != NULL) {
            vec[i++] = token;
            token = strtok(NULL, ",");
        }
    }
    vec[i]=NULL;
    return vec;
}

TContent createContent(char * line, const char * delim)
{
    TContent newContent;
    /// Se separan los datos de line mediante el delimitador.
    strcpy(newContent.titleType,strtok(line,delim));

    strcpy(newContent.primaryTitle,strtok(NULL,delim));
    ///El uso de atoi y atof es para convertir aquellos datos de la linea que deberian ser tratados como numeros
    newContent.startYear = atoi(strtok(NULL,delim));
    newContent.endYear = atoi(strtok(NULL,delim));

    /**Por el orden de los datos de line, en este punto se obtiene un string con los generos.
     * Para no perder lo almacenado por strtok al llamar a la funcion createGenresVec ( que tambien utiliza strtok )
     * se guarda el string del genero.Se continua creando los datos siguientes de la linea
     * y finalmente, se llama a createGenresVec con el valor almacaenado.
     */
    char * tokenAux = strtok(NULL,delim);

    newContent.averageRating = atof(strtok(NULL,delim));
    newContent.numVotes = atoi(strtok(NULL,delim));
    newContent.runtimeMinutes = atoi(strtok(NULL,delim));

    ///La itencion es crear un vector de strings donde cada uno es un genero de la pelicula, conformando un  vector de generos.
    newContent.genres = malloc(MAX_GENRES*sizeof(char*));
    createGenresVec(newContent.genres, tokenAux);

    return newContent;
}

void errorManager ( int  error , mediaADT media )
{
    switch (error) {
        case INVALID_PATH:
            printf("El path ingresado es invalido\n");
            break;
        case MEM_ERROR:
            printf("Error en asignacion de memoria \n");
            break;
        case RANGE_ERROR:
            printf("El iterador no puede avanzar\n");
            break;
        case CONTENTTYPE_ERROR:
            printf("Se ingreso un tipo de contenido invalido \n");
            break;
        default:
            break;
    }

    /**
     * En esta instancia se verifica si el error recibido es un FATAL ERROR. Ante la presencia de un error
     * de esta clasificacion el programa debe abortar la ejecucion ,  liberando previamente los recursos reservados.
     */
    if (IS_FATALERROR(error))
    {
        if ( error != INVALID_PATH )
            freeMediaADT(media);
        exit(EXIT_FAILURE);
    }
}

void query1(mediaADT media, char * filePath){
    ///Se crea el archivo, se abre en modo "write" para escribir sobre el mismo.
    FILE * file=fopen(filePath, "w");

    ///Se agrega el header correspondiente al archivo.
    fprintf(file, "year;films;series\n");

    toBeginYear(media);
    ///Se itera por años validos para obtener cantida de peliculas y series por año
    while (hasNextYear(media)){
        unsigned short year= nextYear(media);
        ERROR_MANAGER(year,RANGE_ERROR,media,RANGE_ERROR)
        size_t MYears= countContentByYear(media, year, CONTENTTYPE_MOVIE);
        size_t SYears= countContentByYear(media, year, CONTENTTYPE_SERIES);
        fprintf(file, "%u;%ld;%ld\n", year, MYears, SYears);
    }

    ///Se finaliza la escritura del archivo.
    fclose(file);
}

void query2 ( mediaADT media , char * filePath )
{
    ///Se crea el archivo, se abre en modo "write" para escribir sobre el mismo.
    FILE * file = fopen(filePath,"w");

    ///Se agrega el header correspondiende al archivo
    fprintf(file,"year;genre;films\n");

    toBeginYear(media);
    ///Se itera por años validos para obtener los generos y cantidad peliculas por genero
    while (hasNextYear(media))
    {
        unsigned int year = nextYear(media);
        ERROR_MANAGER(year,RANGE_ERROR,media,RANGE_ERROR)
        toBeginGenre(media,year);
        ///Se itera por generos validos para obtener la cantidad de peliculas del genero actual
        while(hasNextGenre(media))
        {
            char * genre = nextGenre(media);
            ERROR_MANAGER(genre,NULL,media,RANGE_ERROR)
            size_t countOfMovies = countContentByGenre(media,year,genre,CONTENTTYPE_MOVIE);

            /** Se tiene año , genero y cantidad de peliculas para el par (año,genero). Se guarda la información en el
              * archivo y se continua la iteracion. En caso de no haber peliculas no se imprime la linea.
              */
              if (countOfMovies != 0){
                  fprintf(file,"%d;%s;%ld\n",year , genre , countOfMovies);
              }
        }
    }

    ///Se finaliza la escritura del archivo.
    fclose(file);
}

void query3(mediaADT media, char * filePath){
    ///Se crea el archivo, se abre en modo "write" para escribir sobre el mismo.
    FILE * file = fopen(filePath, "w");

    ///En primer lugar, se añade el header al archivo.
    fprintf(file,"startYear;film;votesFilm;ratingFilm;serie;votesSerie;ratingSerie\n");

    ///Se inicia el iterador por años.
    toBeginYear(media);
    while (hasNextYear(media)){ ///< Mientras aún existan años validos, se seguirá escribiendo el archivo.
        unsigned short year = nextYear(media);
        ERROR_MANAGER(year,RANGE_ERROR,media,RANGE_ERROR)

        ///Se obtiene la película y la serie más votada del año correspondiente.
        TContent movie = mostVoted(media, year, CONTENTTYPE_MOVIE);
        TContent series = mostVoted(media, year, CONTENTTYPE_SERIES);

        /**
         * Se imprime en el archivo la información con el formato correspondiente.
         * En caso de que no se obtenga el titulo original de la pelicula/serie, se imprimira en su lugar
         * un simbolo especial indicando que no se obtuvo información sobre ese campo.
         */

        if (movie.primaryTitle[0] == '\0')
            fprintf(file, "%d;\\N;\\N;\\N;%s;%lu;%.1f\n",year, series.primaryTitle, series.numVotes, series.averageRating);

        else if (series.primaryTitle[0] == '\0')
            fprintf(file, "%d;%s;%lu;%.1f;\\N;\\N;\\N\n",year, movie.primaryTitle, movie.numVotes, movie.averageRating);
                 
        else
            fprintf(file, "%d;%s;%lu;%.1f;%s;%lu;%.1f\n",year,
                movie.primaryTitle, movie.numVotes, movie.averageRating,
                series.primaryTitle, series.numVotes, series.averageRating);

    }

    ///Se finaliza la escritura del archivo.
    fclose(file);
}
