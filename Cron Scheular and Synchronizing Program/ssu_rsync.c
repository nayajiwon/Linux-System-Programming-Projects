#include "ssu_rsync.h"
char logdir[BUFLEN]; 
char curdir[BUFLEN];
char dstdir[BUFLEN]; 
char srcdirstatic[BUFLEN]; 
char copydir[BUFLEN]; 
int srcdirlength;
int dstdirlength; 
char dir[BUFLEN];
int main(int argc, char *argv[]){

	struct stat statbuf;
	struct timeval start_time;
	struct timeval end_time;
	double execution_time;

	gettimeofday(&start_time, NULL);
	 
	ssu_rsync(argc, argv);

	//시그널을 받으면 핸들러 함수 호출
	signal(SIGINT, (void *)handler);

	gettimeofday(&end_time, NULL);

	printf("%f\n", (double)(end_time.tv_usec)-(double)(start_time.tv_usec));

	//커피한 디렉토리 모두 삭제
	removedir(copydir);


}

void removedir(char *path){

	//COPY디렉토리 들어옴

	
	DIR * dp = NULL;
	
	char *ptr1, *ptr2; 
	struct dirent *dentry;
	struct stat statbuf;
	char filename[BUFLEN];
	int ind,p,res,flag;

	if((dp = opendir(path)) == NULL)
	{
		return; 
	}


	while((dentry = readdir(dp))!=NULL)
	{
		
		
		if(strcmp(dentry->d_name,"..")==0 || strcmp(dentry->d_name, ".")==0)
		{
			continue;
		}

		//다음에 탐색할 파일 정보
		sprintf(filename, "%s/%s", path, dentry->d_name);
		

		if(stat(filename, &statbuf) == -1)
			continue;
		
		//파일이라면
		if(S_ISREG(statbuf.st_mode))//일반 파일일때만
		{
			remove(filename);
		}
		else if(S_ISDIR(statbuf.st_mode))
		{
		
			//디렉토리라면 
			
			removedir(filename);
		}
	}

	//지금 있는 디렉토리를 닫고 
	closedir(dp);
	rmdir(path);//삭제

}

	
void handler(int signo){
	
	if(access(copydir, F_OK)<0)
		return; 

	//0. 기존디렉토리를 삭제하고
	//1. 기존디렉토리 이름의 디렉토리를 하나 만들고
	//2. 커피디렉토리안의 파일 및 디렉토리를 다시 복사한다
	//3. 커피디렉토리를 삭제한다 
	removedir(dir);
	mkdir(dir, 0777);
	makethesame(dir, copydir);
	removedir(copydir);

	return;
}


//알싱크 옵션을 가려내기 위한 함수
void ssu_rsync(int argc, char *argv[])
{
	char src[BUFLEN];
	char dst[BUFLEN]; 
	char option[5]; 
	_arr srcarr[BUFLEN]; 
	int srclen; 
	_arr dstarr[BUFLEN]; 
	int dstlen; 
	char onlyfirstdir[BUFLEN];

	char temp[BUFLEN];

	FILE *fp; 

	if(argc == 4)
	{
		strcpy(option, argv[1]); 
		strcpy(src, argv[2]);
		strcpy(dst, argv[3]); 
	}
	else if(argc == 3)
	{
		strcpy(src, argv[1]);
		strcpy(dst, argv[2]); 
	}

	getcwd(curdir, BUFLEN);

        sprintf(logdir, "%s/%s",curdir , "ssu_rsync_log");

	//로그가 없다면 생성하라
	if(access(logdir, F_OK)<0)
        {
                fp = fopen(logdir, "w");
                fclose(fp);
        }

	//파일의 접근권한이 있다면
	isfileOk(src);
	srcdirlength = strlen(src);	
	isfileOk(dst); 
	dstdirlength = strlen(dst);

	//전역변수에 복사
	strcpy(dstdir, dst);
	strcpy(srcdirstatic, src);



	strcpy(temp, src);
	strcpy(onlyfirstdir, dst);

	//디렉토리라면
	//커피 디렉토리를 하나 만들고 
	//현재 디렉토리 이후 가장 최상의 디렉토리까지 받아옴  
	isfileordir(onlyfirstdir);//함수 갔다오면 함수뒤에 커피 붙임

	
		strcpy(temp, onlyfirstdir); 

		strcat(onlyfirstdir,"_<copy>"); 
		mkdir(onlyfirstdir, 0777);

		
		
		makethesame(onlyfirstdir,temp);
	
	char buf[BUFLEN];
	strcpy(buf, dst);

		
	//전역변수
	strcpy(dir, buf);

	//전역변수

	strcpy(copydir, onlyfirstdir);
		
	if(argc == 3)
	{
		
		srclen = 0;
		dstlen = 0;
	
		//배열에 저장하는 함수
		filesync(src, srcarr, &srclen); 
		filesync(dst, dstarr, &dstlen); 
		
		//디버깅용 함수
		//arrcheck(srcarr, srclen);
		//arrcheck(dstarr, dstlen);

		gorsync(srcarr, dstarr, srclen, dstlen);

	}
	//rOption구현 실패
	/*
	else if(argc == 4)
	{
		if(!strcmp(option, "-r"))
		{
			printf("rOption:\n");
			filesync(dst, dstarr, &dstlen); 
			rsyncRoption(src ,dstarr,dstlen);
			arrcheck(dstarr, dstlen);

		}
	}*/
	


}


//디렉토리 재귀를 통해 시그인트를 받았을 때 커피할 디렉토리 정보를 가져오는 함수
void makethesame(char *path, char *srcdir){

	char temp[BUFLEN], dirn[BUFLEN]; 
	char newdir[BUFLEN];
	char bef[BUFLEN], aft[BUFLEN]; 
	DIR * dp = NULL;
	
	char *ptr1, *ptr2; 
	struct dirent *dentry;
	struct stat statbuf;
	char filename[BUFLEN];
	int ind,p,res,flag;

	if((dp = opendir(srcdir)) == NULL)
	{
		return; 
	}

	while((dentry = readdir(dp))!=NULL)
	{
		
		
		if(strcmp(dentry->d_name,"..")==0 || strcmp(dentry->d_name, ".")==0)
		{
			continue;
		}

		//다음 디렉토리로 가기위해 파일명 이어붙ᄋ
		sprintf(filename, "%s/%s", srcdir, dentry->d_name);
		sprintf(newdir, "%s/%s", path, dentry->d_name);
		

		if(stat(filename, &statbuf) == -1)
			continue;
		
		if(S_ISREG(statbuf.st_mode))//일반 파일일때만
		{
			
			makeandcopy_sigint(filename, newdir);
				
		}
		else if(S_ISDIR(statbuf.st_mode))
		{
		
			//디렉토리라면 

			if(access(newdir, F_OK)<0)
			{
				//디렉토리가 존재하지 않는다면
				mkdir(newdir, 0777);//디렉토리 생성
			}
			
			makethesame(newdir, filename);//똑같은 디렉토리를 만듦
		}
	}

	closedir(dp);

}

//시그인트를 받았을 때 동기화 이전상태로 돌리기 위해 데스티네이션 디렉토리 전체를 복사하는 함수
void makeandcopy_sigint(char *src, char *srccpy){

	char srcptr[BUFLEN];
	FILE *fp1;
	FILE *fp2;
	char buf[3],newfname[BUFLEN];
	struct utimbuf utimebuf; 
	struct stat statbuf; 


	strcpy(srcptr, src);
	
	stat(src, &statbuf);

	utimebuf.modtime = statbuf.st_mtime;//utimebuf도 기존의 소스로 변경했고 


	if((fp1 = fopen(src, "r"))<0){
		fprintf(stderr, "fopen err\n");
		exit(1);
	}
	if((fp2 = fopen(srccpy, "w"))<0){//파일을 새로 열어 내용도 복사했고
		fprintf(stderr, "fopen err\n");
		exit(1);
	}
	
	//파일 내용 한바이트씩 복사
	while(fread(buf, 1, 1, fp1)>0){
		if(fwrite(buf, 1, 1, fp2)!=1)
		{
			fprintf(stderr, "error");
			exit(1);
		}
	}
	
	fclose(fp1);
	fclose(fp2);

	//동기화 이전상태로 돌리기위해 이전상태의 엠타임으로 돌림
	if(utime(srccpy, &utimebuf)<0){
		fprintf(stderr, "utime err\n");
		exit(1);
	}
	
	stat(srccpy, &statbuf);


}




//파일인지 디렉토리인지 여부를 확인하는 함수
int isfileordir(char *path){

	int len;
	char *ptr; 
	struct stat statbuf; 

	len = strlen(curdir);
	ptr = path+len+1; 

	
	while(*ptr != '/')
		ptr++; 
	
	*ptr = '\0'; 

	ptr = path;

	
}

/*알옵션 구현 실패
void rsyncRoption(char *path ,_arr *dst, int dstlen){

	char temp[BUFLEN], dirn[BUFLEN]; 
	DIR * dp = NULL;
	struct dirent *dentry;
	struct stat statbuf;
	char filename[BUFLEN];
	int ind,p,res,flag;
	먼저 체크 하는 과정이 필요함 재귀라서 여따 쓰면 안되거든
	stat(path, &statbuf);
	if(s_isreg(statbuf.st_mode))
	{
		//파일이라면 그냥 리턴해줌
		strcpy(arr[*len].arr, path);
		arr[*len].fmtime = statbuf.st_mtime;
		arr[*len].fsize = statbuf.st_size;
		(*len)++;
		printf("len : 	%d\n", *len);
		return; 
	}

	if((dp = opendir(path)) == NULL)
	{
		return -1; 
	}


	while((dentry = readdir(dp))!=NULL)
	{
		
		
		if(strcmp(dentry->d_name,"..")==0 || strcmp(dentry->d_name, ".")==0)
		{
			continue;
		}
		printf("%s\n", dentry->d_name);
		sprintf(filename, "%s/%s", path, dentry->d_name);
		printf("	src: %s\n", filename);

		if(stat(filename, &statbuf) == -1)
			continue;
		
		if(S_ISREG(statbuf.st_mode))//일반 파일일때만
		{
			flag = 0; 
			for(p=0; p<dstlen; p++)
			{
				printf("dst: %s\n", dst[p].arr);
				res = issyncOk(filename, dst[p].arr);
				printf("res: %d\n", res);
				if(res!=1)
				{
					flag = 1; 
					break;
				}

			}

			if(flag == 0)
			{
				//같은 이름의 파일이 아무것도 없어
				//그냥 동기화 시켜도 됨
				printf("같은 이름의 파일이 없어 동기화 시킵니다\n");
				makeandcopy(filename, dst[p].arr);
			}
			else if(flag == 1)
			{
				printf("엠타임이나 사이즈가 다르지만 이름은 같은 파ᄋ이 하나 존재 \n");
				//dst파일을 삭제하고 src 파일로 대체

				printf("%s를 삭제하고 %s로 대체합니다\n", dst[p].arr, filename);
				makeandcopy(filename, dst[p].arr);
			}
	
		}
		else if(S_ISDIR(statbuf.st_mode))
		{
			
			strcpy(temp, filename); 
			filenameExtract(temp);
			sprintf(dirn, "%s/%s", dstdir, temp); 
			printf("%s\n", dirn);

			if(access(dirn, F_OK)<0)
			{
				printf("디렉토리가 존재하지 않아 하나 만듭니다\n");
				//디렉토리가 존재하지 않는다면
				mkdir(dirn, 0777);
			}
			
			rsyncRoption(filename,dst,dstlen);
		}
	}

	closedir(dp);

}
*/
	

void filenameExtract(char file[BUFLEN]){

	
	

	char *ptr; 
	int len;
	len = strlen(file); 
	
	ptr = file + len; 
	while(*ptr!='/'){
		ptr--; 
	}
	ptr++; 
	strcpy(file, ptr);



}
int issyncOk(char *src, char *dst){
	struct stat statbuf;
	time_t srcMtime; 
	size_t srcSize;
	char srcptr[BUFLEN];
	char *dstptr;


	
	strcpy(srcptr, src);
	filenameExtract(srcptr);

	dstptr = dst+dstdirlength+1; 
	
	//이름이 다를 때 
	if(strcmp(srcptr, dstptr))
	{
		return 1; //동기화 해라
	}
	
	//이름이 같을 때
	
	stat(src, &statbuf);
	srcMtime = statbuf.st_mtime; 
	srcSize = statbuf.st_size;
	 
	stat(dst, &statbuf);

	if(srcMtime != statbuf.st_mtime)
	{
		return -1; 
	}

	if(srcSize != statbuf.st_size)
	{
		return -1; 
	}

	return 0; //완전히 같은 파일
}

//파일의 동기화 가능성에 따라 동기화여부를 판단하고, 실행
void gorsync(_arr *src, _arr *dst, int srclen, int dstlen){

	int i, p, res; 
	int flag; 
	char buf[BUFLEN][BUFLEN];
	int count =0;
	for(i=0; i<srclen; i++)
	{

		flag = 0;
		for(p=0; p<dstlen; p++)
		{
			res = issyncOk(src[i].arr, dst[p].arr);
			if(res!=1)//파일이 같지 않을 때 
			{
				if(res == 0)
					flag = 2; 
				if(res == -1)
					flag = 1; 
				
				break;
			}

		}

		if(flag == 0)
		{
			//같은 이름의 파일이 아무것도 없어
			//그냥 동기화 시켜도 됨
			makeandcopy(src[i].arr, dst[p].arr);
			strcpy(buf[count], src[i].arr); count++; 
		}
		else if(flag == 1)
		{
			//dst파일을 삭제하고 src 파일로 대체

			makeandcopy(src[i].arr, dst[p].arr);
			strcpy(buf[count], src[i].arr); count++; 
			
		}
		else if(flag == 2){
		}
	}

	if(count > 0)//로그파일 찍기
		writeinlogwithTime(buf, count);
	
}
//형식에 따라 로그파일 찍는 함수
void writeinlogwithTime(char (*buf)[BUFLEN],int cnt){

	struct stat statbuf;
	int temp[BUFLEN]; 
	char imsi[BUFLEN], curTime[BUFLEN]; 
	char *curweek, *curmonth;
	FILE *fp; 
	char *ptr1;
	char *ptr2; 
	struct tm Tm;
        time_t current_time;
	char *week[7] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};
        char *month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	current_time = time(NULL);
	localtime_r(&current_time, &Tm);
	
	curweek = week[Tm.tm_wday];
        curmonth = month[Tm.tm_mon]; 

	sprintf(curTime, "[%s %s %02d %02d:%02d:%02d %04d] ", curweek, curmonth, Tm.tm_mday, Tm.tm_hour, Tm.tm_min, Tm.tm_sec, Tm.tm_year+1900);


        //형식에 맞는 시간 


	fp = fopen(logdir, "a");
	
	ptr1 = srcdirstatic + strlen(curdir) + 1; 
	ptr2 = dstdir + strlen(curdir) + 1; 
	

	sprintf(imsi, "%s ssu_rsync %s %s\n",curTime, ptr1, ptr2); 
	fputs(imsi,  fp);

	memset(imsi, 0, BUFLEN); 
	
	for(int i=0; i<cnt; i++){
		stat(buf[i], &statbuf);
		temp[i] = statbuf.st_size; 
	}
	for(int i=0; i<cnt; i++)
	{
		ptr1 = buf[i] + strlen(srcdirstatic)+1; 

		sprintf(imsi, "		%s %dbytes\n", ptr1,temp[i]); 
		fputs(imsi,  fp);
		memset(imsi, 0, BUFLEN); 
	}

	for(int i=0; i<cnt; i++)
	{
			
	}
	fclose(fp);
}

//파일을 만들고 안의 내용을 복사하는 함수
void makeandcopy(char *src, char *dst){

	char srcptr[BUFLEN];
	FILE *fp1;
	FILE *fp2;
	char buf[3],newfname[BUFLEN];
	struct utimbuf utimebuf; 
	struct stat statbuf; 


	strcpy(srcptr, src);
	filenameExtract(srcptr);
	

	sprintf(newfname, "%s/%s", dstdir, srcptr);
	

	stat(src, &statbuf);

	utimebuf.modtime = statbuf.st_mtime;//utimebuf도 기존의 소스로 변경했고 



	fp1 = fopen(src, "r");
	fp2 = fopen(newfname, "w");//파일을 새로 열어 내용도 복사했고
	
	//한바이트ᄊᆔᆨ 복사
	while(fread(buf, 1, 1, fp1)>0){
		fwrite(buf, 1, 1, fp2);
	}
	
	fclose(fp1);
	fclose(fp2);


	if(utime(newfname, &utimebuf)<0){
		fprintf(stderr, "utime err\n");
		exit(1);
	}
	
	stat(newfname, &statbuf);


}
/*
void arrcheck(_arr *arr, int len){

		printf("총: %d개 \n",len);
		for(int i=0; i<len; i++){
			printf("arr[%d]=> %s\n", i, arr[i].arr);
			printf("mtime[%d]=> %d\n", i, arr[i].fmtime);
			printf("size[%d]=> %d\n", i, arr[i].fsize);
			printf("=============\n");
		}

		printf("어레이 체크 finished**************************\n");
		printf("\n\n");


}
*/

//주어진 디렉토리를 탐색하며 엠타임, 크기, 이름을 구조체에 저장하는 함수
void filesync(char *path, _arr arr[BUFLEN], int *len){

	DIR * dp = NULL;
	struct dirent *dentry;
	struct stat statbuf;
	char filename[BUFLEN];
	int ind; 

	stat(path, &statbuf);
	if(S_ISREG(statbuf.st_mode))
	{
		//파일이라면 그냥 리턴해줌
		strcpy(arr[*len].arr, path);
		arr[*len].fmtime = statbuf.st_mtime;
		arr[*len].fsize = statbuf.st_size;
		(*len)++;
		return; 
	}

	if((dp = opendir(path)) == NULL)
	{
		return; 
	}

	//파일을 하나씩 컴색한다

	while((dentry = readdir(dp))!=NULL)
	{
		
		
		if(strcmp(dentry->d_name,"..")==0 || strcmp(dentry->d_name, ".")==0)
		{
			continue;
		}
		sprintf(filename, "%s/%s", path, dentry->d_name);

		if(stat(filename, &statbuf) == -1)
			continue;
		
		//디렉토리는 무시
		if(S_ISREG(statbuf.st_mode))//일반 파일일때만
		{
			//엠타임, 이름, 사이즈 저장

			strcpy(arr[*len].arr, filename);
			arr[*len].fmtime = statbuf.st_mtime;
			arr[*len].fsize = statbuf.st_size;
			(*len)++;
		}
	}

	closedir(dp);

	


	
}


//파일의 접근 권한을 확인하여 접근권한이 없는 경우 usage  를 출력하는 함수
int isfileOk(char *path){

	char *ptr; 
	char buf[BUFLEN]; 
	if(access(path, F_OK)<0 || access(path, W_OK)<0 || access(path, R_OK)<0)
	{
		printf("usage");
		exit(1); 
	}


	realpath(path, buf);
	
	strcpy(path, buf);
}







