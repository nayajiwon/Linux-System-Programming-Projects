#include "ssu_daemon.h"

char curr_path[BUF_LEN];
char log_path[BUF_LEN];

char check_Dir[BUF_LEN];
char saved_path[BUF_LEN];
char eval_Dir[BUF_LEN];

char logDir[BUF_LEN]; 

int compcnt=0;

char PATH[BUF_LEN];

int flag = 0; 


int main(){
	time_t current_time;
	struct tm* Tm; 
	char curTime[BUF_LEN];
	int tlen;
	pid_t pid;
	char prelist[BUF_LEN][BUF_LEN];
	int precnt; 
	int ret = 0;
       	struct dirent **filelist;  
	time_t intertime;
	char fDir[BUF_LEN];
	struct stat statbuf[BUF_LEN];
		
	int mtime[BUF_LEN] = {-1,};

	getCertainDir();
        

	sprintf(logDir, "%s/log.txt", saved_path);// 실행 디렉토리에 Log.txt 파일을 만듦


//	printf("logDir, %s\n", logDir);


 	if(ssu_daemon_init() < 0){//데몬 프로세스를 시작
                fprintf(stderr, "ssu_daemon_init failed\n");
                exit(1);
        }

	
	//파일들의 상태, 개수 를 저장 
	getPrelist(eval_Dir,prelist,mtime);
	precnt = compcnt; 
	compcnt = 0; //전역, 또써야함 
		
	
	while(1){
		
		time(&current_time);//시간을 구한다 
		Tm = localtime(&current_time);
		strftime(curTime, sizeof(curTime), "%Y-%m-%d %H:%M:%S", Tm); 
	
		//생성, 삭제, 변형 여부를 판단하는 함수를 부른다
		//함수를 들어가기 전 값과 나올 때의 값이 다르면 셋중에 하나 
		log_isCreatedorDeleted(prelist, &precnt, mtime ,curTime);
		flag = 0; 
		 
	}


}



//데몬 프로세스를 하는 함수 
int ssu_daemon_init(void){
	pid_t pid;
	int fd, maxfd;

	if((pid=fork())<0){
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid!=0) //부모 프로세스는 죽인다 				
		exit(0);
	

	pid = getpid();//데몬의 pid를 받아온다 
	//printf("process %d running as daemon\n", pid);
	setsid();//독자적인 프로세스 그룹을 생성 
	signal(SIGTTIN, SIG_IGN);//시그널을 무시 
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	//존재하는 fd크기 
	maxfd = getdtablesize(); 

	for(fd = 0; fd < maxfd; fd++)//현존하는 fd를 모두 죽인다 
		close(fd);
	
	umask(0);
	
	chdir("/");
	fd = open("/dev/null", O_RDWR);//표준출력을 모두 Null로 보냄 
	dup(0);
	dup(0);
	return 0; 
}

/** 삭제, 혹은 생성 된 상태를 알아보기 위해 상태가 변하기 전 대상과  mtime 배열에 저장**/ 
void getPrelist(char *wd, char(*prelist)[BUF_LEN], int mtime[BUF_LEN]){

    struct dirent **items;
    int nitems, i, j;
    struct stat fstat;
    struct stat statbuf[BUF_LEN];
    time_t intertime;
    char per;


    //디렉토리 검사를 위해 재귀를 돌림 
    if (chdir(wd) < 0){
        fprintf(stderr, "size 하기 위한 디렉토리가 존재하지 않습니다 \n");
        return;
    }


    
    nitems = scandir(".", &items, NULL, alphasort);
	

    for (i = 0; i < nitems; i++)
    {
        struct stat fstat;


	//. , ..  는 무시 
        if ( (!strcmp(items[i]->d_name, ".")) || (!strcmp(items[i]->d_name, "..")) )
        {
            continue;
        }
	//상태, 디렉토리 혹은 파일 
        lstat(items[i]->d_name, &fstat);

	//삭제를 했을 경우 stat 를 해주게 되면 없는 파일에 대한 stat error가 뜹니다. 
	//while문으로 인해 삭제가 되기 전에 이 함수에 오고, 삭제가 되는 경우가 있어 나오는 stat error를 방지하기 위한 조치입니다 
	if(access(prelist[i], F_OK)!=0 && flag == 100)
		continue;

	//mtime을 위한 스태트
	if(stat(items[i]->d_name, &statbuf[compcnt])<0){
		fprintf(stderr, "1) stat error for %s\n", items[i]->d_name);
		//exit(1);
	}
		
	//mtime을 파일의 개수만큼 배열에 저장 
	intertime = statbuf[compcnt].st_mtime;

	mtime[compcnt] = intertime;


	//절대 경로를 받음 
	realpath(items[i]->d_name, PATH);
	//printf("	~~ %s\n", PATH);


	//절대 주소 복사 
	strcpy(prelist[compcnt],PATH);
	
	
	//printf("%d에 넣음, mtime: %d 	prelist: %s\n",compcnt ,mtime[compcnt], prelist[compcnt]);
	++compcnt;

        if (!S_ISDIR(fstat.st_mode)){

        }

        if (S_ISDIR(fstat.st_mode))
        {
            getPrelist(items[i]->d_name,prelist,mtime);
        }
    }



    chdir("..");

}



/*while 문 안에서 이전 단계의 배열과 비교해서 파일이 생성됐는지, 삭제됐는지, 변경됐는지 검사하는 함수*/
void log_isCreatedorDeleted(char(*prelist)[BUF_LEN], int *precnt, int mtime[BUF_LEN] ,char *time){

 	int cnt = 0; 
	int ret = 0;


	char replybuf[BUF_LEN];
	int len;
	FILE *fp;
	int curT[BUF_LEN];
	char filelist[BUF_LEN][BUF_LEN];
	int mMtime[BUF_LEN];
	char rfnn[BUF_LEN];
	
	//현재 디렉토리의 상태를 배열에 저장
	getPrelist(eval_Dir, filelist, mMtime);

	ret = compcnt;
	
	
	compcnt = 0; 
	
	//이전과 비교해서 증가했다면 생성
	if(ret > *precnt)  
	{
	
		if((fp = fopen(logDir, "a+"))==NULL){
			fprintf(stderr, "error while reading log.txt\n");
			exit(1);
		}

		//이전배열과 비교했을 때 특정 인덱스의 배열 값이 다르다면, 생성된 값이다 
		for(cnt=0; cnt<ret; cnt++){
			if(strcmp(prelist[cnt], filelist[cnt])){
					break;
			}
		}
		
		relativeP(filelist[cnt], rfnn);
		//sprintf(replybuf, "[%s][create_%s]\n",time,filelist[cnt]); 
		sprintf(replybuf, "[%s][create_%s]\n",time,rfnn);
		memset(rfnn, 0, BUF_LEN);


		len = strlen(replybuf);
	
		//파일내 내용 write
		fwrite(replybuf, 1, len, fp);

		fclose(fp);

		for(int i=0; i<ret; i++){
			memset(prelist[i],0, sizeof(char)*BUF_LEN);
		}

		//현재 배열을 다음 루프에 이전 배열로 만들기 위한 문자열 복사 
		for(int i=0; i<ret; i++){
			len = strlen(filelist[i]);
			memcpy(prelist[i], filelist[i], len);
		}

		++(*precnt);//이전배열보다 상승했기 때문에 증가시킴 
		curState(curT, filelist, ret);//상태가 변화했기 때문에 mtime 한번 더 계산


	}//이전 배열의 상태와  비교해서 하나가 줄어들었다면 삭제된 것임
	else if(ret < *precnt)
	{


	//	printf("&************삭제 ********\n");
	//	sleep(3);
		


		flag = 100;//break될 때  cnt가 삭제한 원소
		for(cnt=0; cnt<*precnt; cnt++){
			if(cnt == *precnt-1 || strcmp(prelist[cnt], filelist[cnt])){
				/** 마지막 까지 for문이 도는 경우: 마지막 삭제되되는 경우**/
				break;
			}	
		}
		
		//로그 파일에 적음 
		if((fp = fopen(logDir, "a+"))==NULL){
			fprintf(stderr, "error while reading log.txt\n");
			exit(1);
		}

		relativeP(prelist[cnt], rfnn);
		//sprintf(replybuf, "[%s][create_%s]\n",time,filelist[cnt]); 
		sprintf(replybuf, "[%s][delete_%s]\n",time,rfnn);
		memset(rfnn, 0, BUF_LEN);


		///sprintf(replybuf, "[%s][delete_%s]\n",time,prelist[cnt]); 
		len = strlen(replybuf);
	
		fwrite(replybuf, 1, len, fp);

		fclose(fp);
	

		//변수 초기화 
		for(int i=0; i<ret+1; i++)
			memset(prelist[i],0, sizeof(char)*BUF_LEN);
		
		//현재 배열을 다음 루프에 이전 배열로 만들기 위한 문자열 복사 
		for(int i=0; i<ret; i++){
			len = strlen(filelist[i]);
			memcpy(prelist[i], filelist[i], len);
		}

		--(*precnt);//삭제했기 때문에 이전상태보다 하나 더 적음 
	
		//curStateforD(curT, filelist, ret, prelist[cnt-1]);
		curState(curT, filelist, ret);	//상태가 변화했기 때문에 mtime 한번 더 계산

	}
	//개수가 같을 때 1.변화x, 2. 수정했을 때 
	else if(ret == *precnt){
		flag = 100;
		
		curState(curT, filelist, ret);	
		
		for(int i=0; i<ret; i++){//최종 수정시간이 이전 루프와 다르다면 수정된 것이다 
			if(mtime[i] != curT[i]){
				relativeP(filelist[cnt], rfnn);
		
				sprintf(replybuf, "[%s][modify_%s]\n",time,rfnn);
	
				memset(rfnn, 0, BUF_LEN);

				len = strlen(replybuf);
				
				logWrite(replybuf);
	
	

			}
		} 

		//변경된 수정 시간을 대입해서 다음 루프때 사용한다 
		for(int i=0; i<ret; i++)
			mtime[i] = curT[i]; 
		return;
	}

	//변경된 수정 시간을 대입해서 다음 루프때 사용한다 
	for(int i=0; i<ret; i++)
		mtime[i] = curT[i]; 
	

}

//로그.txt에 적는 함수
void logWrite(char* logtext){
			
	FILE *fp;
	int len; 

	//파일을 열고 
	if((fp = fopen(logDir, "a+"))==NULL){	
		fprintf(stderr, "error while reading log.txt\n");	
		exit(1);		
	}

	
	len = strlen(logtext);	
	//내용을 적는다 
	fwrite(logtext, 1, len, fp);
	
	fclose(fp);

	printf("%s\n", logtext);

}

//파일의 상태를 파악하는 함수(개수,이름, mtime)
void curState(int *curstat, char (*filelist)[BUF_LEN], int ret ){

	int cnt = 0; 
	//int ret = 0;
	char fDir[BUF_LEN];
       	//struct dirent **filelist;  
	struct stat statbuf[BUF_LEN];
	time_t intertime; 
	

	for(int i=0; i<ret; i++){


		if(flag == 100)//그전에 삭제가 진행됐다면 없는 파일에 대한 스태트 구조체를 구하지 않기 위해 continue
			if(access(filelist[i], F_OK)!=0)
				continue;
		
		//mtime을 위한 stat 
		if(stat(filelist[i], &statbuf[i])<0){
			perror("Error: \n");
			fprintf(stderr, "1) stat error for %s\n", filelist[i]);
			//exit(1);
		}
		//배열에 각 파일들의 mtime을 저장한다 	
		intertime = statbuf[i].st_mtime;
		curstat[i] = intertime; 
			
	}

	return;
		

}

//ssu_mntr.c 에 똑같은 함수가 존재합니다. 
void getCertainDir(){

	DIR * dir_ptr = NULL;
        struct dirent *dir_file = NULL;
        struct stat buf;
        char file_name[BUF_LEN];
        struct dirent *dentry;
        int N;
        struct dirent **item; 



	 memset(saved_path, 0, BUF_LEN);

        //현재 디렉토리
        if(getcwd(saved_path, BUF_LEN)==NULL){
                fprintf(stderr, "getpwd error\n");
                //exit(1);
        }

        N = scandir(saved_path, &item, NULL, alphasort);
       
	for (int i = 0; i < N; i++)
        {
                struct stat fstat;

                //.이나 ..은 무시 
		if ( (!strcmp(item[i]->d_name, ".")) || (!strcmp(item[i]->d_name, "..")) )

                 {
                   continue;
                 }


			
		//파일인지 디렉토리인지 알기 위해 
                 if(stat(item[i]->d_name, &buf)<0){
                         fprintf(stderr, "stat error\n");
                         return;
                 }

                 lstat(item[i]->d_name, &fstat);

		 //특정디렉토리는 trash, 20170771 이 아니어야 한다 
		 if ((fstat.st_mode & S_IFDIR) == S_IFDIR && strcmp(item[i]->d_name, "trash") && strcmp(item[i]->d_name, "20170771"))
                {
                //특정한 디렉토리라면

                        strcpy(check_Dir, item[i]->d_name);//특정 디렉토리의 주소정보 저장

                        break;
                }

        }

        sprintf(eval_Dir, "%s/%s", saved_path, check_Dir);
}

void relativeP(char Fname[BUF_LEN], char fnm[BUF_LEN]){
	char fnamee[BUF_LEN];
	strcpy(fnamee, Fname);
	char *ptr = strrchr(fnamee, '/');
	ptr += 1; 

	strcpy(fnm, ptr);	
}

//scandir 디버깅용 함수 
static int filter(const struct dirent *dirent)
{
     /* 현재 디렉토리, 이전 디렉토리 표시는 출력안함 */
  
	if(!(strcmp(dirent->d_name, ".")) || !(strcmp(dirent->d_name, ".."))){
	}else{
		//printf("   %s() : %s\n", __FUNCTION__, dirent->d_name);
	}
}


