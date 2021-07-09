#include "mediaADT.h"
#include <stdio.h>
#include <string.h>

#define GENRES_COUNT 0

int getDataFromFile(char * filePath);

TContent createContent(char * line, char * delim);

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