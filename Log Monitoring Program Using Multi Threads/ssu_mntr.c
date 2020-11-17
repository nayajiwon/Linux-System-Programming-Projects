//디몬프로세스 헤더
		
#include "ssu_mntr.h"
//#include "ssu_size.c"
//#include "ssu_recover.c"
//#include "ssu_tree.c"


#include <time.h>
//extern char check_Dir[BUF_LEN];
char check_Dir[BUF_LEN];
char saved_path[BUF_LEN];
char eval_Dir[BUF_LEN];

char InfoDir[FILE_LEN];
char trashAddr[BUF_LEN];
char FileDir[FILE_LEN];

int main()
{
	DIR * dir_ptr = NULL;
	struct dirent *dir_file = NULL;
	struct stat buf;
	char file_name[BUF_LEN];
	struct dirent *dentry;
	int N;
	struct dirent **item; 

	memset(saved_path, 0, BUF_LEN);

	//현재 디렉토리의 주소  정보를 저장
	if(getcwd(saved_path, BUF_LEN)==NULL){
		fprintf(stderr, "getpwd error\n");
		exit(1);
	}

	//디렉토리 안의 파일들을 저장
	N = scandir(saved_path, &item, NULL, alphasort);
	
	sprintf(trashAddr, "%s/%s", saved_path, "trash");//trash디렉토리의 절대주소
	sprintf(InfoDir, "%s/%s", trashAddr, "info");//info디렉토리의 절대주소
	sprintf(FileDir, "%s/%s", trashAddr, "files");//files디렉토리의 절대 주소 
	
	
	//특정(지정) 디렉토리를 찾기위한 for문 
	//명세상 디렉토리이며 trash 디렉토리이며 학번 디렉토리가 아니면 지정한 디렉토리
	for (int i = 0; i < N; i++) 
    	{   
        	struct stat fstat; 

		//현재 또는 이전 디렉토리임을 나타내는 .은 건너뜀
       		 if ( (!strcmp(item[i]->d_name, ".")) || (!strcmp(item[i]->d_name, "..")) ) 
        
		 {
         	   continue; 
        	 }
    		 

		//현재 파일의 상태를 알아냄 
		 if(stat(item[i]->d_name, &buf)<0){
			 fprintf(stderr, "stat error\n");
			 return -1;   
		 }

		//현재 파일의 상태를 알아냄 
		 lstat(item[i]->d_name, &fstat);        
		
		 
		//명세상 디렉토리이며 trash 디렉토리이며 학번 디렉토리가 아니면 지정한 디렉토리
		 if ((fstat.st_mode & S_IFDIR) == S_IFDIR && strcmp(item[i]->d_name, "trash") && strcmp(item[i]->d_name, "20170771")) 
                {
                //특정한 디렉토리라면
                        strcpy(check_Dir, item[i]->d_name);//특정 디렉토리의 이름을 extern변수에 저장  
			break;
                }

	}

	sprintf(eval_Dir, "%s/%s", saved_path, check_Dir);//특정 디렉토리의 절대주소를 extern변수에 저장 

	while(1){
	
		printf("20170771>");
	
		if(checkOption()<0){
			helpme();
				
				//printf("help명령어  \n");
			/*** 
			  help 명령어 만들기	  ***/
			
		}
	}

}
//return값 설정해서 그 외의 명령어가 나오면 help명령어를 찍게끔 
int checkOption()
{
	char input[BUF_LEN];
	char option[OP_LEN][OP_LEN];
	char rPath[BUF_LEN];
	int i;
	int p=0;
	int n=0;


	fgets(input, BUF_LEN, stdin);//한줄 입력받고 버퍼에 저장함
	//while(getchar()!='\n');//버퍼지우기
	
	p = strlen(input)-1;//개행 없애기 위한 Null문자 삽입   
	input[p] = '\0';
	

	//토큰 단위로 나누는 while문 
	char *ptr = strtok(input, " ");

	while(ptr != NULL){
		strcpy(option[n++], ptr);
		ptr = strtok(NULL, " ");
	}
	

	//delete 옵션이 들어왔을 때
	if(!strcmp(option[0], "delete")){
		strcpy(rPath, option[1]);
		
		if(n<2){//개수가 1개이하라면 삭제 할 파일의 입력이 없기때문에 에러처리
			fprintf(stderr,"error: 삭제 할 파일의 입력이 없습니다\n");
			return 1;
		}

		drepath(rPath);//상대주소인지 절대주소인지 판단하고 절대주소로 바꿔줌 
			
		ddelete(option, n, rPath);
	
	}
	else if(!strcmp(option[0], "size")){
		if(n == 2)//입력한 파일의 전체 사이즈 
			getDIRsize(option[1], "no"); 
		else if(n == 4){ 
		
			if(strcmp(option[2],"-d")){//d옵션 외에 들어오면 예외처리
				fprintf(stderr ,"error: size는 -d 옵션만 가능합니다\n");
				return 1; 
			}

			getDIRsize(option[1], option[3]);//각각의 레벨의 파일 사이즈 
	
		}
	}
	else if(!strcmp(option[0], "recover")){
		if(n == 2)//그냥 복귀
			recover(option[1]);
		else if(n == 3) //옵션 제공 
			printLoption(option[1]);
		else{
			fprintf(stderr, "error: recover 의 형식에 맞는 입력이 아닙니다\n"); 
			return 1;
		}
			
	}
	else if(!strcmp(option[0], "tree")){
		Tree();
	}
	else if(!strcmp(option[0], "exit")){
		exit(1);
	}
	else if(!strcmp(option[0], "help")){
		helpme();
	}
	//명세에 주어진 명령어 외에 다른 명령어를 쳤다면 help명령어를 호출한다. 
	else 
		return -1;
	//	else if((strcmp(option[0], "delete")) && (strcmp(option[0], "size"))&& (strcmp(option[0], "recover")) && (strcmp(option[0], "tree")) && (strcmp(option[0], "exit")))   
//	{
//			helpme();
//	}
	return 1;

}


void drepath(char path[BUF_LEN]){
		 
	char Exple[BUF_LEN];
        char Eval[BUF_LEN];
        char Eval2[BUF_LEN];
        char temp[BUF_LEN];
 	int len; 
	int flag=0;
	int i = 0;

	strcpy(Exple, eval_Dir);//특정디렉토리 절대주소
        strcpy(Eval, path);//파일 
        strcpy(Eval2, path);//파일 

	len = strlen(eval_Dir); //특정 디렉토리의 절대주소만큼의 길이 
	
	for(i=0; i<len; i++){ //절대주소의 길이보다 작으면 상대주소로 간주 
		if(eval_Dir[i] != path[i])
			break; 
	}
	
	if(i == len){//절대경로
	}
	
	if(i < len)//상대경로 
	{

		char * ptr = strtok(Eval, "/"); 
		if(!strcmp(ptr, ".")){ // " ./특정디렉토리 " 이름 으로 주어졌을 때 
			ptr = Eval2;
			ptr += 2;
			strcpy(Eval2, ptr);//이름만 추출 
		}

		//특정디렉토리 절대주소 이후에 상대주소를 연결 -> 절대주소 완성 
		strcpy(temp, eval_Dir);
		strcat(temp, "/"); 
		strcat(temp, Eval2);

		strcpy(path, temp); 
	}


	return; 
	

}

void ddelete(char option[OP_LEN][OP_LEN], int cnt, char rPath[BUF_LEN]){
	
	int test = 0; 
	pthread_t thread;
	int diOption; 
	int drOption;
	
	threadOptions thread_Op; 


	
	char ddir[FILE_LEN];
	char dEndTime[OP_LEN];
	char dOp[OP_LEN];

	char cur_dir[FILE_LEN];
	char *dFname;
	char dDir[FILE_LEN];
	
	
	char reserveTime[FILE_LEN];
	char curT[FILE_LEN];	
	pid_t pid; 

	char Filename[FILE_LEN];
	char dreserve[BUF_LEN];

	int lastnum;
	char imsi[BUF_LEN];
	char imsi2[3];

	/** 1.trash디렉토리 존재 여부 확인 **/ 
	
	//trash 디렉토리가 없으면 생성하라 

	
	if(access(trashAddr, F_OK) < 0)
		d_makeTrashDir(trashAddr);	
		
	strcpy(ddir, rPath);//삭제 할 디렉토리 절대주소 복사 

	if((dFname = strrchr(ddir, '/'))==NULL) 
		//dFname: null
		strcpy(dDir, ddir);//삭제할 파일 이름 복사 
	else{ 
		//dFname: 파일 이름 
		dFname++;//'/'는 포함하지 않는다
		strcpy(dDir, dFname);
	}
	//dDir: 삭제하고싶은 파일 이름
	
	//ddir: 삭제하고 싶은 파일 경로
	
	//파일이나 디렉토리가 존재하지 않을 경우 에러처
	
	
	
	if(access(ddir, F_OK) < 0){
		fprintf(stderr, "삭제 할 파일이 존재하지 않습니다\n");
		return;
	}
	
	/*** 입력 두개, 바로 삭제해야 할 경우***/ 
	if(cnt <= 2){
		currentTime(curT);//현재 시간 

		lastnum = checkSameFname(InfoDir, dDir); //동일한 이름의 파일을 세고 최종 번호를 리턴하여 파일 이름 앞에 x_num 형식으로 만듬 
		//형식에 맞게 만
		sprintf(imsi2, "%d", lastnum);
		strcpy(imsi, imsi2);
		strcat(imsi, "_");
		strcat(imsi, dDir);
	
		toTrashInfo(ddir, trashAddr, imsi, curT);//info에 rename
		toTrashFiles(ddir, trashAddr, imsi);//trash에 rename
	
		dinfoOver2kb();//info 디렉토리가 2Kb가 넘는지 확인하는 함수 
		return;  		
	}

	
	//시간일 경우 
	if(d_isTime(option[2])){
		
		sprintf(reserveTime, "%s %s", option[2], option[3]); 
		
		//thread함수에 보내기 위한 구조체 초기화 
		thread_Op.iOption = 0; //iOption 인지
		thread_Op.rOption = 0; //rOption인지
		thread_Op.nOption = 0; //아무 옵션 없을 때 

		strcpy(thread_Op.rtime, reserveTime);//예약시간
		strcpy(thread_Op.Dir, ddir);//삭제할 파일 절대주소
		strcpy(thread_Op.trashDir, trashAddr);//trash절대주소
		strcpy(thread_Op.InfoDir, InfoDir);//info절대주소
		strcpy(thread_Op.dFname, dDir);//삭제할 파일 이름 

//		printf("%s\n", thread_Op.Dir);

		//iOption일 때 
		if(!strcmp(option[4], "-i")){
			thread_Op.iOption = 1; 
		}
		//rOption일 때
		else if(!strcmp(option[4], "-r")){
			thread_Op.rOption = 1; 
		}
		//아무 옵션도 주어지지 않았을 때 
		else{
			thread_Op.nOption = 1;
		}
		
		//옵션 초기화 
		memset(option[4], 0, BUF_LEN);

		//thread 시작!
		pthread_create(&thread, NULL,dreserve_thread, (void**)&thread_Op); 
		
		//메모리 분리 
		pthread_detach(thread);
			
		return;
			
		
	}
	//-i옵션일 경우 
	else if(d_isTime(option[2])==0){

		struct stat fstat;

		lstat(ddir, &fstat);//상태 
		
		//rOption일 때 
		if(!strcmp(option[2], "-r")){
			fprintf(stderr, "-r 옵션을 받기 위해 예약 시간을 먼저 설정하셔야 합니다\n");
			return;
		}

		//iOption일 때 디렉토리 삭제 불가 
		if (S_ISDIR(fstat.st_mode)){
    			fprintf(stderr, "i옵션은 디렉토리를 삭제할 수 없습니다\n");
        		return;
		}

		//옵션이 항상 마지막 토큰에 있어여 함
		if(cnt>3){
			fprintf(stderr, "%s 옵션 뒤에 인자가 와서는 안됩니다 \n", option[2]);
			return;
		}

		//바로 삭제 
		remove(ddir);
	
		return; 
	}
	       	
}



void* dreserve_thread(void*arg){
	struct stat fstat;
	char curTime[BUF_LEN];
	char curTime_tilsec[BUF_LEN];
	struct tm Tm; 
	time_t current_time;
	char ans[2];
	int len; 
        int lastnum;
        char imsi[BUF_LEN];
        char imsi2[3];
	int i=0; 
	
	current_time = time(NULL);//현재시간 

	pthread_t tid; 

	tid = pthread_self();

	threadOptions *tO = (threadOptions *)arg;//main에서 thread 로 받은 변수들의 구조체
	
	
	//현재시간을 계속 검사하여 입력한 예약시간과 문자열이  같은 순간 Break하여 삭제를 진행한다 
	while(1){

		memset(curTime, 0, BUF_LEN);
		current_time = time(NULL);

		localtime_r(&current_time, &Tm);//시간을 명세에 따른 구조로 바꾼다 

		//0000-00-00 00:00 의 구조를 반드시 따라야 한다 
		sprintf(curTime,"%04d-%02d-%02d %02d:%02d",Tm.tm_year + 1900, Tm.tm_mon + 1, Tm.tm_mday, Tm.tm_hour, Tm.tm_min);
		sprintf(curTime_tilsec,"%04d-%02d-%02d %02d:%02d:%02d",Tm.tm_year + 1900, Tm.tm_mon + 1, Tm.tm_mday, Tm.tm_hour, Tm.tm_min, Tm.tm_sec);

		//예약시간과 현재 시간이 같을경우 Break
		if(!strcmp(curTime, tO->rtime)){
			break; 
		}
	
	}


	//iOption: trash에 정보를 저장하지 않고 삭제함 
	if(tO-> iOption){
		//iopion 의 대상인 디렉토리의 상태 
		lstat(tO->Dir, &fstat);

		//디렉토리라면 remove 를 사용 불가
		if (S_ISDIR(fstat.st_mode)){
    			fprintf(stderr, "i옵션은 디렉토리를 삭제할 수 없습니다\n");
			pthread_exit((void *)0);
        	}

		remove(tO->Dir);
		
		//thread 종
		pthread_exit((void *)0);
	}
	else if(tO->rOption){ //rOption일 경우 
		while(1){
			printf("Delete [y/n]? ");
			
			fgets(ans, sizeof(ans), stdin);//답을 입력받음 
			while(getchar()!='\n');//버퍼지우기
			
			len = sizeof(ans) / sizeof(int);
			
			ans[len-1] = '\0'; //개행 없애기

			if(!strcmp(ans, "n"))//n일때 thread 종료 
				pthread_exit((void *)0);
			
			if(!strcmp(ans,"y")){//Y일때 파일 삭제 하기위한 Break
				break;
			}

			else
				fprintf(stderr,"n과 y중에 고르셔야 합니다 \n"); 
			
		}
	}

	//***현재시간을 info에 기입하기 위해 형식에 맞는 현재시간을 구합니다 
	memset(curTime, 0, BUF_LEN);
	current_time = time(NULL);

	localtime_r(&current_time, &Tm);

	//형식에 맞는 시간 
	sprintf(curTime,"%04d-%02d-%02d %02d:%02d",Tm.tm_year + 1900, Tm.tm_mon + 1, Tm.tm_mday, Tm.tm_hour, Tm.tm_min);
	sprintf(curTime_tilsec,"%04d-%02d-%02d %02d:%02d:%02d",Tm.tm_year + 1900, Tm.tm_mon + 1, Tm.tm_mday, Tm.tm_hour, Tm.tm_min, Tm.tm_sec);
		
	//현재 trash 에 있는 동일한 이름의 파일 의 개수를 리턴함(_제외한 이름) 
	lastnum = checkSameFname(InfoDir, tO->dFname); 

	//x_num.txt 형식으로 만듦
        sprintf(imsi2, "%d", lastnum);
        strcpy(imsi, imsi2);
        strcat(imsi, "_");
        strcat(imsi, tO->dFname);


	//info디렉토리에 rename
	toTrashInfo(tO->Dir, tO->trashDir, imsi, curTime);
	//files디렉토리에 rename
	toTrashFiles(tO->Dir, tO->trashDir, imsi);

	//info디렉토리가 2kb가 넘었다면 삭제하는 함수 
	dinfoOver2kb();

	pthread_exit((void *)0);
	
}


/***주어진 형식에 맞게 현재 시간을 구하는 함수***/
//0000-00-00 00:00 
void currentTime(char curtime[FILE_LEN]){
        char chtime[BUF_LEN];
        struct tm* Tm;
        time_t T = time(NULL);//현재시간 
        Tm = localtime(&T);//의 형식

        sprintf(chtime,"%04d-%02d-%02d %02d:%02d:%02d",Tm->tm_year + 1900, Tm->tm_mon + 1, Tm->tm_mday, Tm->tm_hour, Tm->tm_min, Tm->tm_sec);

        strcpy(curtime, chtime);

        return;



}


/** return = 0 날짜와 옵션 중 옵션 
 * return = 1 날짜와 옵션 중 날짜 
 * 시간인지 옵션인지 판단하는 함수 
 * 시간일 경우 2020년보다 작을경우 에러처리함 
 */
int d_isTime(char endTime[BUF_LEN]){
	char chtime[FILE_LEN];
	char *ptr2;

	strcpy(chtime, endTime);
	
	//뒤에서 부터 -이후의 문자열을 가리킴
	char *ptr1 = strrchr(chtime, '-');
	
	//-r, -i 옵션일 경우
	if(ptr1 != NULL)
	{
		if(!strcmp(ptr1, "-r")){
			fprintf(stderr, "r옵션을 사용하기 위해서는 삭제시간을 예약하셔야 합니다\n");
			return -1; 
		}
		if(!strcmp(ptr1, "-i"))
			return 0; 

	}

	strcpy(chtime, endTime); 

	//앞에서부터 -이전의 문자열을 가리킴
	ptr1 = strtok(chtime, "-");

	if(ptr1 == NULL){
		printf("	잘못된 입력을 받았습니다 \n");
		return -1;
	}

	
	//예약 년도가 2020보다 작을경우 
	//숫자로 바꾸지 못할경우 0 
	if(atoi(ptr1)<2020){
		fprintf(stderr, "잘못된 입력을 받았습니다 \n");
		return -1;	
	}

	return 1;	
}


/** trash디렉토리 안의 files, info 디렉토리를 만드는 함수***/ 
void d_makeTrashDir(char trash[FILE_LEN]){	
	
	mkdir(trash, 0755); 
		
	 //trash디렉토리 안에 다른 디렉토리 생성 
	  //trash디렉토리로 주소를 옮긴 뒤 하위 디렉토리(files, info)를 만든다 
	if(chdir(trash)<0){
		fprintf(stderr,"working on the Trash Dir failed\n");
		return;
	}

	if(mkdir("files", 0755)<0){
		fprintf(stderr,"working on the Trash Dir/files failed\n");
		return;
	}
		
	if(mkdir("info", 0755)<0){
		fprintf(stderr,"working on the Trash Dir/info failed\n");
		return;
	}		
		
	//복귀 
	if(chdir(saved_path)<0)
		return;

}

	
/****trash/info에 rename 하는 함수*****/ 
void toTrashInfo(char* Dir, char *trashDir ,char *dFname, char *dTime){
     char imsi[BUF_LEN];

	FILE *fp;
	char InfoDirsFile[BUF_LEN];
	char ddDir[BUF_LEN];
	char D_dTime[BUF_LEN];
	char M_mTime[BUF_LEN];
	char M_mtime[BUF_LEN];
////	struct stat statbuf[BUF_LEN];
	struct stat statbuf;
	time_t mT; 
	struct tm Tm; 
	int len; 
	

	sprintf(InfoDirsFile, "%s/%s/%s", trashDir, "info", dFname);//생성될 info파일의 절대 주소 
	sprintf(ddDir, "%s", Dir);
	sprintf(D_dTime, "D : %s", dTime);

	//삭제 할 디렉토리의 상태
	if(stat(Dir, &statbuf)<0){
		fprintf(stderr ,"stat error for %s\n", Dir);
		exit(1);
	}

	//mtime을 지정한 형식으로 변수에 저장 
	mT = statbuf.st_mtime;
	localtime_r(&mT, &Tm);
	sprintf(M_mtime,"%04d-%02d-%02d %02d:%02d:%02d",Tm.tm_year + 1900, Tm.tm_mon + 1, Tm.tm_mday, Tm.tm_hour, Tm.tm_min, Tm.tm_sec);	
	sprintf(M_mTime, "M : %s\n", M_mtime);

	//동일한 이름의 파일을 Info에 생성 후 정보를 적음 
        if((fp = fopen(InfoDirsFile, "a+"))==NULL){   
      
		fprintf(stderr, "error while reading log.txt\n");    
                exit(1);    
      
	}


	//주어진 형식대로 기
	fputs("[Trash info]\n",fp); 

	fputs(ddDir, fp);

	fputs("\n", fp);

	fputs(D_dTime, fp);

	fputs("\n", fp);

	fputs(M_mTime, fp);
	
	fclose(fp);

}	



//Dir: 버릴 파일디렉토리 , trashDir:trash 절대 주소  , dFname: 버릴 파일 이름 
int toTrashFiles(char *Dir, char *trashDir, char *dFname){
	
	char filesDir[FILE_LEN];

	//rename이 될 절대주소
	sprintf(filesDir, "%s/%s/%s", trashDir, "files", dFname);
	
	//버릴 디렉토리를 rename
	if(rename(Dir, filesDir)!=0){
		fprintf(stderr, "error: 파일 삭제 오류가 생겼습니다\n"); 
		return -1; 
	}

	return 1; 

}
//add: files 디렉토리
//ddir:삭제할 파일 이름  
/**
  동일한 이름을 가진 파일이 존재하나 확인하고 존재한다면 파일개수1,
  존재하지 않는다면 0을 리턴*/
int checkSameFname(char *add, char *dfname){
	

	DIR * dir_ptr = NULL;
        struct dirent *dir_file = NULL;
        struct stat buf;
        char file_name[BUF_LEN];
        struct dirent *dentry;
        int N;
        struct dirent **item; 
	char temp[BUF_LEN];
	int len; 
	int flag = 0; 

	char dfN[BUF_LEN];
	char num[3] = "0";

	//디렉토리의 모든 파일들
	//info디렉토리이기 때문에 재귀를 돌릴 디렉토리가 없슴(파일로만 이루어져 있음)
	N = scandir(add, &item, NULL, alphasort);

        for (int i = 0; i < N; i++)
	{	
	
		len = strlen(temp);
		memset(temp, 0, len);
		strcpy(temp, item[i]->d_name);

		if ( (!strcmp(item[i]->d_name, ".")) || (!strcmp(item[i]->d_name, "..")) )

                 {
                   continue;
                 }


		//x_A.txt 의 형식에 따라 _뒤에 이름을 가리킴	
		char *ptr1 = strrchr(temp, '_');
		ptr1 += 1;//_제외  

		if(!strcmp(ptr1, dfname)){//동일한이름이 이미 존재한다면 앞의 번호가 몇번인지 알아냄 
			flag = 1;
			char *ptr2 = strtok(temp, "_");//_앞의 번호를 가리킴 
			len = strlen(num);
			memset(num, 0, len);//초기화 
			strcpy(num, ptr2); 
		}

		
	}
	if(flag == 0)//동일한 파일의 개수가 없다면 0을 리턴 
		return 0; 
		
	return atoi(num)+1;  //동일한 파일의 개수을 리턴 
}

/**info 가 2kb가 넘을시 삭제하는 함수**/
void dinfoOver2kb(){

	
	int total_size = 0; 
	int ind;
	int cnt = 0; 
	char infoD[BUF_LEN];
	char filesD[BUF_LEN];
	comp compare[BUF_LEN];

	//info에서 삭제시간이 오래된 순서대로 compare 구조체에 받아온다 
	//ind: 개수 
	ind = getDtimeinfo(compare);

	for(int i=0; i<ind; i++)
	{
		struct stat fstat;
		memset(infoD, 0, BUF_LEN);
		memset(filesD, 0, BUF_LEN);
		sprintf(infoD, "%s/%s", InfoDir, compare[i].fname);
		
		lstat(infoD, &fstat);

		//info의 총 사이즈를 갱신
		total_size += fstat.st_size;

		//2000바이트가 넘는다면 넘지 않을 때 까지 info에서 오래된 파일 순으로 삭제한다 
		while(total_size>2000){				

			memset(infoD, 0, BUF_LEN);
			memset(filesD, 0, BUF_LEN);
			sprintf(infoD, "%s/%s", InfoDir, compare[cnt].fname);//compare 의 앞 인덱스부터 오래된 파일이기 때문에 cnt = 0; 부터 삭제
			sprintf(filesD, "%s/%s", FileDir, compare[cnt].fname);					
			
			if((remove(infoD))<0){//info 파일 삭제
				fprintf(stderr, "remove 'info' failed\n");
				exit(1);
			}
				
			if((remove(filesD))<0){//files 파일 삭제 
				fprintf(stderr, "remove 'files' failed\n");
				exit(1);
			}
			
			//총 사이즈에서 삭제한 만큼 빼줌 
			total_size -= fstat.st_size;		

			++cnt;//다음 파일 삭제하기 위한 ++
	
		}

	}

	return; 

}

void helpme(){
	//printf("**********thread 로 컴파일 해야합니다!!!*************\n");
	printf("\n	모든 모니터링은 특정 디렉토리를 대상으로 진행합니다\n"); 

	printf("delete\n 1.delete [FILENAME] 시간예약없이 바로 삭제합니다 \n 2.delete[FILENAME] -i: 파일을 trash에 저장하지 않고 삭제합니다\n	(**주의**디렉토리는 삭제할 수 없습니다)\n 3.delete[FILENAME][ENDTIME]: 주어진 시간에 파일을 삭제합니다\n	(**주의** 시간입력시 반드시 0000-00-00 00:00 의 포멧을 지키셔야 합니다!!! ex)2020-05-13 03:08 )\n4.delete[FILENAME][ENDTIME] -i: 주어진 시간에 파일을 trash에 저장하지 않고 삭제합니다\n	(**주의**디렉토리는 삭제할 수 없습니다)\n\n 5.delete [FILENAME][ENDTIME] -r: 주어진 시간에 삭제시 삭제여부를 한번 더 묻습니다\n	(***실패.. ** 이유: 버퍼문제 같지만 해결하지 못했습니다. scanf, fgets, fet, while(개행지우기) , getchar(); , 모든 경우의 수로 해봤지만 입력을 n, y의 입력을 받으면 바로 help명령어가 나옵니다(main으로 돌아감)\n");


	
	printf("\nsize\n 	(**주의** size 명령어의 상대경로의 기준은 실행 디렉토리 이며 특정 디렉토리는 실행 디렉토리에 있어야 합니다  \n");
	printf(" 1.size [DIRECTORY] : [DIRECTORY](혹은 파일) 의 사이즈를 출력합니다\n 2.size[DIRECTORY] -d x : x 깊이 만큼에 있는 각각의 디렉토리와 파일들의 사이즈를 측정합니\n");
	printf("\nrecover\n");
	printf(" 1.recover [FILENAME]: trash디렉토리 안에 있는 파일을 원래 경로로 복구합니다\n 동일한 이름이 여러개 인 경우 _로 구분합니다\n");
	printf("	(**주의**사용자가 파일 이름을 저장 할 때 _ 절대 금지!) \n");
	printf("이름이 중복되는 경우 넘버링을 해주면 사용자가 복구시킬 파일을 선택합니다 \n"); 
	printf(" 2.recover [FILENAME][OPTION] : trash디렉토리 안에 있는 파일을 원래 경로로 복구합니다.\n");
	printf("또한 trash밑에 있는 파일을 삭제 시간이 오래된 순으로 출력합니다\n"); 
	printf("tree\n특정 디렉토리의 구조를 트리 형태로 보여줍니다\n"); 
	printf("exit\n프로그램을 완전히 종료합니다\n");
	printf("help\nhelp명령어를 출력합니다\n");
	printf("주어진 명령어 외에 다른 명령어를 입력하시면 help가 출력됩니다\n");


}


