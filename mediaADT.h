#ifndef TPEFINAL_MEDIAADT_H
#define TPEFINAL_MEDIAADT_H

typedef struct mediaCDT * mediaADT;

typedef struct media{
    char * titleType;
    char * primaryTitle;
    char ** genres;
    unsigned short startYear;
    unsigned short endYear;
    unsigned short runtimeMinutes;
    unsigned long numVotes;
    float averageRating;
} TMedia;

#endif //TPEFINAL_MEDIAADT_H
