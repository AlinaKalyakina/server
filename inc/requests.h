#ifndef REQUESTS_H_INCLUDED
#define REQUESTS_H_INCLUDED
enum nums { ANSNUM, RANSNUM, ALLNUM};

int get_frag(int fdin, char *frag, int n);

int req_q(int fdin, int fdout, int n);

int req_check(int fdin, int fdout, char *ans, int n);

int req_top (char* topic, int fdin, int fdout);

int req_q_num (int fdin, int fdout);

int reqr_num (int fdin, int fdout);

int req_save (int fdin, int fdout, int op);
#endif
