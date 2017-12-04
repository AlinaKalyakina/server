#ifndef GENERAL_H_INCLUDED
#define GENERAL_H_INCLUDED
#define ASSERT(x)   do {\
                        if (!(x)) {\
                            goto FAIL;\
                        }\
                    } while (0)
#define READCHECK(x) switch (x) {\
                            case -1:\
                                goto FAIL;\
                            case 0:\
                                return ERR;\
                            break;\
                            }

enum state {ERR = -1, OK = 0, RIGHT = 1, WRONG = -2, NOPLACE = -3};
enum {BUFLEN = 128};
enum commands {SAVERES, RQNUM, ALLQNUM, QUE, CHECK, TOP};

static const char SER[] = "Server";
static const char MAN[] = "Manager";
static const char CLN[] = "Client";
static const char TST[] = "Test-program";
#endif

