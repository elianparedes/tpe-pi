#include "mediaADT.h"
#include <stdio.h>
#include <string.h>
#define MAX_GENRES 15
#define BUFFER_SIZE 128
#define COMPARE_TYPES(S1,S2,TYPE) { if (strcasecmp(S1,S2)==0) \
                                                  return TYPE;}

int getDataFromFile(mediaADT media, const char * filePath);
char ** createGenresVec(char ** vec, char * string);
TContent createContent(char * line, const char * delim );
void query1(mediaADT media, char * filePath);
void query2 ( mediaADT media , char * filePath );

int main(int argc, char *argv[]) {
    return 0;
}

/**
 * @brief Determina si el contenido es una pelicula , serie u otro.
 *
 * @details Si no es una serie o una pelicula , devuelve CONTENTTYPE_ERROR indicando que no es
 * un contentType esperado
 *
 * @param content es el contenido del cual se determinara su contentType
 * @return CONTENTTYPE_MOVIE si es un pelicula
 * @return CONTENTTYPE_SERIES si es una serie
 * @return CONTENTTYPE_ERROR si no es serie o pelicula
 */
contentType getContentType ( TContent content )
{
    COMPARE_TYPES(content.titleType,"movie",CONTENTTYPE_MOVIE)
    COMPARE_TYPES(content.titleType,"tvSeries",CONTENTTYPE_SERIES)
    return (contentType)CONTENTTYPE_ERROR;
}

int getDataFromFile(mediaADT media, const char * filePath){

    char buffer[BUFFER_SIZE];

    FILE *file = fopen(filePath, "r");
    fgets(buffer, BUFFER_SIZE, file);
    while (fgets(buffer, BUFFER_SIZE, file)){
        TContent new = createContent(buffer, ";");
        contentType aux = getContentType(new);
        if ( aux == CONTENTTYPE_ERROR )
            //TENER EL CUIDADO DE SI HAY QUE LIBERAR O NO
            exit(EXIT_FAILURE);
        addContent(media, new, new.startYear, new.genres, new.numVotes, aux);
        free(new.genres);
    }
    fclose(file);

    return 1;
}

char ** createGenresVec(char ** vec, char * string){
    char * token;
    token = strtok(string, ",");
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

TContent createContent(char * line, const char * delim )
{
    TContent newContent ;
    newContent.titleType = strtok(line,delim);

    char * title = strtok(NULL,delim);
    newContent.primaryTitle = malloc(strlen(title) + 1);
    strcpy(newContent.primaryTitle, title);

    newContent.startYear = atoi(strtok(NULL,delim));
    newContent.endYear = atoi(strtok(NULL,delim));

    char * tokenAux = strtok(NULL,delim);

    newContent.averageRating = atof(strtok(NULL,delim));
    newContent.numVotes = atoi(strtok(NULL,delim));
    newContent.runtimeMinutes = atoi(strtok(NULL,delim));

    newContent.genres = malloc(MAX_GENRES*sizeof(char*));
    createGenresVec(newContent.genres, tokenAux);
    return newContent;
}

void query1(mediaADT media, char * filePath){
    FILE * file=fopen(filePath, "w");

    fprintf(file,"%s;%s;%s\n" , "year" ,"films", "series");

    toBeginYear(media);
    fprintf(file, "year;genre;films\n");
    while (hasNextYear(media)){
        unsigned short year= nextYear(media);
        size_t MYears= countContentByYear(media, year, CONTENTTYPE_MOVIE);
        size_t SYears= countContentByYear(media, year, CONTENTTYPE_SERIES);
        fprintf(file, "%u;%zu;%zu\n", year, MYears, SYears);
    }
    fclose(file);
}

void query2 ( mediaADT media , char * filePath )
{
    FILE * file = fopen(filePath,"w");

    ///Se agrega el header correspondiende al archivo
    fprintf(file,"%s;%s;%s\n" , "year" ,"genre", "films");

    toBeginYear(media);
    ///Se itera por años validos para obtener los generos y cantidad peliculas por genero
    while (hasNextYear(media))
    {
        unsigned int year = nextYear(media);
        toBeginGenre(media,year);
        ///Se itera por generos validos para obtener la cantidad de peliculas del genero actual
        while(hasNextGenre(media))
        {
            char * genre = nextGenre(media);
            size_t countOfMovies = countContentByGenre(media,year,genre,CONTENTTYPE_MOVIE);
            /**Se tiene año , genero y cantidad de peliculas para el par (año,genero). Se guarda la información en el
            *archivo y se continua la iteracion
             */
            fprintf(file,"%d;%s;%zu\n",year , genre , countOfMovies);
        }
    }

    fclose(file);
}
