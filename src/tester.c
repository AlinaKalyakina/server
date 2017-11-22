#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "TandT.h"
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

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
    if ((chpid = fork()) == 0) {
        // test program
        ASSERT(dup2(fdin[1],1) != ERR);
        ASSERT(dup2(fdout[0],0) != ERR);
        close(fdin[0]);
        close(fdin[1]);
        close(fdout[0]);
        close(fdout[1]);
        if (argc > 1) {
            execlp(argv[1], argv[1], (char*)0);
        }
        goto FAIL;
    }
    if (chpid < 0) {
        goto FAIL;
    }
    close(fdin[1]);
    close(fdout[0]);
    //test-manager
    int q_num;    
    char topic[MAXLEN + 1];
    ASSERT((q_num = getq_num(fdin[0], fdout[1])) != ERR);
    ASSERT(gettop(topic, fdin[0], fdout[1]) != ERR);
    printf("Test ""%s"" has %d questions\n", topic, q_num);
    int cmd, i, right = 0;
    for (i = 1; i <= q_num; i++) {
        ASSERT(getq(fdin[0], fdout[1], i) != ERR);
        ASSERT((cmd = checkans(fdin[0], fdout[1],i)) != ERR);
        if (cmd == END) {
            break;
        }
        switch (cmd) {
            case ERR: 
                goto FAIL;
                break;
            case WRONG:
                puts("Wrong answer!");
                break;
            case RIGHT:
                ASSERT(puts("Right answer!") != EOF);
                right++;
                break;
        }
    }
    kill(chpid, SIGINT);
    wait(NULL);
    printf("\nYour result is %d\n", 5*right/q_num);
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
