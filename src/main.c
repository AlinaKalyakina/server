#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "testing.h"
#include <signal.h>


int
getq(int fdin, int fdout, int n)
{
    int str_l, cmd[2] = {QUE, n};
    ASSERT(write(fdout, &cmd, 2*sizeof(cmd[0])) != ERR);
    ASSERT(read(fdin, &str_l, sizeof(str_l)) != ERR);
    char str[MAXLEN + 1];
    ASSERT(read(fdin, &str, sizeof(str[0])*str_l) != ERR);
    str[str_l] = '\0';
    ASSERT(puts(str) == EOF);
FAIL:
    return ERR;
}

int
checkans(int fdin, int fdout, int n)
{
    char str[MAXLEN + 1];
    ASSERT(fgets(str, MAXLEN + 1, stdin) != NULL);
    int cmd[2] = {ANS, n};
    int str_l = strlen(str)*sizeof(str[0]) - 1;
    ASSERT(write(fdout, &cmd, 2*sizeof(cmd[0])) != ERR);
    ASSERT(write(fdout, &str_l, sizeof(str_l)) != ERR);
    ASSERT(write(fdout, str, str_l) != ERR);
    while (strlen(str) == MAXLEN && str[MAXLEN - 1] == '\n') {
        ASSERT(fgets(str, MAXLEN + 1, stdin) == NULL);
    }
    ASSERT(read(fdin, &cmd[0], sizeof(cmd[0])) != ERR);
    return cmd[0];
FAIL:
    return ERR;
}

int main(int argc, char *argv[])
{
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
    int q_num, cmd = QNUM, right = 0;
    ASSERT(write(fdout[1], &cmd, sizeof(cmd)) != ERR);
    ASSERT(read(fdin[0], &q_num, sizeof(q_num)) != ERR);
    for (int i = 0; i < q_num; i++) {
        getq(fdin[0], fdout[1], i);
        ASSERT((cmd = checkans(fdin[0], fdout[1],i)) != ERR);
        if (cmd == 1) { //
            ASSERT(puts("Right answer!") != EOF);
            right++;
        } else {
            puts("Wrong answer!");
        }
    }
    kill(chpid, SIGINT);
    printf("Your result is %d\n", 5*right/q_num);
    close(fdin[0]);
    close(fdout[1]);
    return OK;
FAIL:
    perror(NULL);
    kill(chpid, SIGINT);
    printf("Server crashed!");
    close(fdin[0]);
    close(fdout[1]);
    return ERR;
}
