#include "mediaADT.h"
#include <stdio.h>
#include <string.h>

#define MIN_YEAR 1850
#define MAX_GENRES 15
#define BUFFER_SIZE 128

int getDataFromFile(mediaADT media, const char * filePath);

char ** createGenresVec(char ** vec, char * string);

TContent createContent(char * line, const char * delim );

void query1(mediaADT media, char * filePath);

void query2(mediaADT media, char * filePath);

/**
 * @brief Consulta las peliculas y series más votadas de cada año. Crea un archivo en el directorio especificado
 * y escribe el mismo con la información obtenida.
 *
 * @param media ADT creado para el manejo de películas/series.
 * @param filePath Directorio destino del archivo.
 */
void query3(mediaADT media, char * filePath);

int main(int argc, char *argv[]) {
    return 0;
}

int getDataFromFile(mediaADT media, const char * filePath){
    char buffer[BUFFER_SIZE];

    FILE *file = fopen(filePath, "r");
    fgets(buffer, BUFFER_SIZE, file);
    while (fgets(buffer, BUFFER_SIZE, file)){
        TContent new = createContent(buffer, ";");

        addContent(media, new, new.startYear, new.genres, new.numVotes, CONTENTTYPE_MOVIE);
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

    toBeginYear(media);
    while (hasNextYear(media)){
        unsigned short year= nextYear(media);
        size_t MYears= countContentByYear(media, year, CONTENTTYPE_MOVIE);
        size_t SYears= countContentByYear(media, year, CONTENTTYPE_SERIES);
        fprintf(file, "%u; %zu; %zu\n", year, MYears, SYears);
    }
    fclose(file);
}

void query2 ( mediaADT media , char * filePath )
{
    FILE * file = fopen(filePath,"w");

    toBeginYear(media);
    //Se itera por años validos para obtener los generos y cantidad peliculas por genero
    while (hasNextYear(media))
    {
        unsigned int year = nextYear(media);
        toBeginGenre(media,year);
        //Se itera por generos validos para obtener la cantidad de peliculas del genero actual
        while(hasNextGenre(media))
        {
            char * genre = nextGenre(media);
            size_t countOfMovies = countContentByGenre(media,year,genre,CONTENTTYPE_MOVIE);
            /*Se tiene año , genero y cantidad de peliculas para el par (año,genero). Se guarda la información en el
            *archivo y se continua la iteracion
             */
            fprintf(file,"%d;%s;%zu\n",year , genre , countOfMovies);
        }
    }

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

        ///Se obtiene la película y la serie más votada del año correspondiente.
        TContent movie = mostVoted(media, year, CONTENTTYPE_MOVIE);
        TContent series = mostVoted(media, year, CONTENTTYPE_SERIES);

        ///Se imprime en el archivo la información con el formato correspondiente.
        fprintf(file, "%d;%s;%.1f;%lu;%s;%.1f;%lu\n",year,
                movie.primaryTitle,movie.averageRating,movie.numVotes,
                series.primaryTitle,series.averageRating,series.numVotes);

    }

    ///Se finaliza la escritura del archivo.
    fclose(file);
}
