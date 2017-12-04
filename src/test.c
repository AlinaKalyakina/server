#include <unistd.h>
#include <stdlib.h>
#include "testinterface.h"
#include "general.h"
#include <signal.h>
#include <stdio.h>

void handler (int sig)
{
    exit(0);
}

Que task[] = {{"Topic #1", 0, NULL},
              {"5*5?", NUMERAL, "25"},
              {"10-7?", NUMERAL, "3"},
              {"First letter of Endlish alphabet", STR, "A"},
              {"12/3?", NUMERAL, "4"},
              {"What country we leave in?", STR, "Russia"}};

Cmd instr[] = {{QUE, sndq},
               {CHECK, sndcheck},
               {ALLQNUM, sndq_num},
               {TOP, sndtop}};

int main(void)
{
    signal(SIGINT, handler);
    int cmd = 0;
    do {
        READCHECK(read(0,&cmd, sizeof cmd));
        unsigned int i = 0;
        while (cmd != instr[i].name && (++i) < sizeof(instr)/sizeof(instr[i]));
        if (i < sizeof(instr)/sizeof(instr[0])) {
            if(instr[i].func(task, sizeof(task)/sizeof(task[0]) - 1) == ERR) {
                exit(0);
            }
        } else {
            fputs("Requested instruction doesn't exists", stderr);
            exit(0); 
        }  
    } while (1);
FAIL:
    perror(TST);
    exit(0);
}

