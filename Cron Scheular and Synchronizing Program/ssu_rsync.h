#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<ctype.h>
#include<time.h>
#include<utime.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/time.h>
#define BUFLEN 1024
#define TIMELEN 64

typedef struct _Arr{
	char arr[BUFLEN];
	time_t fmtime;
	size_t fsize; 
	int count;
}_arr; 
void handler(int signo);
void removedir(char *path);
void handler(int signo);
void ssu_rsync(int argc, char *argv[]);
void makethesame(char *path, char *srcdir);

void makeandcopy_sigint(char *src, char *srccpy);
int isfileordir(char *path);
void filenameExtract(char file[BUFLEN]);
int issyncOk(char *src, char *dst);
void gorsync(_arr *src, _arr *dst, int srclen, int dstlen);
void writeinlogwithTime(char (*buf)[BUFLEN],int cnt);
void makeandcopy(char *src, char *dst);
void filesync(char *path, _arr arr[BUFLEN], int *len);
int isfileOk(char *path);

