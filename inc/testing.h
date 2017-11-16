enum state {ERR = -1, OK = 0};
enum commands {QUE, ANS, QNUM, TOP, SHUT};
enum {MAXLEN = 255};

#define ASSERT(x)   do {\
                        if (!(x)) {\
                            goto FAIL;\
                        }\
                    } while (0)
