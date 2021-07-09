#include "mediaADT.h"
#include <stdio.h>
#include <string.h>
#define MAX_GENRES 15
#define GENRES_COUNT 0

int getDataFromFile(char * filePath);

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

TContent createContent(char * line, char * delim )
{
    TContent newContent ;
    newContent.titleType = strtok(line,delim);
    newContent.primaryTitle  = strtok(NULL,delim);
    newContent.startYear = atoi(strtok(NULL,delim));
    newContent.endYear = atoi(strtok(NULL,delim));
    char * tokenAux = strtok(NULL,delim);
    newContent.genres = malloc(MAX_GENRES*sizeof(char*));
    createGenresVec(newContent.genres, tokenAux);

    newContent.averageRating = atof(strtok(tokenAux,delim));
    newContent.numVotes = atoi(strtok(NULL,delim));
    newContent.runtimeMinutes = atoi(strtok(NULL,delim));



    return newContent;

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
            /*Se tiene año , genero y cantidad de peliculas para el par (año,genero). Se guarda la iformacion en el
            *archivo y se continua la iteracion
             */
            fprintf(file,"%d;%s;%llu",year , genre , countOfMovies);
        }
    }

    fclose(file);
}



int
main(int argc, char *argv[]) {


    return 0;
}
