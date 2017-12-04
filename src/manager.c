#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "srcget.h"
#include "general.h"
#include "maninterface.h"
#include "requests.h"

MCmd instr[] = {{QUE, give_q},
                {CHECK, give_check},
                {ALLQNUM, give_all_q_num},
                {RQNUM, give_right_num},
                {SAVERES, save}};

int semid, shmid;

int //ERR - no more place
findrec(int chid, StRec *begin)
{
    int nrec = begin[0].qnum;
    int i = 1;
    while(i <= nrec && begin[i].id != chid) {
        i++;
    }
    if (i == SHMSIZE) {
        return ERR;
    }
    return i;
}

int //recnum or ERR if
checkcl(int fdin, int fdout)
{
    int id = getclid();
    int qnum = req_q_num(fdin, fdout);
    if (qnum == ERR) {
        exit(0);
    }
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = -1;
    sop.sem_flg = SEM_UNDO;
    ASSERT(semop(semid, &sop, 1) != ERR);
    StRec *st = shmat(shmid, NULL, 0);
    ASSERT(st != NULL);
    int recnum;
    if ((recnum = findrec(id, st)) == ERR) {
        recnum = NOPLACE;
        write(1, &recnum, sizeof recnum);
        exit(0);
    }
    if (recnum > st[0].qnum) {
        st[0].qnum++;
        st[recnum].id = id;
        st[recnum].qnum = 0;
        st[recnum].rnum = 0;
    }
    int permit = st[recnum].qnum;
    if (permit >= qnum) {
        permit = WRONG;
        recnum = WRONG;
    } else {
        sop.sem_num = 2;
        sop.sem_op = 1;
        sop.sem_flg = SEM_UNDO;
        ASSERT(semop(semid, &sop, 1) != ERR);
    }
    ASSERT(shmdt(st) != ERR);
    sop.sem_num = 0;
    sop.sem_op = 1;
    sop.sem_flg = 0;
    ASSERT(semop(semid, &sop, 1) != ERR);
    ASSERT(write(1, &permit, sizeof permit) > 0); //number of already asked question or WRONG
    return recnum;
FAIL:
    perror(MAN);
    exit(0);
}

int
getsrc(int fdin, int fdout)
{
    char topic[BUFLEN + 1];
    if (req_top(topic, fdin, fdout) == ERR) {
        exit(0);
    }
    char filename[MAXNAME + 1];
    getipcname(topic, filename);
    key_t key = ftok(filename, 'a');
    ASSERT(key != ERR);
    ASSERT((shmid = shmget(key, SHMSIZE, 0)) != ERR);
    ASSERT((semid = semget(key, 1, 0)) != ERR);
    return OK;
FAIL:
    perror(MAN);
    return ERR;
}

int main(int argc, char *argv[])    
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    int fdin[2], fdout[2];
    pid_t testpid = testbirth(fdin, fdout, argv[1]);
    getsrc(fdin[0], fdout[1]);
    int recnum;
    if((recnum = checkcl(fdin[0], fdout[1])) == WRONG) {
        exit(0);
    }
    int cmd;
    do {
        READCHECK(read(0, &cmd, sizeof cmd));
        unsigned int i = 0;
        while (cmd != instr[i].name && (++i) < sizeof(instr)/sizeof(instr[i]));
        if (i < sizeof(instr)/sizeof(instr[0])) {
            int arg1, arg2, arg3;
            if (cmd == SAVERES || cmd == RQNUM){
                arg1 = semid;
                arg2 = shmid;
                arg3 = recnum;
            } else {
                arg1 = fdin[0];
                arg2 = fdout[1];
                arg3 = recnum;
            }
            if (instr[i].func(arg1, arg2, arg3) == ERR) {
                goto TERM;
            }
        } else {
            fputs("Requested to manager instruction doesn't exist", stderr);
            exit(0);
        }
    } while (1);
TERM:
    close(fdin[0]);
    close(fdout[1]);
    wait(NULL);
    return OK;
FAIL:
    perror(MAN);
    kill(testpid, SIGINT);
    close(fdin[0]);
    close(fdout[1]);
    return ERR;
}
