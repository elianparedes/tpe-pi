#include "mediaADT.h"
#include <stdio.h>

#define BUFFER_SIZE 128

FILE * createFile(char * filePath);

int getDataFromFile(mediaADT media, const char * filePath);

TContent createContent(char * line, const char * delim);

int
main(int argc, char *argv[]) {

    return 0;
}

int getDataFromFile(mediaADT media, const char * filePath){

    char buffer[BUFFER_SIZE];

    FILE *file = fopen(filePath, "r");
    /** while (fgets(buffer, BUFFER_SIZE, file)){

    } */
    fclose(file);

    return 1;
}

FILE * createFile(char * filePath){
    FILE * newFile = fopen(filePath, "w");
    return newFile;
}


