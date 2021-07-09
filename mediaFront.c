#include "mediaADT.h"
#include <stdio.h>
#include <string.h>

#define GENRES_COUNT 0

int getDataFromFile(char * filePath);

TContent createContent(char * line, char * delim )
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

    newContent.genres = malloc(MAX*sizeof(char*));
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

int
main(int argc, char *argv[]) {


    return 0;
}