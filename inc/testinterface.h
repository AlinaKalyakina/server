// n - number of real questions!!!!!!!!
enum quetypes {STR, NUMERAL};

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
    int (*func) (Que task [], int n);
} Cmd;

//for test-programe
int sndq(Que task [], int n);
int sndcheck(Que task [], int n);
int sndq_num(Que task [], int n);
int sndtop(Que task [], int n);
