#ifndef DATAOP_H_INCLUDED
#define DATAOP_H_INCLUDED
int find(int clid, StRec *rec, int semid, char flag, StRec *buf);

int getdata(StRec *rec, int recnum, int semid, char flag);

int mkchg(StRec *rec, int rec_semid, int recnum, int op);

int sumrec(StRec *rec, int semid, StRec *buf);
#endif
