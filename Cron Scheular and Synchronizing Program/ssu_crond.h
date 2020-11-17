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
//디몬프로세스 헤더
#include<syslog.h>
#include<signal.h>
#include<pthread.h>

#define FILELEN 64
#define BUFLEN 1024


typedef struct _finfo{
        struct _finfo *next; 
        char data[BUFLEN];
        char createtime[BUFLEN];
}finfo;

typedef struct _Time{
	int min;
	int hr;
	int mday;
	int tm_mon;
	int tm_wday;
	int tm_sec;
}_time;

typedef struct _Command{
	char command[BUFLEN];
	char fulldata[BUFLEN]; 
}_command; 

typedef struct arr{
	int count; 
	int arr[BUFLEN];
}Arr; 


int ssu_daemon_init(void);
void toreserve(char (*prelist)[BUFLEN], time_t *mtime, int *precount);
void removenode_num(int num, char *data);
void getPrelist(char (*curlist)[BUFLEN], int *count);
void waitiltime();
void tokenanalizer(char *data);
void devidetoken(char *timeinfo);
int possiblecase(Arr *_possi, char command[BUFLEN], char fulldata[BUFLEN]);
void *systemcommand(void *arg);
void removenode(char *data);
void writeinlogdir(char *data);
void putin_possi(Arr *possi, Arr *array, int arrind);
void fillfulltime(int step, Arr *array);
void devidecalculate(char *ptr, Arr *array);
void dashcalculate(char *ptr1, char *ptr2, Arr *array);
void add(char *data);
void getcurtime(char *curTime);
void getcurTime(_time *curTime);
void getsec(int *sec);
void traverse();



