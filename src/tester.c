#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "testing.h"
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

void
SigHandler(int sig) //get SIGINT
{
    puts("Goodbye!!!\n");
    signal(SIGINT, SIG_DFL);
    exit(OK);
}

int
getq(int fdin, int fdout, int n)
{
    int str_l, cmd[2] = {QUE, n};
    ASSERT(write(fdout, &cmd, 2*sizeof(cmd[0])) != ERR);
    ASSERT(read(fdin, &str_l, sizeof(str_l)) != ERR);
    char str[MAXLEN + 1];
    ASSERT(read(fdin, &str, sizeof(str[0])*str_l) != ERR);
    str[str_l] = '\0';
    ASSERT(puts(str) != EOF);
    return OK;
FAIL:
    return ERR;
}

int
checkans(int fdin, int fdout, int n)
{
    char str[MAXLEN + 1];
    ASSERT(fgets(str, MAXLEN + 1, stdin) != NULL);
    ASSERT(!feof(stdin));
    int cmd[2] = {ANS, n};
    int str_l = (strlen(str) - 1)*sizeof(str[0]);
    ASSERT(write(fdout, &cmd, 2*sizeof(cmd[0])) != ERR);
    ASSERT(write(fdout, &str_l, sizeof(str_l)) != ERR);
    ASSERT(write(fdout, str, str_l*sizeof(str[0])) != ERR);
    while (str[strlen(str) - 1] != '\n') {
        ASSERT(fgets(str, MAXLEN + 1, stdin) != NULL);
        ASSERT(!feof(stdin));
    }
    ASSERT(read(fdin, &cmd[0], sizeof(cmd[0])) != ERR);
    return cmd[0];
FAIL:
    if (feof(stdin)) {
        return END;
    }
    return ERR;
}

int 
gettop ( char* topic, int fdin, int fdout)
{
    int cmd = TOP;
    ASSERT(write(fdout, &cmd, sizeof(cmd)) != ERR);
    ASSERT(read(fdin, &cmd, sizeof(cmd)) != ERR);
    ASSERT(read(fdin, topic, sizeof(topic[0])*cmd) != ERR);
    topic[cmd/sizeof(topic[0])] = '\0';
    return OK;
FAIL:
    return ERR;
}

int
getq_num (int fdin, int fdout)
{
    int cmd = QNUM;
    ASSERT(write(fdout, &cmd, sizeof(cmd)) != ERR);
    ASSERT(read(fdin, &cmd, sizeof(cmd)) != ERR);
    return cmd;
FAIL:
    return ERR;
}

int 
main(int argc, char *argv[])
{
    signal(SIGINT, SigHandler);
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
    for (i = 0; i < q_num; i++) {
        ASSERT(getq(fdin[0], fdout[1], i) != ERR);
        ASSERT((cmd = checkans(fdin[0], fdout[1],i)) != ERR);
        if (cmd == END) {
            break;
        }
        switch (cmd) {
            case ERR: 
                goto FAIL;
                break;
            case 0:
                puts("Wrong answer!");
                break;
            case 1:
                ASSERT(puts("Right answer!") != EOF);
                right++;
                break;
        }
    }
    kill(chpid, SIGINT);
    wait(&chpid);
    printf("\nYour result is %d\n", 5*right/q_num);
    close(fdin[0]);
    close(fdout[1]);
    return OK;
FAIL:
    perror(NULL);
    kill(chpid, SIGINT);
    wait(&chpid);
    printf("Server crashed!\n");
    close(fdin[0]);
    close(fdout[1]);
    return ERR;
}
