#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "general.h"
#include "maninterface.h"
#include "srcget.h"
#include "requests.h"
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <fcntl.h>
#include <setjmp.h>
#include <sys/stat.h>
#define GETMAN(x)   if (x) {\
                        puts(ManWait);\
                        goto NEWMAN;\
                    }

enum {CONATT = 3};

int msgid;
char fifoCtoM[MAXNAME + 1], fifoMtoC[MAXNAME + 1];

int // WRONG == wrong format, ERR = error or ctrl+D, OK == ok
getans(char *ans)
{
    ASSERT(fgets(ans, BUFLEN + 2, stdin) != NULL);
    if (ans[strlen(ans) - 1] != '\n') {
        while (ans[strlen(ans) - 1] != '\n' && !feof(stdin)) {
            ASSERT(fgets(ans, BUFLEN + 2, stdin) != NULL);
        }
        if (feof(stdin)) {
            goto FAIL;
        }
        puts("Your answer is too long, try again");
        return WRONG;
    }
    if (strlen(ans) == 1) {
        puts("Your answer is empty, try again");
        return WRONG;
    }
    ans[strlen(ans) - 1] = '\0';
    return OK;
FAIL:
    if(!feof(stdin)) {
        perror(NULL);
    }
    return ERR;
}

int
dispque(int fdin, int fdout, int qn)
{
    int str_l, real_size;
    ASSERT((str_l = req_q(fdin, fdout, qn)) != ERR);
    char str[BUFLEN + 1];
    int rest = str_l;
    while (rest > 0) {
        ASSERT((real_size = get_frag(fdin, str, rest)) > 0);
        rest -=real_size;
        str[real_size/sizeof(str[0])] = '\0';
        printf("%s", str);
    }
    puts("");
    return OK;
FAIL:
    return ERR;
}

void handler (int sig)
{
    ServMsg mes;
    switch (sig) {
    case SIGALRM:
        msgrcv(msgid, &mes, sizeof mes - sizeof(long), getpid(), IPC_NOWAIT);
        puts("The server response time has expired, try again later");
        break;
    default:
        break;
    }
    remove(fifoCtoM);
    remove(fifoMtoC);
    exit(0);
}

int
introduction(void)
{
    char ans[BUFLEN + 2];
    int id, suc;
    puts("Introduct yourself, please");
    while(!((suc = getans(ans)) != WRONG && sscanf(ans, "%d", &id) == 1 && id > 0)) {
        if (feof(stdin)) {
            exit(0);
        }
        puts("Try again");
    }
    if (suc == ERR) {
        id = 0;
    }
    return id;
}


int
main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, handler);
    signal(SIGALRM, handler);
    getfifoname(getpid(), fifoCtoM, CtoM);
    getfifoname(getpid(), fifoMtoC, MtoC);
    if (argc < 2) {
        puts("You haven't told test topic");
        exit(0);
    }
    char ipcfile[MAXNAME + 1];
    getipcname(argv[1], ipcfile);
    if (access(ipcfile, F_OK) != 0) {
        puts("Server with such topic doesn't exist");
        exit(0);
    }
    key_t key = ftok(ipcfile, 'a');
    ASSERT(key != ERR);
    msgid = msgget(key, 0);
    if (errno == ENOENT) {
        puts("Server with such topic doesn't exist");
        exit(0);
    }
    ASSERT(msgid != ERR);
    int id = introduction();
    if (id == 0) {
        goto TERM;
    }
    int managernum = 0, reportnum = 0;
NEWMAN:
    managernum++;
    if (managernum > 3) {
        puts("Too many attempts to reanimate manager");
        goto TERM;
    }
    ASSERT(makefifo(fifoCtoM, CtoM, getpid()) != ERR);
    ServMsg mes;
    mes.type = getpid();
    int fdout = -1, fdin = -1;
    alarm(3);
    msgsnd(msgid, &mes, sizeof(mes) - sizeof(long), 0);
    ASSERT((fdout = open(fifoCtoM, O_WRONLY)) != ERR);
    ASSERT((fdin = open(fifoMtoC, O_RDONLY)) != ERR);
    alarm(0);
    int ansq_num, allq_num, right; 
    GETMAN((ansq_num = getinfo(fdin, fdout, id)) == ERR);
    if (ansq_num == NOPLACE) {
        puts("No more place for a new record");
        goto TERM;
    } else {
        if (ansq_num == WRONG) {
            puts("Someone with the same id is passing this test");
            goto TERM;
        }
    }
    GETMAN((allq_num = req_q_num(fdin, fdout)) == ERR);
    GETMAN((right = reqr_num(fdin, fdout)) == ERR);
    if (allq_num == ansq_num) {
        printf("You've already passed this test. Your result was %d.\n", right * 5 /allq_num);
        goto TERM;
    }
    if (reportnum == 0) {
        printf("Test contains %d questions, you've answered %d (%d of them correctly)\n", \
               allq_num, ansq_num, right);
        reportnum++;        
    }
    int i;
    for (i = ansq_num + 1; i <= allq_num; i++) {
        char ans[BUFLEN + 2];
        int res;
        do {
            GETMAN(dispque(fdin, fdout, i) == ERR);
        } while ((res = getans(ans)) == WRONG);
        if (res == ERR) {
            goto TERM;
        }
        switch (res = req_check(fdin, fdout, ans, i)) {
            case ERR:
                GETMAN(1);
                break;
            case WRONG:
                puts("Wrong answer!");
                break;
            case RIGHT:
                ASSERT(puts("Right answer!") != EOF);
                right++;
                break;
        }
        GETMAN(req_save(fdin, fdout, res) == ERR);
    }
    close(fdin);
    close(fdout);
    remove(fifoCtoM);
    remove(fifoMtoC);
    printf("\nYour result is %d\n", (5*right)/allq_num);
    return OK;
FAIL:
    perror(NULL);
TERM:
    alarm(0);
    close(fdin);
    close(fdout);
    remove(fifoCtoM);
    remove(fifoMtoC);
    return OK;
}
