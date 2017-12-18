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
#include "requests.h"
#include <errno.h>
#include "general.h"
#include <errno.h>

int shmid = -1, semid = -1, msgid = -1;
char ipcfile[MAXNAME] = {'\0'};
char top[BUFLEN + 1];


void
SigHandler (int sig)
{
    signal(sig, SigHandler);
    key_t key;
    pid_t pid;
    switch (sig) {
    case SIGCHLD:
        while((pid = waitpid(-1, NULL, WNOHANG)) != 0 && pid != -1) {
            printf("Manager %d finished\n", pid);
        }
        break;
    case SIGTERM:
        signal(SIGCHLD, SIG_DFL);
        if ((pid = fork()) == 0) {
            execlp(statpath, statpath, top, (char*) NULL);
            perror(NULL);
            exit(ERR);
        }
        //check status
        signal(SIGINT, SIG_IGN);
        waitpid(pid, NULL, 0);
        ASSERT((key = ftok(ipcfile, 'a')) != ERR);
        ASSERT((semid = semget(key, USERNUM + 1, 0)) != ERR);
        ASSERT((semctl(semid, 0, IPC_RMID, (int) 0)) != ERR);
        ASSERT((msgctl(msgid, IPC_RMID, NULL)) != ERR);
        ASSERT((key = ftok(ipcfile, 'b')) != ERR);
        ASSERT((semid = semget(key, USERNUM, 0)) != ERR);
        ASSERT((semctl(semid, 0, IPC_RMID, (int) 0)) != ERR);
        ASSERT((shmctl(shmid, IPC_RMID, NULL)) != ERR);
        ASSERT((remove(ipcfile)) == 0);
        exit(0);
        break;
    case SIGINT:
        exit(0);
    }
    return;
FAIL:
    perror("Removing resources");
    exit(0);
}

int //number of questions in test
top_and_qnum(char *topic, char *prog)
{
    int fdin[2], fdout[2];
    int qnum;
    ASSERT(testbirth(fdin, fdout, prog) != ERR);
    ASSERT(req_top(topic, fdin[0], fdout[1]) != ERR);
    ASSERT((qnum = req_q_num(fdin[0], fdout[1])) != ERR);
    close(fdin[0]);
    close(fdout[1]);
    wait(NULL);
    return qnum;
FAIL:
    exit(ERR);
}

int
semorganize(key_t key)//key 'a'
{   
    semid = semget(key, USERNUM + 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == ERR && errno == EEXIST) { //sem already exists
        ASSERT((semid = semget(key, USERNUM + 1, 0)) != ERR); //just connect
        if (semctl(semid, USERNUM, GETVAL, 0) == 0) {//server exists
            puts("Server with such topic exists");
            exit(0);
        }
    } else { //sems hasn't exist yet or error while creating
        ASSERT(semid != ERR);
        for (int i = 0; i < USERNUM + 1; i++) {
            ASSERT(semctl(semid, i, SETVAL, (int)1) != ERR);
        }
    }
    //down server existence
    struct sembuf sem = {.sem_num = USERNUM, .sem_op = -1, .sem_flg = SEM_UNDO};
    ASSERT(semop(semid, &sem, 1) != ERR);
    return OK;
FAIL:
    perror(SER);
    exit(ERR);
}

void
shmorganize(key_t key, int num, int qnum)
{   
    shmid = shmget(key, num * sizeof (StRec), IPC_CREAT | IPC_EXCL | 0666);
    if (errno == EEXIST && shmid == ERR) {
        ASSERT((shmid = shmget(key, num * sizeof (StRec), 0)) != ERR);
    } else {
        ASSERT(shmid != ERR);
        StRec *rec;//shm hasn't existed before -> initializate
        ASSERT((rec = shmat(shmid, NULL, 0)) != NULL);
        memset(rec, 0, num * sizeof(StRec));
        rec[USERNUM].qnum = qnum;
        ASSERT(shmdt(rec) != ERR);
    }
    return;
FAIL:
    perror(SER);
    exit(ERR);
}

int main(int argc, char *argv[])
{
    signal(SIGTERM, SigHandler); 
    signal(SIGINT, SigHandler);
    signal(SIGPIPE, SIG_IGN);
    if (argc < 2) {
        puts("No name of testing program");
        return ERR;
    }
    int qnum = top_and_qnum(top, argv[1]);
    getipcname(top, ipcfile);
    if (access(ipcfile, F_OK) != 0) {
        int fd;
        ASSERT((fd = open(ipcfile, O_RDWR | O_CREAT | 0666)) != ERR);
        close(fd);
    } 
    //getting resources on key with 'a'
    key_t key = ftok(ipcfile, 'a');
    ASSERT(key != ERR);
    semorganize(key);//checking server existence
    ASSERT((msgid = msgget(key, IPC_CREAT | 0666)) != ERR);
    //getting resources on key with 'b'
    key = ftok(ipcfile, 'b');
    semid = semget(key, USERNUM, IPC_CREAT | IPC_EXCL | 0666);//sems for array of record
    if (semid == ERR && errno == EEXIST) { //sem already exists
        ASSERT((semid = semget(key, USERNUM, 0)) != ERR); //just connect
    } else { //sems hasn't exist yet or error while creating
        ASSERT(semid != ERR);
        for (int i = 0; i < USERNUM; i++) {
            ASSERT(semctl(semid, i, SETVAL, (int)1) != ERR);//access to records
        }
    }
    shmorganize(key, USERNUM + 1, qnum);
    //main cycle
    ServMsg rcvmsg;
    signal(SIGCHLD, SigHandler);
    while (1)
    {
        if (msgrcv(msgid, &rcvmsg, sizeof rcvmsg - sizeof(long), 0, 0) == ERR) {
            if (errno == EINTR) {
                continue;
            } else {
                goto FAIL;
            }
        }
        pid_t manpid, pidcl = rcvmsg.type;
        printf("New messange from %d\n", pidcl);
        if ((manpid = fork()) == 0)
        {
            int fd;
            char fifoname[MAXNAME + 1];
            ASSERT(makefifo(fifoname, MtoC, pidcl) != ERR);
            signal(SIGALRM, _exit);
            alarm(5);
            getfifoname(pidcl, fifoname, CtoM);
            ASSERT((fd = open(fifoname, O_RDONLY)) != ERR);
            ASSERT(dup2(fd, 0) != ERR);
            close(fd);
            getfifoname(pidcl, fifoname, MtoC);
            ASSERT((fd = open(fifoname, O_WRONLY)) != ERR);
            ASSERT(dup2(fd, 1) != ERR);
            close(fd);
            alarm(0);
            execl(manpath, manpath, argv[1],(char*)0);
            goto FAIL;            
        }
        ASSERT(manpid > 0);
        printf("Manager %d for %d was created\n", manpid, pidcl);
    }
FAIL:
    perror(SER);
}

