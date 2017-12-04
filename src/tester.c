#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "TandT.h"
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int
getans(char *ans)
{
    ASSERT(fgets(ans, MAXLEN + 2, stdin) != NULL);
    if (ans[strlen(ans) - 1] != '\n') {
        while (ans[strlen(ans) - 1] != '\n' && !feof(stdin)) {
            ASSERT(fgets(ans, MAXLEN + 2, stdin) != NULL);
        }
        if (feof(stdin)) {
            goto FAIL;
        }
        puts("\nYour answer is too long, try again");
    return ERR;
    }
    return OK;
FAIL:
    if(!feof(stdin)) {
        perror(NULL);
    }
    exit(0);
}

void
dispque(int fdin, int fdout, int qn)
{
    int str_l, buf_size, real_size;
    ASSERT((str_l = reqq(fdin, fdout, qn)) != ERR);
    if (str_l < MAXLEN) {
        buf_size = str_l;
    } else {
        buf_size = MAXLEN;
    }
    char str[MAXLEN + 1];
    int rest = str_l;
    while (rest > 0) {
        ASSERT((real_size = getfrag(fdin, str, buf_size)) != ERR);
        rest -=real_size;
        str[real_size/sizeof(str[0])] = '\0';
        printf("%s", str);
    }
    puts("");
    return;
FAIL:
    perror(NULL);
    exit(0);
}

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
    char topic[MAXLEN + 1];
    ASSERT((q_num = reqq_num(fdin[0], fdout[1])) != ERR);
    ASSERT(reqtop(topic, fdin[0], fdout[1]) != ERR);
    printf("Test ""%s"" has %d questions\n", topic, q_num);
    int right = 0;
    for (int i = 1; i <= q_num; i++) {
        char ans[MAXLEN + 2];
        do {
            dispque(fdin[0], fdout[1], i);
        } while ((getans(ans)) == ERR);
        switch (reqcheck(fdin[0], fdout[1], ans, i)) {
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
    printf("\nYour result is %d\n", (5*right)/q_num);
    close(fdin[0]);
    close(fdout[1]);
    wait(NULL);
    return OK;
FAIL:
    perror(NULL);
    printf("Server crashed!\n");
    close(fdin[0]);
    close(fdout[1]);
    wait(NULL);
    return ERR;
}
