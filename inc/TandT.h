// n - number of real questions!!!!!!!!
enum state {ERR = -1, OK = 0};
enum anses {RIGHT, WRONG};
enum commands {QUE, ANS, QNUM, TOP, SHUT};
enum {MAXLEN = 63};
enum quetypes {STR, NUMERAL};

#define ASSERT(x)   do {\
                        if (!(x)) {\
                            goto FAIL;\
                        }\
                    } while (0)

typedef struct {
    const char *que;
    const int type;
    void *ans;
} Que;

typedef struct {
    const int type;
    int (*func) (const char *ans, Que *que);
} Checker;
    
typedef struct {
    int name;
    void (*func) (Que task [], int n);
} Cmd;

//for test-programe
void sndq(Que task [], int n);
void sndcheck(Que task [], int n);
void sndq_num(Que task [], int n);
void sndtop(Que task [], int n);


//for tester
int getfrag(int fdin, char *frag, int n);
int reqq(int fdin, int fdout, int n);
int reqcheck(int fdin, int fdout, char* ans, int n);
int reqtop(char* topic, int fdin, int fdout);
int reqq_num (int fdin, int fdout);


