#ifndef SRCGET_H_INCLUDED
#define SRCGET_H_INCLUDED
#include <sys/types.h>
enum {SHMSIZE = 100, MAXNAME = 255, LEN64B = 21};
static const char extipc[] = ".test";
static const char extfifo[] = ".fifo";
static const char tmpdir[] = "/tmp/";
static const char manpath[] = "./garbage/manager.out";
static const char MtoC[] = "MtoC";
static const char CtoM[] = "CtoM";

typedef struct {
    unsigned long type; //
} ServMsg;

typedef struct {
    int id; //
    int qnum; //in [0] - number of all records
    int rnum;
} StRec;

pid_t testbirth(int fdin[2], int fdout[2], char *prog);

void getipcname (const char *topic, char filename[]);

void getfifoname(unsigned long pid, char *filename, const char *mode);

void makepipe(pid_t pid, int steam);

#endif
