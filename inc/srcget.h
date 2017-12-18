#ifndef SRCGET_H_INCLUDED
#define SRCGET_H_INCLUDED
#include <sys/types.h>
/*Shm organisation:
on key with 'a': msgque, array[0..USERNUM] of sems indicators if online([USERNUM] - for server existence check)
                        0 == online, 0 == server exists
on key with 'b': array[0..USERNUM] of StRec([USERNUM for servise information), array[0..USERNUM - 1] of sems for access to records)*/
enum {USERNUM = 6, MAXNAME = 255, LEN64B = 21, STEP = 5}; //100 = 2*2*5*5, STEP = 3 ->
static const char extipc[] = ".test";
static const char extfifo[] = ".fifo";
static const char tmpdir[] = "/tmp/";
static const char MtoC[] = "MtoC";
static const char CtoM[] = "CtoM";

typedef int stid_t;

typedef struct {
    unsigned long type; //
} ServMsg;

typedef struct {
    int id;
    int qnum;//[0] - number of all records
    int rnum;
} StRec;

pid_t testbirth(int fdin[2], int fdout[2], char *prog);

void getipcname (const char *topic, char filename[]);

void getfifoname(unsigned long pid, char *filename, const char *mode);

int makefifo(char *fifoname, const char *mode, long pid);

#endif
