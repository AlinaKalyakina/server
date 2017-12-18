#ifndef STATINTERFACE_H_INCLUDED
#define STATINTERFACE_H_INCLUDED
#include "srcget.h"
#include <stdio.h>
enum format{L_ALL = 80, L_NAME = 50, L_STATUS = 8, L_MARK = 5, L_QNUM = 10};

int shortstat(StRec *reclist, int rec_semid, int onl_semid, FILE* f, FILE *garbage);
int longstat(StRec *reclist, int rec_semid, int onl_semid, FILE* f, FILE *idf);
typedef struct {
    char cmd;
    int (*func) (StRec *reclist, int rec_semid, int onl_semid, FILE* f, FILE *idf);
} StatCmd;
#endif
