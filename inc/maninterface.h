#ifndef MANDC_H_INCLUDED
#define MAAND_H_INCLUDED
enum saveop {ADDR, ADDW};
static const char ManWait[] = "Something has gone wrong...Wait for a new manager";

typedef struct {
    int name;
    int (*func) (int fdin, int fdout, int recnum);
} MCmd;

int give_q(int fdin, int fdout, int garb);

int give_check(int fdin, int fdout, int recnum);

int save(int semid, int shmid, int recnum);

int give_right_num(int semid, int shmid, int recnum);

int give_all_q_num(int fdin, int fdout, int garb);

int getclid(void);

int getinfo(int fdin, int fdout, int id);

int get_num(int fdin, int fdout, int mode);

#endif
