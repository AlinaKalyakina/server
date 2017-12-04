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

void
SigHandler (int sig)
{
    signal(sig, SigHandler);
    switch (sig) {
    case SIGCHLD:
        printf("Manager %d finished\n", wait(NULL));
        break;
    case SIGTERM:
        msgctl(msgid, IPC_RMID, NULL);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID, (int) 0);
        remove(ipcfile);
        exit(0);
        break;
    case SIGINT:
        exit(0);
    }
}
void
topic(char *topic, char *prog)
{
    int fdin[2], fdout[2];
    ASSERT(testbirth(fdin, fdout, prog) != ERR);
    ASSERT(req_top(topic, fdin[0], fdout[1]) != ERR);
    puts(topic);
    close(fdin[0]);
    close(fdout[1]);
    wait(NULL);
    return;
FAIL:
    exit(ERR);
}

void
initshm (void)
{
    StRec* rec = shmat(shmid, 0, 0);
    ASSERT(rec != NULL);
    rec->qnum = 0;
    ASSERT(shmdt(rec) != ERR);
    return;
FAIL:
    perror(SER);
    exit(ERR);
}


int
existence(key_t key)
{
    semid = semget(key, 3, IPC_CREAT | IPC_EXCL | 0666);
    if (errno == EEXIST) { //sem already exists
        ASSERT((semid = semget(key, 3, 0)) != ERR); //just connect
        if (semctl(semid, 1, GETVAL, 0) != 0) {//server exists
            puts("Server with such topic exists");
            exit(0);
        }
    } else {
        ASSERT(semid != ERR);
    }
    ASSERT(semctl(semid, 0, SETVAL, (int)1) != ERR);
    ASSERT(semctl(semid, 1, SETVAL, (int)0) != ERR);
    ASSERT(semctl(semid, 2, SETVAL, (int)0) != ERR);
    struct sembuf sem;
    sem.sem_num = 1;
    sem.sem_op = 1;
    sem.sem_flg = SEM_UNDO;
    ASSERT(semop(semid, &sem, 1) != ERR);
    return OK;
FAIL:
    perror(SER);
    exit(ERR);
}

int
shmexisted(key_t key)
{
    shmid = shmget(key, SHMSIZE * sizeof(StRec), IPC_CREAT | IPC_EXCL | 0666);
    if (errno == EEXIST) {
        ASSERT((shmid = shmget(key, SHMSIZE * sizeof(StRec), IPC_CREAT | 0666)) != ERR);
        return 0;//server hasn't existed before
    } else {
        ASSERT(shmid != ERR);
    }
    return 1; //server existed
FAIL:
    perror(SER);
    exit(ERR);
}

int main(int argc, char *argv[])
{
    signal(SIGTERM, SigHandler); //!!!!!!!!!!!!!
    signal(SIGINT, SigHandler);
    signal(SIGPIPE, SIG_IGN);
    if (argc < 2) {
        puts("No name of testing program");
        return ERR;
    }
    char top[BUFLEN + 1];
    topic(top, argv[1]);
    getipcname(top, ipcfile);
    int fd = -1;
    if (access(ipcfile, F_OK) != 0) {
        ASSERT(fd = open(ipcfile, O_RDWR | O_CREAT | 0666) != ERR);
    }
    close(fd);
    key_t key = ftok(ipcfile, 'a');
    ASSERT(key != ERR);
    existence(key);//getting sems
    if (!shmexisted(key)) { //getting shm
        initshm();// server hasn't existed yet or finished correctly
    }
    ASSERT((msgid = msgget(key, IPC_CREAT | 0666)) != ERR);
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
            char fifoname[MAXNAME + 1];
            getfifoname(pidcl, fifoname, MtoC);
            ASSERT((fd = open(fifoname, O_WRONLY)) != ERR);
            ASSERT(dup2(fd, 1) != ERR);
            close(fd);
            getfifoname(pidcl, fifoname, CtoM);
            ASSERT((fd = open(fifoname, O_RDONLY)) != ERR);
            ASSERT(dup2(fd, 0) != ERR);
            close(fd);
            execl(manpath, manpath, argv[1],(char*)0);
            goto FAIL;            
        }
        ASSERT(manpid > 0);
        printf("Manager %d for %d was created\n", manpid, pidcl);
    }
FAIL:
    perror(NULL);
}

