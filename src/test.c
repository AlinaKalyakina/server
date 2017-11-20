#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "testing.h"
#include <signal.h>

void
SigHandler(int sig)
{
    exit(0);
}

char *topic = "Topic #1";

typedef struct Que {
    const char *que;
    const char *ans;
} Que;

typedef struct cmd {
    int name;
    void (*func) (void);
} Cmd;

void que(void);

void ans(void);

void give_num(void);

void give_topic(void);

Que task[] = {{"5*5?", "25"},
              {"10-7?", "3"},
              {"12/3?", "4"}};
Cmd instr[] = {{QUE, que},
               {ANS, ans},
               {QNUM, give_num},
               {TOP, give_topic}};

void
que(void)
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
ans(void)
{
    int par_n, str_l;
    unsigned int q_num;
    ASSERT(read(0,&q_num, sizeof(q_num)) != ERR);
    ASSERT(read(0,&par_n, sizeof(par_n)) != ERR);
    char ans[MAXLEN+1];
    if (par_n > MAXLEN) {
        str_l = MAXLEN;
        par_n -= MAXLEN;
    } else {
        str_l = par_n;
        par_n =  0;
    }
    ASSERT(read(0, ans, str_l) != ERR);
    ans[str_l] = '\0';
    if (q_num < sizeof(task)/sizeof(task[0])) {
        int c = strcmp(ans, task[q_num].ans) == 0;
        ASSERT(write(1,&c,sizeof(c)) != ERR);
    }
    while (par_n > 0) {
        if (par_n < MAXLEN) {
            str_l = par_n;
        }
        read(0, ans, str_l);
        par_n -= str_l;
    }
    return;
FAIL:
    exit(ERR);
}

void
give_num(void)
{
    int c = sizeof(task)/sizeof(task[0]);
    if (write(1, &c, sizeof(c)) == ERR) {
        exit(ERR);
    }
}

void
give_topic(void)
{
    int c = strlen(topic);
    ASSERT(write(1, &c, sizeof(c)) != ERR);
    ASSERT(write(1,topic, sizeof(char)*c) != ERR);
    return;
FAIL:
    exit(ERR);
}

int main(void)
{
    signal(SIGINT, SigHandler);
    int cmd;
    do {
        read(0,&cmd, sizeof(cmd));
        unsigned int i = 0;
        while (cmd != instr[i].name && (++i) < sizeof(instr)/sizeof(instr[i]));
        if (i < sizeof(instr)/sizeof(instr[0])) {
            instr[i].func();
        } 
    } while (1);
}

