#include "requests.h"
#include <unistd.h>
#include "general.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int
get_frag(int fdin, char *frag, int n)
{
    int len;
    READCHECK(len = read(fdin, frag, n));
    return len;
FAIL:
    perror("Getting fragment");
    return ERR;
}

int
req_q(int fdin, int fdout, int n)
{
    int str_l, cmd[2] = {QUE, n};
    ASSERT(write(fdout, cmd, sizeof cmd) > 0);
    READCHECK(read(fdin, &str_l, sizeof(str_l)));
    return str_l;
FAIL:
    perror("Question request");
    return ERR;
}

int
req_check(int fdin, int fdout, char *ans, int n)
{
    int cmd[2] = {CHECK, n};
    int str_l = strlen(ans) * sizeof(ans[0]);
    ASSERT(write(fdout, cmd, sizeof cmd) > 0);
    ASSERT(write(fdout, &str_l, sizeof(str_l)) > 0);
    ASSERT(write(fdout, ans, str_l) > 0);
    READCHECK(read(fdin, &cmd[0], sizeof(cmd[0])));
    return cmd[0];
FAIL:
    perror("Check request");
    return ERR;
}

int
req_top ( char* topic, int fdin, int fdout)
{
    int cmd = TOP;
    ASSERT(write(fdout, &cmd, sizeof cmd) > 0);
    READCHECK(read(fdin, &cmd, sizeof cmd));
    READCHECK(read(fdin, topic, sizeof(topic[0])*cmd));
    topic[cmd/sizeof(topic[0])] = '\0';
    return OK;
FAIL:
    perror("Topic request");
    return ERR;
}

int
req_q_num (int fdin, int fdout)
{
    int cmd = ALLQNUM;
    ASSERT(write(fdout, &cmd, sizeof(cmd)) > 0);
    READCHECK(read(fdin, &cmd, sizeof(cmd)));
    return cmd;
FAIL:
    perror("Number of all questions request");
    return ERR;
}

int
req_save(int fdin, int fdout, int op)
{
    int cmd[2] = {SAVERES, op};
    ASSERT(write(fdout, cmd, sizeof cmd) > 0);
    READCHECK(read(fdin, &cmd[0], sizeof cmd[0]));
    return cmd[0];
FAIL:
    perror("Save request");
    return ERR;
}

int
reqr_num(int fdin, int fdout)
{
    int cmd = RQNUM;
    ASSERT(write(fdout, &cmd, sizeof cmd) > 0);
    READCHECK(read(fdin, &cmd, sizeof cmd));
    return cmd;
FAIL:
    perror("Number of right answered questions");
    return ERR;
}
