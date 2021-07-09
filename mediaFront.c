#include "mediaADT.h"
#include <stdio.h>
#include <string.h>

#define MAX_GENRES 20
#define BUFFER_SIZE 128

FILE * createFile(char * filePath);

int getDataFromFile(mediaADT media, const char * filePath);

TContent createContent(char * line, const char * delim);

char ** createGenresVec(char ** vec, char * string);

int
main(int argc, char *argv[]) {

    return 0;
}

TContent createContent(char * line, const char * delim )
{
    TContent newContent ;
    newContent.titleType = strtok(line,delim);
    newContent.primaryTitle  = strtok(NULL,delim);
    newContent.startYear = atoi(strtok(NULL,delim));
    newContent.endYear = atoi(strtok(NULL,delim));

    char * genres = strtok(NULL,delim);

    newContent.averageRating = atof(strtok(NULL,delim));
    newContent.numVotes = atoi(strtok(NULL,delim));
    newContent.runtimeMinutes = atoi(strtok(NULL,delim));

    newContent.genres = malloc(MAX_GENRES * sizeof(char*));
    createGenresVec(newContent.genres, genres);

    return newContent;
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

int getDataFromFile(mediaADT media, const char * filePath){

    char buffer[BUFFER_SIZE];

    FILE *file = fopen(filePath, "r");
    while (fgets(buffer, BUFFER_SIZE, file)){
        TContent new = createContent(buffer, ";");
        addContent(media, new, new.startYear, new.genres, new.numVotes, CONTENTTYPE_MOVIE);
        free(new.genres);
    }
    fclose(file);

    return 1;
}

FILE * createFile(char * filePath){
    FILE * newFile = fopen(filePath, "w");
    return newFile;
}


