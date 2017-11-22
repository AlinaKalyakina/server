#include <stdio.h>
#include <string.h>
#include "TandT.h"

typedef struct {
    const char *type;
    const char *name;
} Conf;

const char begin[] = "Que task[] = {";
const char end[] = "Cmd instr[] = {";

Conf type[] = {{"STR", "str"},
               {"NUMERAL", "num"}};

int main(int argc, char* argv[])
{
    char gaps[strlen(begin) + 1];
    for (unsigned int i = 0; i < strlen(begin); i++) {
        gaps[i] =' ';
    }
    gaps[strlen(begin) + 1] = '\0';
    int n = 1;
    char str[MAXLEN + 1];
    FILE *qf, *oldpf, *newpf;
    if (argc > 3) {
       if ((qf = fopen(argv[1], "r")) == NULL) {
           perror(NULL);
           return ERR;
       }
       if ((oldpf = fopen(argv[2], "r")) == NULL) {
           perror(NULL);
           return ERR;
       }
       if ((newpf = fopen(argv[3], "w")) == NULL) {
           perror(NULL);
           return ERR;
       }
    } else {
        puts("No argument file\n");
        return ERR;
    }
    while(fgets(str, MAXLEN + 1, oldpf) != NULL && strncmp(begin, str, strlen(begin)) != 0) {
        ASSERT(fputs(str, newpf) != EOF);
    }
    if (feof(oldpf)) {
        puts("Bad program!\n");
        return ERR;
    }
    ASSERT(fputs(begin, newpf) != EOF);
    ASSERT(fgets(str, MAXLEN + 1, qf) != NULL);
    str[strlen(str) - 1] = '\0';
    n++;
    ASSERT(fputs("{\"", newpf) != EOF);
    ASSERT(fputs(str, newpf) != EOF);
    ASSERT(fputs("\", 0, NULL}", newpf) != EOF);
    while (fgets(str, MAXLEN + 1, qf) != NULL ) {//que
        n++;
        ASSERT(fputs(",\n", newpf) != EOF);
        ASSERT(fputs(gaps, newpf) != EOF);
        ASSERT(fputs("{\"", newpf) != EOF);
        str[strlen(str) - 1] = '\0';
        ASSERT(fputs(str, newpf) != EOF);//que
        ASSERT(fgets(str, MAXLEN + 1, qf) != NULL);//type
        n++;
        str[strlen(str) - 1] = '\0';
        if (str[0] == '\0') {
            goto BADLINE;
        }
        unsigned int i = 0;
        while (i < sizeof(type)/sizeof(type[0]) && strcasecmp(type[i].name, str) != 0) {
            i++;
        }
        fputs("\",", newpf);
        if (i < sizeof(type)/sizeof(type[0])) {
            fputs(type[i].type, newpf);//type
        } else {
            goto BADLINE;
        }
        ASSERT(fputs(",\"", newpf) != EOF);
        ASSERT(fgets(str, MAXLEN + 1, qf) != NULL); //ans
        str[strlen(str) - 1] = '\0';
        if (str[0] == '\0') {
            goto BADLINE;
        }
        ASSERT(fputs(str, newpf) != EOF);//ans
        ASSERT(fputs("\"}", newpf) != EOF);
    }
    ASSERT(fputs("};\n", newpf) != EOF);
    while(fgets(str, MAXLEN + 1, oldpf) != NULL && strncmp(end, str, strlen(end)) != 0);
    if (feof(oldpf)) {
        puts("Bad program!\n");
        return ERR;
    }
    ASSERT(fputs(str, newpf) != EOF);
    while(fgets(str, MAXLEN + 1, oldpf) != NULL) {
        ASSERT(fputs(str, newpf) != EOF);
    }
    fclose(qf);
    fclose(oldpf);
    fclose(newpf);
    puts("SUCCESS!!!\n");
    return 0;
FAIL:
    if (feof(qf)) {
        puts("One more line was expected but not found\n");
    } else {
        perror(NULL);
    }
    fclose(qf);
    fclose(oldpf);
    fclose(newpf);
    return ERR;
BADLINE:
    printf("Line №%d is bad\n", n);
    fclose(qf);
    fclose(oldpf);
    fclose(newpf);
    return ERR;
}

