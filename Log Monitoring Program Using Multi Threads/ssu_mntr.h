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
#include<pthread.h>

#define FILE_LEN 64
#define BUF_LEN 1024
#define OP_LEN 20

extern char check_Dir[BUF_LEN];
extern char saved_path[BUF_LEN];
extern char eval_Dir[BUF_LEN];

extern char trashAddr[BUF_LEN]; 
extern char InfoDir[FILE_LEN];
extern char FileDir[FILE_LEN];

typedef struct LappedNameInfo{
	char dirname[BUF_LEN];
	char D[BUF_LEN];
	char M[BUF_LEN];
	char fname[BUF_LEN];
}lapInfo;

typedef struct comp{
	char fname[FILE_LEN];
	char Dtime[BUF_LEN];
}comp;



typedef struct ThreadOptions{
	int iOption;
	int nOption;
	int rOption;
	char rtime[FILE_LEN];
	char Dir[FILE_LEN];
	char trashDir[FILE_LEN];
	char InfoDir[FILE_LEN];
	char dFname[FILE_LEN];
}threadOptions;


//extern char eval_Dir[BUF_LEN];


/**************ssu_mntr.c********************/
int checkOption();
void drepath(char path[BUF_LEN]);
void ddelete(char option[OP_LEN][OP_LEN], int cnt, char rPath[BUF_LEN]);
void* dreserve_thread(void*arg);
void currentTime(char curtime[FILE_LEN]);
int d_isTime(char endTime[BUF_LEN]);
void d_makeTrashDir(char trash[FILE_LEN]);
void toTrashInfo(char* Dir, char *trashDir ,char *dFname, char *dTime);
int toTrashFiles(char *Dir, char *trashDir, char *dFname);
int checkSameFname(char *add, char *dfname);
void dinfoOver2kb();

void helpme();

/**************ssu_recover.c********************/

void printLoption(char fname[BUF_LEN]);
int getDtimeinfo(comp compare[BUF_LEN]);
void sortDtimeinfo(comp compare[BUF_LEN], int ind);
void recover(char fname[BUF_LEN]);
int getInfo_information(char dir[BUF_LEN], char fname[BUF_LEN], lapInfo sameInfo[BUF_LEN]);
int fgetsInfofile(char fname[BUF_LEN], lapInfo* sameInfo);
void putbackinCheck(lapInfo* sameInfo);
int checkFinalNum();

/**************ssu_tree.c***********************/
void Tree();
void makeTree(char *fname);

/**************ssu_size.c***********************/

void getDIRsize(char *fName, char depth[FILE_LEN] );
void relativePath(char realPath[BUF_LEN], char ans[BUF_LEN]);
void getsize(char *wd);
void dsizeOption(char *wd, int height, char ans[BUF_LEN]);
void srepath(char path[BUF_LEN]);
















