#include "maninterface.h"
#include "general.h"
#include "requests.h"
#include "srcget.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>

int
give_q(int fdin, int fdout, int garb)
{
    int num;
    READCHECK(read(0, &num, sizeof num));
    int realsize, rest = req_q(fdin, fdout, num);// !!!!!!!
    ASSERT(write(1, &rest, sizeof rest) > 0);
    if (rest != ERR) {
        char buf[BUFLEN];
        while(rest > 0) {
            realsize = get_frag(fdin, buf, rest); // !!!!!
            ASSERT(write(1, buf, realsize) > 0);
            rest -= realsize;
        }
    }
    return OK;
FAIL:
    perror(MAN);
    return ERR;
}

int
give_all_q_num(int fdin, int fdout, int garb)
{
    int n = req_q_num(fdin, fdout);
    ASSERT(write(1, &n, sizeof n) > 0);
    return OK;
FAIL:
    perror(MAN);
    return ERR;
}

int
give_right_num(int semid, int shmid, int recnum)
{
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = -1;
    sop.sem_flg = SEM_UNDO;
    ASSERT(semop(semid, &sop, 1) != ERR);
    StRec *st = shmat(shmid, 0,  0);
    ASSERT(st != NULL);
    int n = st[recnum].rnum;
    ASSERT(shmdt(st) != ERR);
    sop.sem_flg = 0;
    sop.sem_op = 1;
    ASSERT(semop(semid, &sop, 1) != ERR);
    ASSERT(write(1, &n, sizeof n) > 0);
    return OK;
FAIL:
    perror(MAN);
    return ERR;
}

int
give_check(int fdin, int fdout, int garb)
{
    int num, len;
    READCHECK(read(0, &num, sizeof num));
    READCHECK(read(0, &len, sizeof len));
    int realsize, pos = 0, rest = len;
    char ans[BUFLEN + 1];
    while (rest > 0) {
        realsize = get_frag(0, ans + pos, rest);// !!!
        pos += realsize;
        rest -=realsize;
    }
    ans[pos] = '\0';
    num = req_check(fdin, fdout, ans, num);// !!!
    ASSERT(write(1, &num, sizeof num) > 0);
    return OK;
FAIL:
    perror(MAN);
    return ERR;
}

int
save(int semid, int shmid, int recnum) // maybe it's better to give number of record and attach shm every time
{
    int op;
    READCHECK(read(0, &op, sizeof op));
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = -1;
    sop.sem_flg = SEM_UNDO;
    ASSERT(semop(semid, &sop, 1) != ERR);
    StRec *st = shmat(shmid, 0,  0);
    ASSERT(st != NULL);
    switch (op) {
    case RIGHT:
        st[recnum].rnum++;
    case WRONG:
        st[recnum].qnum++;
        break;
    default:
        goto FAIL;
        break;
    }
    ASSERT(shmdt(st) != ERR);
    sop.sem_flg = 0;
    sop.sem_op = 1;
    ASSERT(semop(semid, &sop, 1) != ERR);
    op = OK;
    ASSERT(write(1, &op, sizeof op) > 0);
    return OK;
FAIL:
    perror(MAN);
    return ERR;
}

int
getclid(void)
{
    int id;
    READCHECK(read(0, &id, sizeof id));
    return id;
FAIL:
    perror(MAN);
    return ERR;
}

//client
int //returnes number of already asked questions or NOPLACE or ERR
getinfo(int fdin, int fdout, int id)
{
    ASSERT(write(fdout, &id, sizeof id) > 0);
    int n;
    READCHECK(read(fdin, &n, sizeof n));
    return n;
FAIL:
    perror(CLN);
    return ERR;
}

