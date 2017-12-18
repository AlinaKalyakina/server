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
#include "dataop.h"

MCmd instr[] = {{QUE, give_q},
                {CHECK, give_check},
                {ALLQNUM, give_all_q_num},
                {RQNUM, give_right_num},
                {SAVERES, save}};

int onl_semid, rec_semid, rec_shmid;

void handler(int sig)
{
    exit(0);
}

int //recnum or ERR if
checkcl(int fdin, int fdout)
{
    int id = getclid();
    //searching for id
    StRec *rec = shmat(rec_shmid, NULL, 0);
    int recnum = find(id, rec, rec_semid, 'i', NULL);
    if (recnum == NOPLACE) { //no more place in statistic
        write(1, &recnum, sizeof recnum);
        exit(0);
    }
    struct sembuf sop = {.sem_num = recnum, .sem_op = -1, .sem_flg = SEM_UNDO};
    int ans;
    if (semctl(onl_semid, recnum, GETVAL, 0) == 0) { // client with such id already exists
        ans = WRONG;
    } else {
        semop(onl_semid, &sop, 1);
        if ((ans = getdata(rec, recnum, rec_semid, 'a')) == ERR) {
            exit(0);
        }
    }
    ASSERT(write(1, &ans, sizeof ans) > 0); 
    return recnum;
FAIL:
    perror(MAN);
    exit(0);
}

int
getipcsrc(int fdin, int fdout)
{
    char topic[BUFLEN + 1];
    if (req_top(topic, fdin, fdout) == ERR) {
        exit(0);
    }
    char filename[MAXNAME + 1];
    getipcname(topic, filename);
    key_t key;
    ASSERT((key = ftok(filename, 'a')) != ERR);
    ASSERT((onl_semid = semget(key, USERNUM + 1, 0)) != ERR);
    ASSERT((key = ftok(filename, 'b')) != ERR);
    ASSERT((rec_shmid = shmget(key, (USERNUM + 1) * sizeof(StRec), 0)) != ERR);
    ASSERT((rec_semid = semget(key, USERNUM, 0)) != ERR);
    return OK;
FAIL:
    perror(MAN);
    return ERR;
}

int main(int argc, char *argv[])    
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, handler);
    signal(SIGCHLD, handler);
    int fdin[2], fdout[2];
    pid_t testpid = testbirth(fdin, fdout, argv[1]);
    ASSERT(getipcsrc(fdin[0], fdout[1]) != ERR);
    int recnum = checkcl(fdin[0], fdout[1]);
    int cmd;
    do {
        switch (read(0, &cmd, sizeof cmd)) {
            case 0: goto TERM;
            case -1:goto FAIL;
        }
        unsigned int i = 0;
        while (cmd != instr[i].name && (++i) < sizeof(instr)/sizeof(instr[i]));
        if (i < sizeof(instr)/sizeof(instr[0])) {
            int arg1, arg2, arg3;
            if (cmd == SAVERES || cmd == RQNUM){
                arg1 = rec_semid;
                arg2 = rec_shmid;
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
FAIL:
    perror(MAN);
    kill(testpid, SIGINT);
TERM:
    close(fdin[0]);
    close(fdout[1]);
    return OK;
}
