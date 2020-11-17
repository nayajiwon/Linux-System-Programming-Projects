#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<unistd.h>
#include<sys/types.h>
#include<time.h>


//디몬프로세스 헤더
#include<syslog.h>
#include<signal.h>
#include<sys/stat.h>
#include<sys/types.h>

#define FILE_LEN 64
#define BUF_LEN 1024
#define OP_LEN 20


void logWrite(char* logtext);

void log_isModified();
void getCertainDir();
void log_isCreatedorDeleted(char(*prelist)[BUF_LEN], int *precnt,int mtime[BUF_LEN],char *time);
void curState(int *curstat, char (*filelist)[BUF_LEN], int ret); 

void getPrelist(char *wd, char(*prelist)[BUF_LEN], int mtime[BUF_LEN]);
 void relativeP(char Fname[BUF_LEN], char fnm[BUF_LEN]);

int ssu_daemon_init(void);

//나중에 헤더에 복사
static int filter(const struct dirent *dirent);


