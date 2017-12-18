#include "statinterface.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "general.h"
#include "srcget.h"
#include <stdlib.h>
#include <string.h>
#include "dataop.h"

static char on[] = "online";
static char off[] = "offline";
static char notdone[] = "hasn't done test\n";


int 
onlnum(int semid) 
{
    int n = 0;
    for (int i = 0; i < USERNUM; i++) {
        switch (semctl(semid, i, GETVAL, 0)) {
            case -1: 
                goto FAIL;
            case 0:
                n++;
        }
    }
    return n;
FAIL:
    perror(STAT);
    return ERR;
} 

int
shortstat(StRec *reclist, int rec_semid, int onl_semid, FILE* f, FILE *garbage)
{
    StRec sum;
    int n;
    if (sumrec(reclist, rec_semid, &sum) == ERR || (n = onlnum(onl_semid)) == ERR) {
        return ERR;
    }
    ASSERT(fprintf(f, "Number of records = %d\nNumber of online users = %d\n", sum.id, n) > 0);
    if (sum.id == 0) {
        fputs("No data for average numbers\n", f);   
    } else {
        ASSERT(fprintf(f, "Average number of answered questions = %lf(test contains %d questions)\n"\
            "Average number of right answers = %lf\n",((double)sum.qnum)/sum.id, reclist[USERNUM].qnum, \
                ((double)sum.rnum)/sum.id) > 0);
    }
    return OK;
FAIL:
    perror(STAT);
    return ERR;
}

static int
getname(char *name, FILE *f)
{
    ASSERT(fgets(name, L_NAME + 2, f) != NULL); //+1 for \n
    if (feof(f)) {
        return WRONG;
    }
    int len = strlen(name);
    char buf[BUFLEN] = "\0";
    if (name[len - 1] != '\n') { //too long name
        while (buf[strlen(buf) - 1] != '\n') {
            fgets(buf, BUFLEN, f);
        }
        name[L_NAME - 3] = '\0';
        strcat(name, "..."); //for beauty
    }
    name[len - 1] = '\0';
    return OK;
FAIL:
    if (!feof(f)) {
        perror(NULL);
    }
    return ERR;
}

static int //WRONG = no p
getid(stid_t* id, FILE *f)
{
    char buf[BUFLEN];
    ASSERT(fgets(buf, BUFLEN, f) != NULL);
    sscanf(buf, "%d", id);
    return OK;
FAIL:
    if(!feof(f)) {
        perror(NULL);
        return ERR;
    }
    return WRONG;
}

static int
getqnum(StRec *rec)
{
    int n = rec[USERNUM].qnum;
    return n;
}


int
longstat(StRec *records, int rec_semid, int onl_semid, FILE* statf, FILE *idf)
{
    ASSERT(fprintf(statf, "%-*s%-*s%-*s%-*s%-*s\n", L_NAME, "NAME", L_STATUS, "STATUS", L_MARK, "MARK",\
            L_QNUM, "ANSWERED", L_QNUM, "RIGHT") > 0);
    int allqnum = getqnum(records);
    if (allqnum == ERR) {
        return ERR;
    }
    int strnum = 0;
    while (!feof(idf)) {
        strnum++;
        char name[L_NAME + 2];
        stid_t id;
        if (getname(name, idf) != OK) { //err or no name more
            break;
        }
        if (getid(&id, idf) != OK) {
            break;
        }
        strnum++;
        ASSERT(fprintf(statf, "%-*s",L_NAME, name) > 0);
        if (id <= 0) {
            printf("\nInvalid id in %d line\n", strnum);
            continue;
        }
        StRec rec;
        int recnum = find(id, records, rec_semid, 'c', &rec);
        if (rec.id == 0) {// empty record
            fputs(notdone, statf);
            continue;
        } 
        char *status;
        if (semctl(onl_semid, recnum, GETVAL, 0) == 0) {
            status = on;
        } else {
            status = off;
        }
        if (allqnum == rec.qnum) { //all questions were answered
            ASSERT(fprintf(statf, "%-*s%-*d%-*d%-*d\n", L_STATUS, status, L_MARK, (rec.rnum*5)/allqnum,\
                L_QNUM, rec.qnum, L_QNUM, rec.rnum) > 0);
        } else { // not all
            ASSERT(fprintf(statf, "%-*s%-*s%-*d%-*d\n", L_STATUS, status, L_MARK, "-",\
                L_QNUM, rec.qnum, L_QNUM, rec.rnum) > 0);
        }
        fflush(statf);
    }
    return OK;
FAIL:
    perror(NULL);
    return ERR;
}
