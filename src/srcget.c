#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include "srcget.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include "general.h"

pid_t
testbirth(int fdin[2], int fdout[2], char *prog)
{
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
        execlp(prog, prog, (char*)0); //!!!!!!!!
        goto FAIL;
    }
    if (chpid < 0) {
        goto FAIL;
    }
    close(fdin[1]);
    close(fdout[0]);
    return chpid;
FAIL:
    perror(NULL);
    exit(0);
}

void
getipcname (const char *topic, char filename[MAXNAME + 1])
{
    filename[0] = '\0';
    strcat(filename, tmpdir);
    int i = -1, count = strlen(filename);
    while (topic[++i] != '\0' && count < MAXNAME - strlen(extipc)) {
        if (topic[i] == '/') {
            continue;
        }
        filename[count++] = topic[i];
    }
    filename[count] = '\0';
    strcat(filename, extipc);
}

void
ptoa(unsigned long n, char *str)
{
    int i = 0;
    char tmp;
    while(n != 0) {
        tmp = '0' + n % 10;
        str[i++] = tmp;
        n /= 10;
    }
    for (int j = 0; j < i/2; j++) {
        tmp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = tmp;
    }
    str[i] = '\0';
}

void
getfifoname(unsigned long pid, char *filename, const char* mode)
{
    char str[21];
    filename[0] = '\0';
    ptoa(pid, str);
    strcat(filename, tmpdir);
    strcat(filename, mode);
    strcat(filename, str);
    strcat(filename, extfifo);
}


