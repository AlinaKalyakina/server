#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "general.h"
#include "srcget.h"
#include <stdlib.h>
#include <string.h>
#include "statinterface.h"
#include <signal.h>

StRec *records = NULL;
FILE *idf = NULL, *outf = NULL;

void handler(int sig)//sigint
{
    if (idf != NULL) {
        fclose(idf);
    }
    if (outf != NULL) {
        fclose(outf);
    }
    exit(0);
}

static StatCmd interface[] = {{'s', shortstat}, {'l', longstat}};
char introduction[] = "Welcome! This program can:\n1)show and save short and long statistics\n2)"\
        "For the short one write command 's', for long - 'l'. You can close this program any time with ^C.";

int
getcmd(void)
{
    char buf[BUFLEN];
    ASSERT(fgets(buf, BUFLEN, stdin) != NULL);
    if (buf[1] == '\n') {
        return (int)(unsigned char)buf[0];
    }
    while (buf[strlen(buf) - 1] != '\n') {
        ASSERT(fgets(buf, BUFLEN, stdin) != NULL);
    }
    return WRONG;
FAIL:
    if (!feof(stdin)) {
        perror (NULL);
    }
    return ERR;
}

int
getstr(char *name)
{
    ASSERT(fgets(name, MAXNAME + 2, stdin) != NULL);
    if (name[strlen(name) - 1] != '\n') {
        while (name[strlen(name) - 1] != '\n' && !feof(stdin)) {
            ASSERT(fgets(name, MAXNAME + 2, stdin) != NULL);
        }
        if (feof(stdin)) {
            goto FAIL;
        }
        puts("Your answer is too long, try again");
        return WRONG;
     }
    if (strlen(name) == 1) {
        puts("Your answer is empty, try again");
        return WRONG;
    }
    name[strlen(name) - 1] = '\0';
    return OK;
FAIL:
    if(!feof(stdin)) {
        perror(NULL);
    }
    return ERR;
}

int
getsavename(char *name)
{
    puts("Do you want to save it into file(y/n) or just see at screen?");
    int c = getcmd();
    while (c == WRONG || !(c == 'y' || c == 'Y' || c == 'n' || c == 'N')) {
        puts("Wrong. Try again");
        c = getcmd();
    }
    ASSERT(c != ERR);
    if (c == 'n' || c == 'N') {
        name[0] = '\0';
    } else {
        puts("Write file name");
        while ((c = getstr(name)) == WRONG) {
             puts("Wrong. Try again");
        }
        ASSERT(c != ERR);
    }
    return OK;
FAIL:
    return ERR;
}

//[1] - topic, [2] - name of id file
int main(int argc, char *argv[])
{
    signal(SIGINT, handler);
    if (argc < 2) {
        puts("No topic");
        exit(0);
    }
    char ipcname[MAXNAME + 1];
    getipcname(argv[1], ipcname);
    if (access(ipcname, F_OK) != 0) {
        puts("No such topic");
        exit(0);
    }
    key_t key;
    int onl_semid, rec_semid, rec_shmid;
    ASSERT((key = ftok(ipcname, 'a')) != ERR);
    ASSERT((onl_semid = semget(key, USERNUM + 1, 0)) != ERR);
    ASSERT((key = ftok(ipcname, 'b')) != ERR);
    ASSERT((rec_semid = semget(key, USERNUM, 0)) != ERR);
    ASSERT((rec_shmid = shmget(key, sizeof(StRec) * (USERNUM + 1), 0)) != ERR);
    ASSERT((records = shmat(rec_shmid, NULL, 0)) != NULL);
    char cmd;
    puts(introduction);
    while (1) {
        puts("Write command");
        if ((cmd = getcmd()) == ERR) {
            goto TERM;
        }
        int i = 0;
        while(cmd != interface[i].cmd && i++ < sizeof(interface)/sizeof(interface[0]));
        if (i < sizeof(interface)/sizeof(interface[0])) {
            char outfname[MAXNAME + 2], idfname[MAXNAME + 2];
            int suc = 0;
            while (1) {
                if (getsavename(outfname) == ERR) {
                    goto TERM;
                }
                if (outfname[0] == '\0') {
                    outf = stdout;
                    break;
                } else {
                    if ((outf = fopen(outfname, "w")) == NULL) {
                        perror("Try again");
                    } else break;
                }
            }
            if (cmd == 'l') {
                while (1) {
                    puts("Write name of file with ids");
                    while ((suc = getstr(idfname)) == WRONG) {
                         puts("Wrong. Try again");
                    }
                    ASSERT(suc != ERR);
                    if ((idf = fopen(idfname, "r")) == NULL) {
                        perror("Try again");
                    } else break;
                }
            }
            if (interface[i].func(records, rec_semid, onl_semid, outf, idf) == ERR) {
                goto TERM;// !!!!!!!!!!!
            }
        } else {
            puts("Wrong instruction. Try again!");
        }
    }
TERM:
    ASSERT(shmdt(records) != ERR);
    if (idf != NULL) {
        fclose(idf);
    }
    if (outf != NULL) {
        fclose(outf);
    }
    return 0;
FAIL:
    perror("Statistic");
    fclose(idf);
    fclose(outf);
    return 0;
}
