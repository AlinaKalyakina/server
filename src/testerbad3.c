#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "TandT.h"
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

void handler (int sig)
{
    exit(0);
}

int 
main(int argc, char *argv[])
{
    signal(SIGINT, handler);
    int fdin[2], fdout[2];
    ASSERT(pipe(fdin) != ERR);
    ASSERT(pipe(fdout) != ERR);
    int chpid;
    if(argc < 2) {
        puts("No argument");
        exit(0);
    }
    if ((chpid = fork()) == 0) {
        // test program
        ASSERT(dup2(fdin[1],1) != ERR);
        ASSERT(dup2(fdout[0],0) != ERR);
        close(fdin[0]);
        close(fdin[1]);
        close(fdout[0]);
        close(fdout[1]);
        execlp(argv[1], argv[1], (char*)0);
    }
    ASSERT(chpid > 0);
    close(fdin[1]);
    close(fdout[0]);
    //test-manager
    int q_num;
    ASSERT((q_num = reqq_num(fdin[0], fdout[1])) != ERR);
    ASSERT(reqcheck(fdin[0], fdout[1], "aaaaaaaaa", q_num + 10) != ERR); 
    kill(chpid, SIGINT);
    wait(NULL);
    close(fdin[0]);
    close(fdout[1]);
    return OK;
FAIL:
    perror(NULL);
    kill(chpid, SIGINT);
    wait(NULL);
    printf("Server crashed!\n");
    close(fdin[0]);
    close(fdout[1]);
    return ERR;
}
