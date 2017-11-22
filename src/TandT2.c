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

// tester

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
give_que(Que task [], int n)
{
    int num, str_l;
    ASSERT(read(0, &num, sizeof(num)) != ERR);
    str_l = sizeof(task[num].que[0])*strlen(task[num].que) ;
    ASSERT(write(1, &str_l, sizeof(str_l)) != ERR);
    ASSERT(write(1,task[num].que,str_l) != ERR);
    return;
FAIL:
    exit(ERR);
}

void
rcv_ans(Que task [], int n)
{
    int str_l, q_num;
    ASSERT(read(0,&q_num, sizeof(q_num)) != ERR);
    ASSERT(read(0,&str_l, sizeof(str_l)) != ERR);
    char ans[MAXLEN+1];
    ASSERT(read(0, ans, str_l) != ERR);
    ans[str_l] = '\0';
    if (q_num <= n) {
        int type = task[q_num].type;
        unsigned int i = 0;
        while (check[i].type != type && i < sizeof(check)/sizeof(check[0])) {
            i++;
        }
        int mark;
        if (check[i].type == type) {
             mark = check[i].func(ans, &task[q_num]);//!!!
        }
        ASSERT(write(1, &mark, sizeof(mark)) != ERR);
    } else {
        goto FAIL;
    }
    return;
FAIL:
    exit(ERR);
}

void
give_num(Que task [], int n)
{
    if (write(1, &n, sizeof(n)) == ERR) {
        exit(ERR);
    }
}

void
give_topic(Que task [], int n)
{
    int len = strlen(task[0].que);
    ASSERT(write(1, &len, sizeof(len)) != ERR);
    ASSERT(write(1,task[0].que, sizeof(task[0].que[0])*len) != ERR);
    return;
FAIL:
    exit(ERR);
}
