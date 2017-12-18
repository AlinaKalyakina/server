#include "srcget.h"
#include <sys/sem.h>
#include "general.h"
#include <string.h>
#include <stdio.h>

static int 
hash (int x)
{
    return x % USERNUM;
}

int //ERR - no more place
find(int clid, StRec *rec, int semid, char flag, StRec *buf) //flag = 'i' -> insert, flag = 'c' -> copy record
{
    int key = hash(clid);
    int recnum = key;
    if (buf != NULL) {
        buf->id = 0;
    }
    struct sembuf sop = {.sem_num = recnum, .sem_op = -1, .sem_flg = SEM_UNDO};
    do {
        sop.sem_num = recnum;
        sop.sem_op = -1;
        semop(semid, &sop, 1);
        if (rec[recnum].id == clid || rec[recnum].id == 0) {
            switch (flag) {
                case 'i': 
                    rec[recnum].id = clid;
                    break;
                case 'c':
                    memcpy(buf, &(rec[recnum]), sizeof rec[recnum]);
            }
            break;
        } 
        sop.sem_op = 1;
        semop(semid, &sop, 1);
        recnum = (recnum + STEP) % USERNUM;
        if (recnum == key) {
            return NOPLACE;
        }
    } while (1);
    sop.sem_op = 1;
    semop(semid, &sop, 1);
    return recnum;
}

int
getdata (StRec *rec, int recnum, int semid, char flag)
{
    struct sembuf sop = {.sem_num = recnum, .sem_op = -1, .sem_flg = SEM_UNDO};
    int data;
    ASSERT(semop(semid, &sop, 1) != ERR);
    if (flag == 'r') { // right
        data = rec[recnum].rnum;
    } else {
        data = rec[recnum].qnum;
    }
    sop.sem_op = 1;
    ASSERT(semop(semid, &sop, 1) != ERR);
    return data;
FAIL:
    perror(NULL);
    return ERR;
}

int
mkchg(StRec *rec, int rec_semid, int recnum, int op) 
{
    struct sembuf sop = {.sem_num = recnum, .sem_op = -1, .sem_flg = SEM_UNDO};
    ASSERT(semop(rec_semid, &sop, 1) != ERR); //open rec
    switch (op) {
    case RIGHT:
        rec[recnum].rnum++;
    case WRONG:
        rec[recnum].qnum++;
        break;
    default:
        goto FAIL;
        break;
    }
    sop.sem_flg = SEM_UNDO;
    sop.sem_op = 1;
    ASSERT(semop(rec_semid, &sop, 1) != ERR); //close rec
    return OK;
FAIL:
    perror(MAN);
    return ERR;
}

int 
sumrec(StRec *rec, int semid, StRec *buf)
{
    buf->id = 0;
    buf->qnum = 0;
    buf->rnum = 0;
    struct sembuf sop = {.sem_num = 0, .sem_op = -1, .sem_flg = SEM_UNDO}; 
    for (int i = 0; i < USERNUM; i++) {
        sop.sem_num = i;
        sop.sem_op = -1;
        ASSERT(semop(semid, &sop, 1) != ERR);
        buf->id += rec[i].id != 0;
        buf->qnum += rec[i].qnum;
        buf->rnum += rec[i].rnum;
        sop.sem_op = 1;
        ASSERT(semop(semid, &sop, 1) != ERR);
    }
    return OK;
FAIL:
    perror(STAT);
    return ERR;
}

