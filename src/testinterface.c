#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "general.h"
#include "testinterface.h"

int ansstr(const char *ans, Que *que);
int ansnum(const char *ans, Que *que);

Checker check[] = {{STR, ansstr}, {NUMERAL, ansnum}};

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

int
sndq(Que task [], int n)
{
    int num, str_l;
    READCHECK(read(0, &num, sizeof num));
    if (num > n) {
        fputs("Requested question doesn't exists", stderr);
        goto FAIL;
    }
    str_l = sizeof(task[num].que[0])*strlen(task[num].que) ;
    ASSERT(write(1, &str_l, sizeof str_l) > 0);
    ASSERT(write(1, task[num].que, str_l) > 0);
    return OK;
FAIL:
    perror(TST);
    exit(ERR);
}

int
sndcheck(Que task [], int n)
{
    int str_l, q_num;
    READCHECK(read(0,&q_num, sizeof q_num));
    READCHECK(read(0,&str_l, sizeof str_l));
    char ans[BUFLEN+1];
    int fragsize, rest = str_l, curpos = 0;
    while (rest > 0) {
        READCHECK((fragsize = read(0, ans + curpos, rest)) );
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
        ASSERT(write(1, &mark, sizeof(mark)) > 0);
    } else {
        fputs("Requested question doesn't exists", stderr);
        goto FAIL;
    }
    return OK;
FAIL:
    perror(TST);
    exit(ERR);
}

int
sndq_num(Que task [], int n)
{
    if (write(1, &n, sizeof(n)) <= 0) {
        perror(TST);
        exit(ERR);
    }
    return OK;
}

int 
sndtop(Que task [], int n)
{
    int len = strlen(task[0].que);
    ASSERT(write(1, &len, sizeof(len)) > 0);
    ASSERT(write(1,task[0].que, sizeof(task[0].que[0])*len) > 0);
    return OK;
FAIL:
    perror(TST);
    exit(ERR);
}
