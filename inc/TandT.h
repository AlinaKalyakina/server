// n - number of real questions!!!!!!!!
enum state {ERR = -1, OK = 0, END = -2};
enum anses {RIGHT, WRONG};
enum commands {QUE, ANS, QNUM, TOP, SHUT};
enum {MAXLEN = 255};
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
void give_que(Que task [], int n);
void rcv_ans(Que task [], int n);
void give_num(Que task [], int n);
void give_topic(Que task [], int n);


//for tester
int getq(int fdin, int fdout, int n);
int checkans(int fdin, int fdout, int n);
int gettop ( char* topic, int fdin, int fdout);
int getq_num (int fdin, int fdout);


