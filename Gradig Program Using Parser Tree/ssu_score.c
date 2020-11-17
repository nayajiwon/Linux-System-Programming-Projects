#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "ssu_score.h"
#include "blank.h"

extern struct ssu_scoreTable score_table[QNUM];
extern char id_table[SNUM][10];

//.mOption
extern struct modify_scoreTable modified_table[QNUM];
struct modify_scoreTable modified_table[QNUM];

struct ssu_scoreTable score_table[QNUM];
char id_table[SNUM][10];

char stuDir[BUFLEN];
char ansDir[BUFLEN];
char errorDir[BUFLEN];
char threadFiles[ARGNUM][FILELEN];
char cIDs[ARGNUM][FILELEN];

//.iOption
char startedDir[BUFLEN]; //프로그램의 현재 실행위치를 전역변수로 저장하는 변수 
char iIDs[ARGNUM][BUFLEN]; //iOption의 옵션의 인자를 최대 5개까지 전역변수로 받는 변수 
int numSt;//추가 인자의 개수를 저장

		  /*어떤 옵션을 선택했는지에 대한 항목*/
int iOption = false; 
int eOption = false;
int tOption = false;
int mOption = false;


void ssu_score(int argc, char *argv[])
{
	char saved_path[BUFLEN];
	int i;

	/*-h, 사용법 출력*/
	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-h")){
			print_usage();
			return;
		}
	}
	
	memset(saved_path, 0, BUFLEN);
	//현재 디렉토리의 정보 
	getcwd(saved_path, BUFLEN); 
	
	strcpy(startedDir, saved_path);

	//) std_dir 인지 학번디렉토리 인지 검사한다 
		//현재 디렉토리에서 갈 수 있으면 std_dir, 없으면 학번디렉토리 
	if(chdir(argv[1]) < 0){
		iOption = true;
                check_option(argc, argv);
		return;
    }
	 
	//saved_path(실행디렉토리임) 로 디렉토리 이전
	chdir(saved_path); 
	
	//경로를 변수에 저장함 
	if(argc >= 3 && strcmp(argv[1], "-c") != 0){
		strcpy(stuDir, argv[1]); //학생 답 디렉토리의 경로 
		strcpy(ansDir, argv[2]); //정답의 디렉토리 경로 
	}

	//check_option이 false라면
		// 옵션에 없는 인자를 받았다면  라면 
	if(!check_option(argc, argv))
		exit(1);

//삭제코드, 
//	if(!eOption && !tOption && !pOption && cOption){
//	if(!eOption && !tOption && !mOption && cOption){
//		do_cOption(cIDs);
//		return;
//	}

	//saved_path(실행디렉토리임) 로 디렉토리 이전
	getcwd(saved_path, BUFLEN);

	//학생 답 디렉토리의 경로로 이전
	if(chdir(stuDir) < 0){
		fprintf(stderr, "%s doesn't exist\n", stuDir);
		return;
	}

	//현재 디렉토리의 정보 저장
	getcwd(stuDir, BUFLEN);

	//saved_path(실행디렉토리임) 로 디렉토리 이전
	chdir(saved_path);

	//정답 디렉토리의 경로로 이전
	if(chdir(ansDir) < 0){
		fprintf(stderr, "%s doesn't exist\n", ansDir);
		return;
	}

	//현재 디렉토리의 정보 저장
	getcwd(ansDir, BUFLEN);

	//saved_path(실행디렉토리임) 로 디렉토리 이전
	chdir(saved_path);
   	
	
	set_scoreTable(ansDir, saved_path);
	set_idTable(stuDir);

	printf("grading student's test papers..\n");
	score_students();

	//./실행 STD_DIR ANS_DIR -i ________,옵션이 주어졌을 경우 --> ssu_score함수에서 실행 
	if(iOption)
	{
		iOptionScore(startedDir, iIDs, numSt);
	}
	
//	if(cOption)
//		do_cOption(cIDs);

	return;
}

int check_option(int argc, char *argv[])
{
	int curmodi=0;	

	int i, j;
	int c;
	char temp[BUFLEN];
	double temp2; 
	char tmp[BUFLEN];
	int mo_cnt = 0;
//코드 수정함    
	while((c = getopt(argc, argv, "e:mti")) != -1)
	{
		switch(c){
			case 'e': 
				eOption = true;
				strcpy(errorDir, optarg); //인자로 받은 에러 디렉토리의 이름을 errorDir 에 복사 

				if(access(errorDir, F_OK) < 0)//파일이 존재하지 않으면 생성 
					mkdir(errorDir, 0755);
				else{
					rmdirs(errorDir);//파일이 존재할 경우 기존 파일을 삭제하고 생성 
					mkdir(errorDir, 0755);
				}
				break;
				/****************새로 구현한 부분**********/
			case 'm':
                  		  mOption = true;              
				  while(1){ //'no' 를 입력받으면 탈출                        
					   //바꾸고 싶은 문제번호 					   
					  printf("Input question's number to modify >> ");					  
					  scanf("%s", temp);                         
					  
					  //no를 입력받았다면 저장하고 whie문을 나온다 					  
					  if(!strcmp(temp, "no")){                               						
						  strcpy(modified_table[mo_cnt].m_number, "no");                                 						  
						  
						  break;                         					   
					  }					  
					  
					  //배열에 저장					  
					  strcpy(modified_table[mo_cnt].m_number, temp);

					  //알고 싶은 번호의 기존 배점을 받아온다			  
					  printf("Current score :");
                                			
					  //배점이 있는 디렉토리를 저장한다 
					  sprintf(tmp, "%s/%s", startedDir, "score_table.csv");				
					  
					  //현재 배점을 출력한다 
					  printf("%.2f\n", mOption_curscore(tmp, temp));
					  
					  //바꾸고 싶은 새로운 배점을 입력한다 
					  printf("New score :");
					  scanf("%lf", &temp2);
                                      
					  //배열에 새로운 배점을 저장한다 
					  modified_table[mo_cnt++].m_new_score = temp2;			
				  }
				  /****************새로 구현한 부분**********/
				  break;
			case 't':
				tOption = true;
				i = optind;//옵션 창에서 몇번째 명령어를 가리킬 예정인지
				j = 0;

				while(i < argc && argv[i][0] != '-'){//옵션이 아니라 옵션 뒤에 나오는 인자일 때 

					if(j >= ARGNUM)//가변인자 최대의 개수인 5개보다 많을 경우
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else
						strcpy(threadFiles[j], argv[i]); //argv[i](인자)를 threadFiles[]로 복사한다
						
					i++; //다음 인자로 넘어감 
					j++; 
				}
				break;
			case 'i':
				i = optind;
                j = 0;

                while(i < argc && argv[i][0] != '-'){  //다음 옵션이 나오기 전까지 
					if(j >= ARGNUM)//5개이상 받는것을 방지 
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else 
						strcpy(iIDs[j], argv[i]); //-옵션 뒤의 인자를 변수에 저장한다 
                                        i++;
                                        j++;
					
                }
				
				if(j>5) //5개 이상 받았을 경우에 
					numSt = 5; //5개만 검사하겠다 
				else //그렇지 않는다면 
					numSt = j;
						
				//1) ./실행 -i 옵션 이 주어졌을 경우  
				if(iOption)
				{
					iOptionScore(startedDir, iIDs, j);
					return 1;
				}
				//2) ./실행 STD_DIR ANS_DIR -i ________,옵션이 주어졌을 경우 --> ssu_score함수에서 실행 
				iOption = true; 

				break;
   			case '?': //이상한 옵션이 들어옴 
				printf("Unkown option %c\n", optopt);
				return false;
		}
	}

	return true;
}
/*원하는 id 의 틀린 문제의 목록을 출력하는 함수 */
void iOptionScore(char *saved_path, char (*iIDs)[BUFLEN], int numSt){

	
        //.csv 파일 열고 + 읽어서 에러 파인딩
        FILE *fp;
        char *p;
        char qname[BUFLEN];
        int idx = 0;
		int cnt = 0;
		int amount = 0;

        char filename[BUFLEN]; //.csv파일에 수정한 코드를 집어넣기 위해
        char num[BUFLEN];

	struct Strings Qname[BUFLEN];
	struct Strings Score[50][110];
	struct Strings wrongQ[BUFLEN];

        sprintf(filename, "%s/%s", saved_path, "score.csv");//.csv 파일을 디렉토리 경>로 
        
	//1) .csv 파일을 연다 
        if((fp = fopen(filename, "r")) == NULL){
                fprintf(stderr, "file open error for %s\n", filename);
                return ;
        }
        
	//2) score.csv 의 첫행에서 문제 번호를 받아온다 
        fscanf(fp, "%s,", qname);
        p = strtok(qname,","); //qname 을 ,단위로 잘라서 받는다   
        while(p!=NULL)
        { 
		strcpy(Qname[cnt++].str, p);
		p=strtok(NULL, ",");
        }
	
	//3) score.csv 의 첫행 이후를 참조 
		//학번당 문제의 점수를 받는다 
		int go=0;
		while(fscanf(fp, "%s,", qname) != EOF){
	        go=0;
		p = strtok(qname, ",");//.csv 파일은 열을 ,단위로 구분 
		
        	while(p!=NULL){
			strcpy(Score[idx][go++].str, p); //문제의 점수를 저장 
			p=strtok(NULL, ",");
		}
		++idx; //학생의 수 
		//printf("\n");
	}
	//4) 원하는 학번의 틀린 문제를 가지고온다 
		//numSt : 5명 이상일 경우엔 5, 아닌경우엔 입력받은 id의 개수 
	
	
	//원하는 학생 수만큼 for문 
	for(int i=0; i<numSt; i++)
	{
		char *temp = iIDs[i]; //참조하는 학번 
		printf("%s's wrong answer :\n ", temp);
		
		//전체 학생수만큼 검사
		for(int t=0; t<idx; t++)//학생수만큼 검사를 하겠지 
		{
			//찾고싶은 학번과 .csv에서 불러온 학번의 이름이 같다면 
				//각 행의 0열에는 학생 아이디가 들어감 
			if(!strcmp(temp, Score[t][0].str))
			{
				//문제의 개수만큼  for문을 돌려서
				for(int d=0; d<cnt-1; d++)//.csv 파일의 sum값은 제외
				{					
					//0점 받은 점수를 출ㄹ력한다 
					if(atof(Score[t][d+1].str) == 0)  
					{	
						 
						printf("%s, ", Qname[d].str);
					}
				}				
			}				
		}
		printf("\n");
	}


	fclose(fp);
}


/*score.csv 파일을 읽는 함수*/
void do_cOption(char (*ids)[FILELEN])
{
	FILE *fp;
	char tmp[BUFLEN];
	int i = 0;
	char *p, *saved;

	//score.csv 파일을 읽기 모드로 읽고
	if((fp = fopen("score.csv", "r")) == NULL){
		fprintf(stderr, "file open error for score.csv\n");//파일이 없으면 return
		return;
	}

	//score.csv 에서 스트링 단위로 읽음 
	fscanf(fp, "%s\n", tmp);

	//파일이 끝나기 전까지
	while(fscanf(fp, "%s\n", tmp) != EOF)
	{
		// "," 단위로 끊어서 p에 저장 (.csv 파일은 열을 , 로 구분)
			//한 셀씩 읽는다 
		p = strtok(tmp, ",");

		if(!is_exist(ids, tmp))
			continue;

		printf("%s's score : ", tmp);

		//이전의 ,까지의 길이만큼 넘어서 검사하기 위해 
		while((p = strtok(NULL, ",")) != NULL)
			saved = p;

		printf("%s\n", saved);
	}
	fclose(fp);
}
/*타겟을 찾는 함수*/
int is_exist(char (*src)[FILELEN], char *target)
{
	int i = 0;

	while(1)
	{
		//while문이 5번 이상 돌면 false 리턴 
		if(i >= ARGNUM)
			return false;
		//배열이 비었다면 false 리턴
		else if(!strcmp(src[i], ""))
			return false;
		//배열과 target 이 같다면 true 리턴 
		else if(!strcmp(src[i++], target))
			return true;
	}
	return false;
}
//현재 실행 위치에 존재해야 하기 때문에 saved_path 인자를 추가해줌 
 //void set_scoreTable(char *ansDir)


/*score_table.csv 를 읽거나 만드는 함수*/
void set_scoreTable(char *ansDir, char *saved_path)
{
	
	char filename[FILELEN];
	/***************************수정코드**************************************/

	sprintf(filename, "%s/%s", saved_path, "score_table.csv");
	
	/***************************수정코드**************************************/

	// ../../ansDirs/score_table.csv가 존재한다면
	if(access(filename, F_OK) == 0){ 
		read_scoreTable(filename); //읽어라
	}
	// ../../ansDirs/score_table.csv가 존재하지 않는다면 
	else{ 
		make_scoreTable(ansDir); //만들고 
		write_scoreTable(filename); //score_table.csv 에 적어라 
	}
}


//score_table.csv 파일이 존재할 때 그 파일을 읽는 함수  
void read_scoreTable(char *path)
{
	FILE *fp;
	char qname[FILELEN];
	char score[BUFLEN];
	int idx = 0;
	int type;
	
			
	//파일을 읽어와서 
	if((fp = fopen(path, "r")) == NULL){//읽기만 가능하며 파일이 존재해야함, 존재하지 않을 경우 에러 리턴 
		fprintf(stderr, "file open error for %s\n", path);
		return ;
	}
	
	//qname : 엔터를 제외한 모든것을 문자열 형식으로 받음
	//score : %s 형식의 문자열을 문자열 형식으로 받음 
	//이미 존재한 score_Table.csv 파일에서 문제번호와 점수를 읽어와 배열에 저장하는 while문 
	while(fscanf(fp, "%[^,],%s\n", qname, score) != EOF){

		strcpy(score_table[idx].qname, qname); // score_table.csv 에서 저장한 문제 이름을 배열에 저장
		score_table[idx++].score = atof(score);// score_table.csv 에서 저장한 점수를 배열에 저장
	
	}

	fclose(fp);

	//mOption 일 경우	
       	if(mOption){
                mOption_rescore(path, idx);
	}
	
}
/*************************************추가코드******************************************/
double mOption_curscore(char *path, char* num){
	FILE *fp;
	char qname[FILELEN];
	char score[BUFLEN];
	int idx = 0;
        char tmp[BUFLEN]; //.csv파일에 수정한 코드를 집어넣기 위해
	char *d;
			
	//파일을 읽어와서 
	if((fp = fopen(path, "r")) == NULL){
		fprintf(stderr, "file open error for %s\n", path);
		return 0;
	}
	
	//qname : 엔터를 제외한 모든것을 문자열 형식으로 받음
	//score : %s 형식의 문자열을 문자열 형식으로 받음 
	//이미 존재한 score_Table.csv 파일에서 문제번호와 점수를 읽어와 배열에 저장하는 while문  
	while(fscanf(fp, "%[^,],%s\n", qname, score) != EOF){
	
		strcpy(tmp, qname);

		d = strtok(tmp, "."); //문제의 번호만 가져오기 위해
									//ex) 1-1.txt 의 1-1 

		if(!strcmp(d, num))  //내가 찾는 문제라면 
		{
			fclose(fp); // 파일을 닫고 
			return atof(score); //double 타입으로 형변환 하여 반환 
		}
	}

	fclose(fp);

}
/*************************************추가코드******************************************/
/*************************************추가코드******************************************/

/*mOption : 문제의 배점을 다시 주는 함수*/
void mOption_rescore(char *path, int idx){
     	
	
	int modify_length=0; //바꿀 문제 
        char tmp[BUFLEN]; //.csv파일에 수정한 코드를 집어넣기 위해
        char *s_num;
        int fd;

		//바꿔야 하는 문제의 개수를 세는 for 문 
        for(int i=0; i<idx; i++)
        {
                if(!strcmp(modified_table[i].m_number,"no")) //no를 받으면 입력을 멈추기 때문
		       break;	
                
		++modify_length;

        }



		//바꿔야하는 개수만큼, 
        for(int p=0; p<modify_length; p++){
              
				//문제의 개수만큼 for문을 돌리며 바꿔야 하는 문제를 찾아서 바꿈 
                for(int i=0; i<idx; i++)
                {
						//기존의 score_table에 저장돼있던 문제번호.c or .txt 를 새로운 배열에 저장한뒤
                        strncpy(tmp, score_table[i].qname, strlen(score_table[i].qname));
                        s_num = strtok(tmp, "."); //.앞의 문자열만 참조 

				
						//바꿔야하는 문제를 찾았다면 
                        if(!strncmp(modified_table[p].m_number, s_num, strlen(s_num))){
				score_table[i].score = modified_table[p].m_new_score; //바꿈
                                break;
                        }
                }
	}

		//파일을 읽기 쓰기 전용으로 열고
        if((fd=open(path, O_RDWR | O_TRUNC))<0) //이미 파일이 있다면 offset 을 0으로 함 
        {
                        fprintf(stderr, "file open error for %s\n", path);
                        return;
        }

		//변경한 배점이 있는 점수들을 파일에 다시 적는다 
        
	for(int i=0; i<idx; i++)
        {
                if(score_table[i].score == 0)
                        break;
                sprintf(tmp, "%s,%.2f\n", score_table[i].qname, score_table[i].score);
                write(fd, tmp, strlen(tmp));
        }
        close(fd);


}
/*************************************추가코드******************************************/

/*배점표를 만드는 함수 */
void make_scoreTable(char *ansDir)
{
	int type, num;
	double score, bscore, pscore;
	struct dirent *dirp, *c_dirp;
	DIR *dp, *c_dp;
	char tmp[BUFLEN];
	int idx = 0;
	int i;

	num = get_create_type(); //num = 1 or 2

	if(num == 1)
	{
		printf("Input value of blank question : ");
		scanf("%lf", &bscore);//blank 점수
		printf("Input value of program question : ");
		scanf("%lf", &pscore);//program 점수
	}

	if((dp = opendir(ansDir)) == NULL){ //ansDir 를 연다
		fprintf(stderr, "open dir error for %s\n", ansDir);
		return;
	}	

	//ansDir 안의 파일들을 읽는다 
	while((dirp = readdir(dp)) != NULL)
	{
		// readdir 읽혀진 파일명 중에 현재 디렉토리를 나타네는 . 도 포함되어 있으므로 
			// 무한 반복에 빠지지 않으려면 파일명이 . 이거나 .. 이면 skip 해야 함
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		//ansDir / 문제
		sprintf(tmp, "%s/%s", ansDir, dirp->d_name);
		
		//파일의 타입이 c도 아니고, txt도 아니라면 다음 파일을 읽는다
		if((type = get_file_type(dirp->d_name)) < 0)
			continue;

		//score_table의 qname 에 정답 파일의 문제 이름이저장
		strcpy(score_table[idx++].qname, dirp->d_name);

//***** 삭제코드***////***** 삭제코드***////***** 삭제코드***////***** 삭제코드***////***** 삭제코드***////***** 삭제코드***//

/*
		if((c_dp = opendir(tmp)) == NULL){
			fprintf(stderr, "open dir error for %s\n", tmp);
			return;
		}

		while((c_dirp = readdir(c_dp)) != NULL)
		{
			if(!strcmp(c_dirp->d_name, ".") || !strcmp(c_dirp->d_name, ".."))
				continue;

			if((type = get_file_type(c_dirp->d_name)) < 0)
				continue;

			strcpy(score_table[idx++].qname, c_dirp->d_name);
		}

		closedir(c_dp);*/
//***** 삭제코드***////***** 삭제코드***////***** 삭제코드***////***** 삭제코드***////***** 삭제코드***////***** 삭제코드***//

	}

	closedir(dp);
	/*score_table[](문제 번호들의 배열) 을 순서대로 sort함*/
	sort_scoreTable(idx);

	//문제의 개수만큼 for문을 돌며 배점을 저장하는 for문
	for(i = 0; i < idx; i++)
	{
		/*type 이 CFILE 이나 TEXTFILE*/
		type = get_file_type(score_table[i].qname);

		//1을 선택했을 때 
		if(num == 1)
		{
			//txt 파일일 때
			if(type == TEXTFILE)
				score = bscore;
			//.c 파일일 때 
			else if(type == CFILE)
				score = pscore;
		}
		//2를 선택했을 때 
		else if(num == 2)
		{
			//배점을 입력받아 저장함 
			printf("Input of %s: ", score_table[i].qname);
			scanf("%lf", &score);
		}

		score_table[i].score = score;
	}
}
/*score_table.csv 에 score_table 배열에 저장한 정보를 적는 함수*/
void write_scoreTable(char *filename)
{
	int fd;
	char tmp[BUFLEN];
	int i;
	int num = sizeof(score_table) / sizeof(score_table[0]); //배열의 크기

	if((fd = creat(filename, 0666)) < 0){ //score_table.csv 파일을 생성하고
		fprintf(stderr, "creat error for %s\n", filename);
		return;
	}

	//score_table 배열의 크기만큼 도는 for문 
	for(i = 0; i < num; i++)
	{
		//배점이 0이라면 함수를 break
		if(score_table[i].score == 0) 
			break;
		
		//score_table.csv 파일에 문제의 이름을 적고
			//배점을 실수형 단위로, 소수점 2자리 까지 적는다 
		sprintf(tmp, "%s,%.2f\n", score_table[i].qname, score_table[i].score);
		//파일에 기입 
		write(fd, tmp, strlen(tmp));
	}

	close(fd);
}

/*STD_DIR 정보를 활용해 id_table[]에 학번정보를 대입하는 함수*/
void set_idTable(char *stuDir)
{
	
	struct stat statbuf; //stat 구조체, 파일 정보를 담고있음 
	struct dirent *dirp; // dirent 구조체, 파일 혹은 디렉토리 정보를 가지고 있음 
	DIR *dp;
	char tmp[BUFLEN];
	int num = 0;

	if((dp = opendir(stuDir)) == NULL){//stu_DIR(STD_DIR)에 해당하는 디렉토리 스트림을 열고 포인터 반환
									    //이때 포인터는 첫번째 항목을 가리킴
		fprintf(stderr, "opendir error for %s\n", stuDir);
		exit(1);
	}

	while((dirp = readdir(dp)) != NULL){ //STU_DIR 하위 디렉토리를 한번씩 확인함
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		sprintf(tmp, "%s/%s", stuDir, dirp->d_name); //하위디렉토리 이름
		stat(tmp, &statbuf);

		if(S_ISDIR(statbuf.st_mode)) //stat구처체, S_ISDIR – 디렉토리 파일인지 판별
			strcpy(id_table[num++], dirp->d_name);
		else
			continue;
	}

	sort_idTable(num);
}

/*학생들의 정보가 담긴 id_Table을 번호순으로 정렬하는 함수*/
void sort_idTable(int size)
{
	int i, j;
	char tmp[10];
	
	//버블정렬 알고리즘 
	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 -i; j++){
			
			//학생들의 id를 번호순서대로 저장함(작은 번호 --> 큰번호)  
			if(strcmp(id_table[j], id_table[j+1]) > 0){ // 앞 인덱스의 배열 값이 뒤 인덱스의 배열의 값 보다 크다면 자리를 바꾼다
				strcpy(tmp, id_table[j]); //tmp에 id_table[i]를 복사 
				strcpy(id_table[j], id_table[j+1]); //서로 값을 바꿔 작은 값이 먼저 올 수 있게 함 
				strcpy(id_table[j+1], tmp);
			}
		}
	}
}
/*문제 번호를 크기순으로 정렬하는 함수 */
void sort_scoreTable(int size)
{
	int i, j;
	struct ssu_scoreTable tmp;
	int num1_1, num1_2;
	int num2_1, num2_2;

	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 - i; j++){

			get_qname_number(score_table[j].qname, &num1_1, &num1_2);//비교대상 1 
			get_qname_number(score_table[j+1].qname, &num2_1, &num2_2);//비교대상 2

			//.이나 - 앞의 번호가 더 크거나 같고 
			//.이나 - 뒤의 번호가 비교대상 1 이 더 클 경우 
			//--> 2-2 ,2-1의 소트 / 3.c 2.c의 소트  
			if((num1_1 > num2_1) || ((num1_1 == num2_1) && (num1_2 > num2_2))){

				memcpy(&tmp, &score_table[j], sizeof(score_table[0]));//임시 변수에 인덱스는 앞이지만, 수는 큰 수를 저장해
				memcpy(&score_table[j], &score_table[j+1], sizeof(score_table[0]));//둘의 자리를 바꾼다 
				memcpy(&score_table[j+1], &tmp, sizeof(score_table[0]));
			}
		}
	}
}

/*qname의 . 혹은 - 앞의 번호만 알아내는 함수
 ex) 4-3 중 4
 */
void get_qname_number(char *qname, int *num1, int *num2)
{
	char *p;
	char dup[FILELEN];

	strncpy(dup, qname, strlen(qname)); // dup에 qname의 길이만큼 qname 을 저장 
	*num1 = atoi(strtok(dup, "-."));  //*num1은 - 나 .이 나오기 전까지의 문자열을 double 형으로 변환한 것을 가리킴 

	 //NULL을 넣었을 때는 직전 strtok 함수에서 처리했던 문자열에서 잘린 문자열만큼 
		//다음 문자로 이동한 뒤 다음 문자열을 자릅니다.
	p = strtok(NULL, "-.");
	if(p == NULL)
		*num2 = 0;
	else
		*num2 = atoi(p);
}
//처음 실행시 1번이나 2번 옵션 중 어느 옵션을 선택할 것인지 묻는 함수
int get_create_type()
{
	int num;

	while(1)
	{
		printf("score_table.csv file doesn't exist in TREUDIR!\n");
		printf("1. input blank question and program question's score. ex) 0.5 1\n");
		printf("2. input all question's score. ex) Input value of 1-1: 0.1\n");
		printf("select type >> ");
		scanf("%d", &num);

		if(num != 1 && num != 2) //1번도 아니고 2번도 아니라면 
			printf("not correct number!\n");
		else //1번이나 2번 중 하나를 입력받았을 때 
			break;
	}

	return num;
}

//학생 한명씩 정답여부를 검사하는 함수 
void score_students()
{
	double score = 0;
	int num;
	int fd;
	char tmp[BUFLEN];
	int size = sizeof(id_table) / sizeof(id_table[0]);
	if((fd = creat("score.csv", 0666)) < 0){//score.csv파일 권한 -> 소유자, 그룹, 다른 사람 모두 읽기 쓰기 가능
		fprintf(stderr, "creat error for score.csv");
		return;
	}
	write_first_row(fd);//첫번째 행에 문제의 번호를 기입 
	int ji = 0;
	//학생들의 수 만큼 돈다 
	for(num = 0; num < size; num++)
	{
		if(!strcmp(id_table[num], "")) //학번이 비었다면 
			break;

		//학번을 tmp에 string 형으로 저장한뒤 
		sprintf(tmp, "%s,", id_table[num]);
		//fd에 적는다 
		write(fd, tmp, strlen(tmp));
		//학생들의 총점을 낸다 
		score += score_student(fd, id_table[num]);
	}

	//평균을 출력 
	printf("Total average : %.2f\n", score / num);

	close(fd);
}
//학생들 당 답을 채점한 결과를 더하는 함수 
double score_student(int fd, char *id)
{
	
	int type;
	double result;
	double score = 0;
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]); //score_table 의 크기 
	for(i = 0; i < size+1; i++)
	{
		//배점이 0이라면 break
		if(score_table[i].score == 0)
			break;

		//한 학생의 답 에대한 경로 
		sprintf(tmp, "%s/%s/%s", stuDir, id, score_table[i].qname);

		//파일이 존재하지 않는다면 return fase 
		if(access(tmp, F_OK) < 0)
			result = false;
		else
		{
			//파일의 타입이 c 나 txt 가 아닐경우 
			if((type = get_file_type(score_table[i].qname)) < 0)
				continue;
			
			/*파일의 형식에 따라 채점형식이 달라짐 */

			//txt 파일일경우 
			if(type == TEXTFILE){
			result = score_blank(id, score_table[i].qname);
			}
			//c파일일 경우 
			else if(type == CFILE)
				result = score_program(id, score_table[i].qname);
		}
		//학생답에 error가 있을 때, 정답에 error나 warning 이 존재할 때 false 
		if(result == false)
			write(fd, "0,", 2); //0점을 적음 
		else{ //학생답 warning , 혹은 정답 
			if(result == true){ //맞는 답일 경우 
				score += score_table[i].score; //score 합을 갱신함 
				sprintf(tmp, "%.2f,", score_table[i].score); //score.csv 파일에 배점을 부여함 
			}
			else if(result < 0){ //warning 이 존재할 경우 
				score = score + score_table[i].score + result; //score 의 합 에서 warning(음수) 만큼더하고 
				sprintf(tmp, "%.2f,", score_table[i].score + result); //배점에서 warning(음수)를 더해서 score.csv에 적음 
			}
			write(fd, tmp, strlen(tmp));
		}
	}

	printf("%s is finished. score : %.2f\n", id, score); 
	sprintf(tmp, "%.2f\n", score);
	write(fd, tmp, strlen(tmp));

	return score;
}

/*.csv 파일의 첫 행에 문제를 적는 함수 */
void write_first_row(int fd)
{
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]);//ANS_DIR에 존재하는 문제의 개수

	write(fd, ",", 1);

	for(i = 0; i < size; i++){//ANS_DIR 문제의 개수만큼 
		if(score_table[i].score == 0)
			break;
		
		sprintf(tmp, "%s,", score_table[i].qname); //문제의 이름을 .csv 에 기입 
		write(fd, tmp, strlen(tmp));
	}
	write(fd, "sum\n", 4);//개행을 통해 다음 행으로 offset을 넘김 
}

/*정답 파일에서 답을 가져오는 함수*/
char *get_answer(int fd, char *result)
{
	char c;
	int idx = 0;

	memset(result, 0, BUFLEN);
	while(read(fd, &c, 1) > 0)//eof 를 만날때 까지 파일을 읽는다 
	{
		if(c == ':')//다수의 답이 모두 정답이 되는 경우
			break;
		
		result[idx++] = c; //버퍼에 :전까지 읽은 문자를 저장한다 
	}
	if(result[strlen(result) - 1] == '\n')//파일에서 개행문자를 읽었어 버퍼에 저장했다면
		result[strlen(result) - 1] = '\0';//버퍼에서 '\0'문자로 바꾼다 

	return result;//버퍼에 저장한 학생의 정답을 리턴한다 
}
//.txt 문제를 채점하는 함수 
int score_blank(char *id, char *filename)
{
	char tokens[TOKEN_CNT][MINLEN];
	node *std_root = NULL, *ans_root = NULL;
	int idx, start;
	char tmp[BUFLEN];
	char s_answer[BUFLEN], a_answer[BUFLEN];
	char qname[FILELEN];
	int fd_std, fd_ans;
	int result = true;
	int has_semicolon = false;

	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.'))); // 문제.txt 중 문제만 qname에 저장한다

	sprintf(tmp, "%s/%s/%s", stuDir, id, filename); //tmp에 학번안의 문제.txt의 디렉토리 주소를 대입
	fd_std = open(tmp, O_RDONLY);
	strcpy(s_answer, get_answer(fd_std, s_answer));//fd_std = STD_DIR / 학번 / 문제.txt 
	//answer에 get_answer를 복사(파일을 읽어 버퍼에 저장)
	
	//비어있는 파일일 경우
	if(!strcmp(s_answer, "")){
		close(fd_std);
		return false;
	}
	//완벽한 괄호가 없는 경우 파일을 닫고 false 를 리턴 
	if(!check_brackets(s_answer)){
		close(fd_std);
		return false;
	}

	//앞과 뒤의 공백문자를 없앤뒤 s_answer에 복사
	strcpy(s_answer, ltrim(rtrim(s_answer)));
	
	//s_answer의 끝에 세미콜론이 존재 할 경우 '\0'로 바꿈 
	if(s_answer[strlen(s_answer) - 1] == ';'){
		has_semicolon = true;
		s_answer[strlen(s_answer) - 1] = '\0';
	}
	//토큰 단위로 읽음 
	if(!make_tokens(s_answer, tokens)){
		close(fd_std);
		return false;
	}


	idx = 0;
	//최상위 노드가 std_root에 return 됨 
	std_root = make_tree(std_root, tokens, &idx, 0);


	/***********************************수정코드***********************************/
	sprintf(tmp, "%s/%s", ansDir, filename); //정답 문제 파일
	/***********************************수정코드***********************************/
	
	fd_ans = open(tmp, O_RDONLY);//열고

	/*정답들을 하나씩 학생답과 비교하는 while*/
	while(1)
	{

		ans_root = NULL;
		result = true;

		//토큰의 개수만큼 
		for(idx = 0; idx < TOKEN_CNT; idx++)
			memset(tokens[idx], 0, sizeof(tokens[idx])); //토큰을 초기화 해주고 
		strcpy(a_answer, get_answer(fd_ans, a_answer)); //‘:’ 전이나 eof 를 만나기 전까지 문자를 모두 저장한 배열
/////////////////////////////////////
		if(!strcmp(a_answer, "")){ //비었다면 break
			break;
		}

		//a_anser 의 좌 우 여백을 없앤뒤 
		strcpy(a_answer, ltrim(rtrim(a_answer)));

		//학생 답에 세미콜론이 없는데
		if(has_semicolon == false){
			//정답의 끝에 세미콜론이 존재한다면 또 검사한다 (while)
			if(a_answer[strlen(a_answer) -1] == ';')
				continue;
		}

		//학생 답에 세미콜론이 있는데
		else if(has_semicolon == true)
		{
			//정답의 끝에 세미콜론이 존재하지 않는다면 또 검사한다 
			if(a_answer[strlen(a_answer) - 1] != ';')
				continue;
			else
				//정답의 끝에 세미콜론이 존재한다면 
				a_answer[strlen(a_answer) - 1] = '\0'; //\0대신 대입 
		}

		//정답의 토큰을 만들고
		if(!make_tokens(a_answer, tokens))
			continue;

		idx = 0;
		//정답의 트리를 만든뒤
		ans_root = make_tree(ans_root, tokens, &idx, 0);
		//학생답과 정답을 하나하나 비교한다 
		compare_tree(std_root, ans_root, &result);

		//정답이라면
		if(result == true){
			//파일을 닫고
			close(fd_std);
			close(fd_ans);
			//루트노드가 비지 않았다면 
			if(std_root != NULL)
				free_node(std_root); //노드를 비운다
			if(ans_root != NULL)
				free_node(ans_root);//노드를 비운다
			return true;//true 리턴

		}
	}
	//모두 검사한 뒤에도 정답과 학생답이이 나오지 않았을 때 
	close(fd_std);
	close(fd_ans);

	//루트노드가 비지 않았다면 
	if(std_root != NULL)
		free_node(std_root); //노드를 비운다
	if(ans_root != NULL)
		free_node(ans_root); //노드를 비운다

	return false;//false 리턴
}

double score_program(char *id, char *filename)
{
	
	double compile;
	int result;

	compile = compile_program(id, filename);

	//학생답에 error 
	//정답에 error 나 warning 이 존재 할때 
	if(compile == ERROR || compile == false)
		return false; 
	
	//5초이상 걸리거나 정답과 학생답이 틀릴때 false
	//답이면 true
	result = execute_program(id, filename);

	//5초이상 걸리거나 정답과 학생답이 틀릴때 false	
	if(!result) 
		return false;

	//학생답에 warning 이 존재할 때 
	if(compile < 0)
		return compile;
	//그렇지 않을 때 
	return true;
}
/*지정된 문제 번호가 입력한 -t 옵션의 인자로 입력한 threadFiles에 존재하는지 판단하는 함수*/
int is_thread(char *qname)
{
	int i;
	int size = sizeof(threadFiles) / sizeof(threadFiles[0]); //배열 크기 판단 
	
	//thradFiles크기만큼 돌며 검사하는 문제 중 threadFiles가 있는지 판단 
	for(i = 0; i < size; i++){
		if(!strcmp(threadFiles[i], qname)) //현재 문제번호(qname)가 -t옵션으로 받은 인자값과 동일하다면 
			return true; 
	}
	//동일하지 않다면
	return false;
}

/*컴파일을 위해 명령어와 디렉토리를 결합하고 컴파일(redirection함수)을 한 뒤 에러여부를 파악하는 함수 */
double compile_program(char *id, char *filename)
{
	int fd;
	char tmp_f[BUFLEN], tmp_e[BUFLEN];
	char command[BUFLEN];
	char qname[FILELEN];
	int isthread;
	off_t size;
	double result;

	memset(qname, 0, sizeof(qname));
	//qname 은 . 이전의 문제번호이다 
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));
	
	//qname 이 -t 옵션의 인자로 받은 문제인지 판단하는 변수 
	isthread = is_thread(qname);

	/*정답 컴파일 */

	//~~sprintf(tmp_f, "%s/%s/%s", ansDir, qname, filename);	
	//~~sprintf(tmp_e, "%s/%s/%s.exe", ansDir, qname, qname);
	
	sprintf(tmp_f, "%s/%s", ansDir, filename);
	sprintf(tmp_e, "%s/%s.exe", ansDir, qname);

	if(tOption && isthread)
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	//~~sprintf(tmp_e, "%s/%s/%s_error.txt", ansDir, qname, qname);
	sprintf(tmp_e, "%s/%s_error.txt", ansDir, qname);
	
	fd = creat(tmp_e, 0666);

	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);
	unlink(tmp_e);

	//정답 파일의 컴파일결과의 에러파일이 존재하는 경우에는 false 리턴 
	if(size > 0)
		return false;

	/*학생 답 컴파일 */

	sprintf(tmp_f, "%s/%s/%s", stuDir, id, filename); //tmp__f:STD_DIR / 학생학번 / 파일번호
	sprintf(tmp_e, "%s/%s/%s.stdexe", stuDir, id, qname);//tmp_e : STD_DIR /학생학번/파일번호.stdexe

	if(tOption && isthread) //-t옵션을 선택하고 현재 검사하는 문제의 번호가 -t옵션으로 받은 인자의 번호와 같다면 pthread로 컴파일 할 예정 
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);//-> gcc -o STD_DIR /학생학번/파일번호.stdexe STD_DIR/학생학번/파일번호  

	sprintf(tmp_f, "%s/%s/%s_error.txt", stuDir, id, qname); //학생들의 에러를 저장할 텍스트의 경로를 저장하는 변수
	fd = creat(tmp_f, 0666); //파일 생성 

	redirection(command, fd, STDERR); //파일을 컴파일 하고 에러를 fd 가 가리키는 ~_error.txt 에 저장한다 
	size = lseek(fd, 0, SEEK_END); //~_error.txt 파일의 사이즈를 재고 
	close(fd); //파일 디스크립터를 닫는다 

	if(size > 0){ //에러가 존재할 때 
		if(eOption) //-e를 옵션으로 입력했다면 
		{
			sprintf(tmp_e, "%s/%s", errorDir, id); //입력값으로 경로를 설정하고 
			if(access(tmp_e, F_OK) < 0) //그 디렉토리에 접근한 뒤 없으면 
				mkdir(tmp_e, 0755); //디렉토리를 생성한다 

			sprintf(tmp_e, "%s/%s/%s_error.txt", errorDir, id, qname); //에러를 저장할 파일의 경로를 설정하고 
			rename(tmp_f, tmp_e);
			/*#include <stdio.h>
			int rename(const char *oldpath, const char *newpath);
			//성공시 0 return
			//oldpath와 newpath의 종류가 다르면 오류
			if oldpath 가 파일
				-newpath파일이 이미 있을 때 : newpath파일을 삭제하고 oldpath를 newpath로 변경
				-newpath 파일이 없는 파일일 때 : oldpath파일을 newpath파일로 rename함
			if oldpath 가 디렉토리
				-newpath가 이미 존재하고 비어있는 디렉토리일때 : newpath를 삭제하고 oldpath를 newpath로 이름을 변경
				-newpath가 이미 존재하지만 비어있지 않은 디렉토리일 때 : 오류
				-newpath가 없는 디렉토리일때 : newpath로 이름이 변경됨

			*/

			result = check_error_warning(tmp_e); //에러의 종류에 따른 점수를 계산하기 위한 함수 호출 
		}
		else{ 
			result = check_error_warning(tmp_f); //에러의 종류에 따른 점수를 계산하기 위한 함수 호출 
			unlink(tmp_f);//더이상 그 디렉토리를 안씀 
		}

		return result; //error와 warning 에 따라 계산한 점수를 리턴  
	}

	unlink(tmp_f);//더이상 그 디렉토리를 안씀 
	return true; //에러(error, warning)가 존재하지 않을 때 
}

/*에러 텍스트 파일의 에러종류가 error 인지 warning 인지 판별하는 함수*/
double check_error_warning(char *filename)
{
	FILE *fp;
	char tmp[BUFLEN];
	double warning = 0; 
	/*FILE *fopen( const char *filename, const char *mode );
	//filename : 개방할 파일의 경로 및 파일명
	//mode : 파일 접근 권한(읽기 전용, 쓰기 전용 등 아래 표 참고)
	//반환값 : 개방된 파일의 FILE 구조체 포인터, 실패시 NULL포인터
	*/
	//파일열기에 실패한 경우
	if((fp = fopen(filename, "r")) == NULL){ 
		fprintf(stderr, "fopen error for %s\n", filename);
		return false;
	}

	//파일을 스트링 단위로 받았을 때 
	while(fscanf(fp, "%s", tmp) > 0){ //string단위로 끊어서 데이터를 읽음 
		if(!strcmp(tmp, "error:"))//"error:"가 있을시에 
			return ERROR; //return 0
		else if(!strcmp(tmp, "warning:"))//"warning:"이 있을시에
			warning += WARNING; //warning = warning - 0.1 
	}

	return warning;
}
/* 정답 파일과 학생 답 파일의 실행파일의 결과를 결과 파일에 각각 저장하고, 
시간이 5초이상 지연되는 경우에 프로그램을 kill 하는 함수 */
int execute_program(char *id, char *filename)
{
	/*time_t time( time_t *timer );
		time_t는 time.h 헤더 파일에 존재
		초 단위 시간을 계산
		시스템 클럭에 따라 만국 표준시(UCT)의 1970 년 1월 1일 0시 0분 0초부터 경과된 시간을 초단위로 나타내는 숫자
		리턴 : time() 함수는 인수에서 얻은 값과 같은 값을 반환, 에러시 -1

	*/
	char std_fname[BUFLEN], ans_fname[BUFLEN];
	char tmp[BUFLEN];
	char qname[FILELEN];
	time_t start, end;
	pid_t pid;
	int fd;

	memset(qname, 0, sizeof(qname));//초기화
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));//qname -> .c 앞 문자열, 문제 번호
	
	/*정답 파일의 .exe 와 .stdout*/
	sprintf(ans_fname, "%s/%s.stdout", ansDir,qname);//ANS_DIR/문제번호.stdout
	fd = creat(ans_fname, 0666);

	sprintf(tmp, "%s/%s.exe", ansDir, qname);//실행파일의 경로 
	redirection(tmp, fd, STDOUT);//.exe 파일을 실행한뒤 그 결과를 .stdout 파일에 저장한다 

	close(fd);
	
	//*학생 답 파일의 .exe 와 .stdout*/
	sprintf(std_fname, "%s/%s/%s.stdout", stuDir, id, qname);//STU_DIR/학번/문제번호.stdout
	fd = creat(std_fname, 0666);
	
	//.exe 파일을 백그라운드에서 실행하기 위한 변수 
	sprintf(tmp, "%s/%s/%s.stdexe &", stuDir, id, qname);

//시간재기 start
	start = time(NULL);

	//백그라운드로 실행하여 결과를 .stdout 에 출력 
	redirection(tmp, fd, STDOUT);
	

	
	sprintf(tmp, "%s.stdexe", qname);
	
	//시간이 5초이상일 때 프로세스를 킬하는 while문  
		//프로세스가 존재할 경우에 
	while((pid = inBackground(tmp)) > 0){
		end = time(NULL);
//시간재기 end

		//프로그램의 실행시간을 측정해서 5초보다 클 경우 
		if(difftime(end, start) > OVER){
			//현재 프로세스에 SIGKILL 시그널을 보내 프로세스를 죽인다 
			kill(pid, SIGKILL);
			close(fd);//파일을 닫음 
			return false;
		}
	}

	close(fd);

	return compare_resultfile(std_fname, ans_fname);
}

pid_t inBackground(char *name)
{
	// 채점 프로그램이 실행되는 동안 정답 파일을 실행하는 프로그램이 백그라운드로 실행됨. 
		// ps|grep 명령어는 백그라운드로 실행되는 프로그램을 쉽게 캐치하고자 사용된다

	//ps 는 프로세스의 상태를 확인하는 명령어이다
		//현재 특정 프로세스가 실행되고 있는지 실행되고 있는 프로세스가 어떤 pid를 갖고 있는지 등을 확인하는데 많이 쓰임

	pid_t pid;
	char command[64];
	char tmp[64];
	int fd;
	off_t size;
	
	//tmp를 초기화 
	memset(tmp, 0, sizeof(tmp));

	//background.txt 생성 
	fd = open("background.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
	
	//원하는 프로세스의 이름을 찾는 명령어를 command 에 저장하한다
	sprintf(command, "ps | grep %s", name);
	
	//원하는 프로세스의 이름을 얻어 fd 에 저장한다 
	redirection(command, fd, STDOUT);

	//offset 을 파일의 처음으로 옮긴다 
	lseek(fd, 0, SEEK_SET);

	//파일을 읽어 tmp에 저장한다 
	read(fd, tmp, sizeof(tmp));

	//프로세스가 없다면 background.txt 의 링크를 끊고 파일을 닫는다 
	if(!strcmp(tmp, "")){
		unlink("background.txt");
		close(fd);
		return 0;
	}
	
	//프로세스가 있다면 공백 앞의 값을 pid에 저장한다  
	pid = atoi(strtok(tmp, " "));
	close(fd);

	unlink("background.txt");
	return pid; //pid를 리턴
}


/*학생답과 정답의 .stdout (결과파일)을 비교하는 함수*/
int compare_resultfile(char *file1, char *file2)
{
	//char *file1 : 학생 답.stdout 파일의 경로 
	//char *file2 : 정답.stdout 파일의 경로
	int fd1, fd2;
	char c1, c2;
	int len1, len2;

	fd1 = open(file1, O_RDONLY); //읽기모드로 파일을 연다
	fd2 = open(file2, O_RDONLY); //읽기모드로 파일을 연다

	//stdout 파일의 문자열이 같은지 확인하는 와일문 
		//공백을 제외하고 한 문자씩 검사한다 
	while(1)
	{
		//학생답의 공백울 제외하고 첫 문자를 read 하는 while 문 
		while((len1 = read(fd1, &c1, 1)) > 0){//EOF에 다다를 때 까지 1바이트씩 읽는다
			if(c1 == ' ') //공백일 경우 while문을 continue 한다
				continue;
			else //문자일 경우 while문을 빠져나온다
				break;
		}
		//정답의 공백울 제외하고 첫 문자를 read 하는 while 문 	
		while((len2 = read(fd2, &c2, 1)) > 0){
			if(c2 == ' ') 
				continue;
			else 
				break;
		}
		
		//stdout 파일이 비었다면 더이상 검사를 하지 않음 
		if(len1 == 0 && len2 == 0)
			break;

		//stdout 파일이 공백이 아니라면 
			//대소문자의 차이는 상관없음 
		to_lower_case(&c1); //대소문자를 소문자로 통일하기 위한 함수
		to_lower_case(&c2); //대소문자를 소문자로 통일하기 위한 함수

		if(c1 != c2){ //정답과 학생답의 문자가 다르다면
			close(fd1);//파일을 닫는다 
			close(fd2);
			return false; //false return 
		}
	} //stdout 파일이 둘다 공백일 때 
		//정답과 학생답의 파일의 문자가 같을 때 
	close(fd1);
	close(fd2);
	//학생답과 정답이 같다면 true return
	return true;
}
/*재지향 하는 함수*/
	//old로 들어오는게 system command 이후로 new 에 저장
void redirection(char *command, int new, int old)
{
	/*ex)
	*command : exe 파일
	* int new : ansDir/qname/qname_error.txt
	* old : STDERR(#define 2 = 표준애러)
	*/
	int saved; 
	saved = dup(old);//saved의 디스크립터가가 old의 파일테이블을 가리킴
		//ex) saved 의 디스크립터가 표준에러 파일테이블을 가리킴
	dup2(new, old);//old의 디스크립터가 new의 파일테이블을 가리킴 
		//ex) 표준에러가 error.txt를 가리킴 

	system(command);//exe파일을 컴파일 하라 
		//ex) 이후 표준에러로 나오는 것이 error.txt에 저장됨
	
	dup2(saved, old);//old가 saved를 가리킴 
	//원상복귀 

	close(saved);//saved를 닫음
}

/*.c 파일인지 .txt 파일인지 구분해주는 함수*/
int get_file_type(char *filename)
{
	/*char *strrchr(char * const _String, int const _Ch);
	  strrchr 로 시작하는 문자열의 포인터를 반환 (찾는 문자부터 끝까지 반환)
	  뒤에서부터 검색
	*/
	char *extension = strrchr(filename, '.');

	if(!strcmp(extension, ".txt"))//.부터 시작하는 문자열이 .txt와 같다면 TEXTFILE
		return TEXTFILE;
	else if (!strcmp(extension, ".c"))//.부터 시작하는 문자열이 .c 와 같다면 CFILE
		return CFILE;
	else
		return -1; //둘다 아니라면 -1리턴 
}

//eOption : errorDir를 삭제하는 함수  
void rmdirs(const char *path)
{
	struct dirent *dirp; //dirent 구조체
	struct stat statbuf; //stat 구조체 
	DIR *dp; // 디렉토리 검색을 위해서 특정 디렉토리를 열면 DIR 구조체의 포인터 DIR *를 리턴
	char tmp[BUFLEN];
	
	//errorDir 를 연다
	if((dp = opendir(path)) == NULL)
		return;

	//errorDir 안의 내용을 하나씩 읽는다 
	while((dirp = readdir(dp)) != NULL)
	{
			// readdir 읽혀진 파일명 중에 현재 디렉토리를 나타네는 . 도 포함되어 있으므로 
		// 무한 반복에 빠지지 않으려면 파일명이 . 이거나 .. 이면 skip 해야 함
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		sprintf(tmp, "%s/%s", path, dirp->d_name);//errorDir 의 파일 이름의 경로를 저장

		if(lstat(tmp, &statbuf) == -1) //심볼릭 링크의 파일 정보를 받아옴 
			continue;

		if(S_ISDIR(statbuf.st_mode))//errorDir 안에 또다른 디렉토리가 있다면  
			rmdirs(tmp);//한번 더 검사를 하기 위해 재귀한다 
		else
			unlink(tmp);//그렇지 않는다면 링크를 끊는다
	}

	closedir(dp); 
	rmdir(path);//디렉토리를 삭제한다 
}

void to_lower_case(char *c)
{
	/*아스키코드값
	A(65) ~ Z(90)
	a(97) ~ z(122)*/
	if(*c >= 'A' && *c <= 'Z') //문자가 대문자일 경우
		*c = *c + 32; //소문자로 바꾼다 int amount = (int)'A'-'a'; amoung는 32 
}

void print_usage()
{//-h 누르면 사용법 출력 
	printf("Usage : ssu_score <STUDENTDIR> <TRUEDIR> [OPTION]\n");
	printf("Option : \n");
	printf(" -m                modify question's score\n");
	printf(" -e <DIRNAME>      print error on 'DIRNAME/ID/qname_error.txt' file \n");
	printf(" -t <QNAMES>       compile QNAME.C with -lpthread option\n");
	printf(" -i <IDS>          print ID's wrong questions\n");
	printf(" -h          	   print usage \n");
}
