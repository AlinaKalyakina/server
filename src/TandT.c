#include "TandT.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int ansstr(const char *ans, Que *que);
int ansnum(const char *ans, Que *que);

Checker check[] = {{STR, ansstr}, {NUMERAL, ansnum}};

//tester

int 
getfrag(int fdin, char *frag, int n)
{
    return read(fdin, frag, n);
}

int
reqq(int fdin, int fdout, int n)
{
    int str_l, cmd[2] = {QUE, n};
    ASSERT(write(fdout, &cmd, 2 * sizeof(cmd[0])) != ERR);
    ASSERT(read(fdin, &str_l, sizeof(str_l)) != ERR);
    return str_l;
FAIL:
    return ERR;
}

int
reqcheck(int fdin, int fdout, char *ans, int n)
{
    int cmd[2] = {ANS, n};
    int str_l = (strlen(ans) - 1) * sizeof(ans[0]);
    ASSERT(write(fdout, &cmd, 2*sizeof(cmd[0])) != ERR);
    ASSERT(write(fdout, &str_l, sizeof(str_l)) != ERR);
    ASSERT(write(fdout, ans, str_l*sizeof(ans[0])) != ERR);
    ASSERT(read(fdin, &cmd[0], sizeof(cmd[0])) != ERR);
    return cmd[0];
FAIL:
    return ERR;
}

int 
reqtop ( char* topic, int fdin, int fdout)
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
reqq_num (int fdin, int fdout)
{
    int cmd = QNUM;
    ASSERT(write(fdout, &cmd, sizeof(cmd)) != ERR);
    ASSERT(read(fdin, &cmd, sizeof(cmd)) != ERR);
    return cmd;
FAIL:
    return ERR;
}

// test program

int 
ansstr(const char *ans, Que *que)
{
    if (strcasecmp(ans, que->ans) == 0) {
        return RIGHT;
    }
    return WRONG;
}

int 
ansnum(const char *ans, Que *que)
{
    double n;
    sscanf(ans, "%lf", &n);
    int i = 0, dotn = 0;
    while (ans[i] == ' ') {
        i++;
    }
    while (ans[i] != '\0' && dotn < 2 && (isdigit(ans[i]) || ans[i] == '.')) {
        i++;
        if (ans[i] == '.') {
            dotn++;
        }
    }
    while (ans[i] == ' ') {
        i++;
    }
    if (n == atoi(que->ans) &&  ans[i] == '\0') {
        return RIGHT;
    }
    return WRONG;
}

void
sndq(Que task [], int n)
{
    int num, str_l;
    ASSERT(read(0, &num, sizeof(num)) != ERR);
    if (num > n) {
        fputs("Requested question doesn't exists", stderr);
        goto FAIL;
    }
    str_l = sizeof(task[num].que[0])*strlen(task[num].que) ;
    ASSERT(write(1, &str_l, sizeof(str_l)) != ERR);
    ASSERT(write(1,task[num].que,str_l) != ERR);
    return;
FAIL:
    exit(ERR);
}

void
sndcheck(Que task [], int n)
{
    int str_l, q_num;
    ASSERT(read(0,&q_num, sizeof(q_num)) != ERR);
    ASSERT(read(0,&str_l, sizeof(str_l)) != ERR);
    char ans[MAXLEN+1];
    int fragsize, rest = str_l, curpos = 0;
    while (rest > 0) {
        ASSERT((fragsize = read(0, ans + curpos, rest)) != ERR);
        rest -= fragsize;
        curpos += fragsize/sizeof(ans[0]); 
    }
    ans[str_l/sizeof(ans[0])] = '\0';
    if (q_num <= n) {
        int type = task[q_num].type;
        unsigned int i = 0;
        while (check[i].type != type) {
            i++;
        }
        int mark = check[i].func(ans, &task[q_num]);
        ASSERT(write(1, &mark, sizeof(mark)) != ERR);
    } else {
        fputs("Requested question doesn't exists", stderr);
        goto FAIL;
    }
    return;
FAIL:
    exit(ERR);
}

void
sndq_num(Que task [], int n)
{
    if (write(1, &n, sizeof(n)) == ERR) {
        exit(ERR);
    }
}

void
sndtop(Que task [], int n)
{
    int len = strlen(task[0].que);
    ASSERT(write(1, &len, sizeof(len)) != ERR);
    ASSERT(write(1,task[0].que, sizeof(task[0].que[0])*len) != ERR);
    return;
FAIL:
    exit(ERR);
}
