#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<dirent.h>
#include<unistd.h>
#include<time.h>
#include<ctype.h>


#define BUFLEN 1024
#define TIMELEN 64

typedef struct NODE{
        struct NODE *next; 
        char time[BUFLEN];
        char data[BUFLEN]; 
}node; 
/*
typedef struct TIME{
        char *min;
        char *hr;
        char *day31;
        char *mth;
        char *day7;
}time;
*/
int crontab();
int timeinputcheck(char *buf);
int operationException(int timeflag, char op, int digit);
void removenode(int num);
void readd();
void rewrite(FILE *fp, char *data);
void add(char *data);
/******************디버깅 함수********************/
void traverse();
/******************디버깅 함수********************/
void listinit();
void addinit(char *data);

void writeinlog(char order[BUFLEN], char data[BUFLEN]);

