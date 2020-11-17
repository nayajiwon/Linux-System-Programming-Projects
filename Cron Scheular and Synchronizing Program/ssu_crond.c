#include "ssu_crond.h"

char crondfiledir[BUFLEN]; 
char logdir[BUFLEN]; 
finfo *list; 

int main(){

	
	char prelist[BUFLEN][BUFLEN];
	char buf[BUFLEN]; 
	struct stat statbuf; 
	time_t intertime; 
	int count = 0; 
	int i; 

        getcwd(buf, BUFLEN);
    
        sprintf(crondfiledir, "%s/%s", buf, "ssu_crontab_file"); 
        sprintf(logdir, "%s/%s", buf, "ssu_crontab_log"); 
/*
   	디몬은 가장 먼저 실행되기 때문에 파일에 내용이 없다고 가정해도 ok 
	*/

	while(1)
	{

		if(stat(crondfiledir , &statbuf)==0){
			break; 
		}

	}

	ssu_daemon_init();
	intertime = statbuf.st_mtime;  

	while(1){
		toreserve(prelist, &intertime, &count); 
		waitiltime();
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
        
//	chdir("/");
        fd = open("/dev/null", O_RDWR);//표준출력을 모두 Null로 보냄 
        dup(0);
        dup(0);
        return 0;
}

void toreserve(char (*prelist)[BUFLEN], time_t *mtime, int *precount){
	
	char curlist[BUFLEN][BUFLEN]; 
	int check[BUFLEN]; 
	struct stat statbuf;	
	int i;
	int p; 
	int curcount = 0; 


	if(stat(crondfiledir , &statbuf)<0){
		fprintf(stderr, "stat error for ssu_crondtab_file\n"); 
		exit(1);
	}

	if(*mtime != statbuf.st_mtime)
	{
		*mtime = statbuf.st_mtime; 
		getPrelist(curlist, &curcount); 
	
		memset(check, 0, BUFLEN); 

		if(curcount>*precount){

			//파일내용이증가 *precount-> curcount); 
				
			i=0; p=0; 
			for(i=0; i<curcount; i++)
			{
				
				for(p=0; p<*precount; p++)
				{
		
					if(!strcmp(curlist[i], prelist[p]))
					{
						check[p] = 1; 
					
						break; 
					}
				}
			}
			
			for(i=0; i<curcount; i++)
				if(check[i] == 0)
				{
					//추가됐습니다, curlist[i]); 
					add(curlist[i]); 
					break; 
				}

			traverse(); 	

		}
	
		if(curcount<*precount){
			//파일내용이 감소, *precount, curcount); 
		
			i=0; p=0; 
			for(i=0; i<curcount; i++)
			{
				for(p=0; p<*precount; p++)
				{
					if(!strcmp(curlist[i], prelist[p]))
					{
						//이전 두개가 같음
						check[p] = 1; 
					
						break; 
					}
				}

			}

			for(i=0; i<*precount; i++)
				if(check[i] == 0)
				{
					//삭제, prelist[i]); 
					//traverse(); 
					removenode_num(i, prelist[i]); 
					break; 
				}

			//traverse(); 




		}
		*precount=curcount; 

		//printf("======================================\n");
		//for(int j=0; j<10;j++){
		//	printf("[%d]: %d, ", j, check[j]);
		//}
		//printf("======================================\n");
		
		for(int a=0; a<BUFLEN; a++)
			memcpy(prelist[a], curlist[a], BUFLEN); 
		
		
	}	
	
}

//취소된 쓰레드를 삭제하는 함수
void removenode_num(int num, char *data){
     
	//리무브 노드 합니다
	int cnt = 0; 
	int flag = 0; 
        if(num<0){
                printf("해당 번호가 없습니다\n");
                return; 
        }

	finfo *tmp = list;
        finfo *cur = list;
        finfo *pre = NULL;
	
	traverse();
	
	while(tmp!=NULL){
     
                if(!strcmp(tmp->data, data)){
                 	flag = 1;
			break; 
		}
                tmp=tmp->next;
                cnt++;
        }

	if(flag==0){
		//printf("쓰레드가 이미 삭제했습니다 \n");
		return;
	}

        for(int i=0; i<cnt; i++){
                pre = cur;
                cur = cur->next;
        }

	//삭제된 노드를 제외하고 이전노드와 다음 노드를 잇는다
        if(cnt==0)
                list = list->next;
        else
                pre->next = cur->next;

        free(cur);//메모리 해제
        traverse();






        return;
    
}


//이전 파일 디렉토리의 구조를 가져오는 함수
void getPrelist(char (*curlist)[BUFLEN], int *count){
	
	FILE *fp; 
	int i, p; 
	int length; 
	int cnt; 
	char buf[BUFLEN]; 
	char *ptr; 	
	
	
	if((fp = fopen(crondfiledir, "r"))==NULL){
		fprintf(stderr, "ss_crond_file error\n");
		return; 
	}


	cnt = 0; 
	p=0; 

	

	//파일 안의 내용을 다 가져옴--> 예약받기 위해
	while(fgets(buf, BUFLEN, fp)!=NULL){
		i=0; 
		length = strlen(buf); 

		buf[length-1] = '\0';

                while(buf[i] != '.')
                        i++;
                i+=2;
                ptr = buf+i;

		strcpy(curlist[p], ptr);
		++cnt; ++p; 
	}
	
	*count = cnt; 
	
		
	
	fclose(fp); 
}

//등록된 링크드 리스트 만큼돌리면서 예약한 시간에 쓰레드를 실행하기 위한 함수
void waitiltime(){
        finfo *cur = list;
        while(cur!=NULL){
               
		tokenanalizer(cur->data);
		cur=cur->next;
        }


	
}

//현재시간과 가능한 시간의 경우의수를 다 비교한다
void tokenanalizer(char *data){
	char timeinfo[BUFLEN];
	char curTime[BUFLEN]; 
	_time possibletime; 

	getcurtime(curTime); 
	strcpy(timeinfo, data); 

	devidetoken(timeinfo); 
}

//시간의 정보를 구조체에 다 저장한다 
void devidetoken(char *timeinfo){
	int length,p,i,t,j,arrind, imsi, k;
	char buf[5][BUFLEN]; 
	char temp[BUFLEN]; 
	char temp2[BUFLEN];
	char timein[BUFLEN];

	char *ptr1; 
	char *ptr2; 
	char *ptr3; 
	char *ptr4; 
	char *ptr5; 


	char *ptr6;
	char *ptr7;
	Arr _Arr[BUFLEN]; 
	Arr _possi[5]; 
	char command[BUFLEN]; //명령어가 담김

	int starflag = 0;
	int dashflag = 0;
	int commaflag = 0;
	int devideflag = 0; 
	
	strcpy(timein, timeinfo);
	char *ptr = strtok(timein, " ");
	

	int commandlen = 0; 
	i=0; 

	//명령어를 알맞게 편집한다
	while(ptr!=NULL)
	{
		if(i == 5)
			break; 

		strcpy(buf[i], ptr);
		commandlen+=strlen(buf[i]); 
		length = strlen(buf[i]); 
		buf[i][length] = '\0'; 

		
		ptr = strtok(NULL, " "); 

		i++; 
	}

	strcpy(command, timeinfo+commandlen+5);
	

	//초기화
	for(k=0; k<5; k++)
	{
		_possi[k].count = 0; 	
	}


	//각 시간의 토큰마다 검사하는 for문 

	for(i=0; i<5; i++)
	{

		for(k=0; k<BUFLEN; k++)
		{
			_Arr[k].count = 0; 
			memset(_Arr[j].arr, 0, BUFLEN); 
		}	
		
	
		strcpy(temp, buf[i]); 

		length = strlen(temp)+1; 
		
		arrind = 0;
		commaflag = 0; 
		starflag = 0;
		dashflag = 0;
		devideflag = 0; 
		
		//,의 개수를 구해 형식을 지정
		for(p=0; p<length-1; p++) 
		{
			if(!isdigit(temp[p]))
			{
				if(buf[i][p] == ',')
					commaflag++; 
			}
	
		}
		
		t = 0; j=0;
		ptr1 = temp; 
		ptr2 = temp;
		ptr3 = temp; 
		ptr4 = temp; 
		ptr5 = temp; 

		//각 토큰의 길이만큼 캐릭터 단위로 검사
		for(p=0; p<length; p++) 
		{
			//개행이 아니고, 숫자도 아니라면
			if(!isdigit(temp[p]) && temp[p] != '\0')
			{
				if(temp[p] == ',')
				{
					commaflag = 1; 
					
					if(devideflag == 0 && dashflag == 0 && starflag == 0)
					{
						int ji; 
						ji = p-1; 

						ptr4 = temp + p -1; 
						//컴마 이전에 나온 정수를 구하기 위한 whil
						if(ji!=0)
						{
							while(ji!=0 && isdigit(*ptr4))
							{
								ptr4--; ji--; 
							}
							
							if(ji != 0)
								ptr4++; 
					

						}
						

						imsi =_Arr[arrind].count; 
						
						ptr5 = temp + p; 
						*ptr5++ = '\0';
						
						//주어진 구조체에 저장
						_Arr[arrind].arr[imsi] = atoi(ptr4);
						_Arr[arrind].count++; 

						
						ptr4 = temp+p+1;
						//가능한 케이스를 모두 저장
						putin_possi(&_possi[i], _Arr, arrind);

					
					}
					else if(devideflag == 0 && dashflag == 1 && starflag == 0)
					{

						ptr5 = temp+p;
						*ptr5++ = '\0'; 
						
						dashcalculate(ptr1, ptr2, &_Arr[arrind]);
						putin_possi(&_possi[i], _Arr, arrind);

						dashflag = 0; 
					}
					else if(devideflag == 1 && dashflag == 0 && starflag == 0)
					{
						//"대시와 디바이드가 컴마를 만났을 때\n"); 
						
						ptr1 = temp+p-1; 
						while(isdigit(*ptr1))
							ptr1--; 

						ptr1++; 

						devidecalculate(ptr1, &_Arr[arrind]);

						putin_possi(&_possi[i], _Arr, arrind);
					
						devideflag = 0; 

					}
					
					arrind++; //다음 배열에 옮겨서 계산해라
					
				}
				else if(temp[p] == '*')
				{


					//*을 만나면 주어진 배열을 모두 채워야함
				  	fillfulltime(i, &_Arr[arrind]);
					starflag = 1;
					
					
				}
				else if(temp[p] == '/')
				{
					devideflag = 1;
				
					if(dashflag==1)
					{
						
						ptr3 = temp + p; 
						*ptr3 = '\0'; 

					
						// /연산을 해줌
						dashcalculate( ptr1, ptr2, &_Arr[arrind]);
						
						dashflag = 0; 
					}
					else if(dashflag == 0 && starflag == 0)
					{
					}
					else if(starflag == 1 && dashflag == 0)
                                        {

                                               // printf("대시 플래그 + 스타 \n");


                                                starflag = 0;

                                        }



				}
				else if(temp[p] == '-')
				{
					int ji; ji = p-1;//- 하나 이전부터 시작하니깐 

					dashflag = 1; 
					

					ptr2 = temp+p-1; //-이 맨 앞에 나올리는 없으니깐 음수 인덱스는 고려하지 않아도 됨 
					
					if(ji!=0)
					{
						while(ji!=0 && isdigit(*ptr2))
						{
							
							ptr2--; ji--;
						}
						if(ji!=0)
							ptr2++; 
					}
					
					
					
					ptr1 = temp+p;
					*ptr1++ = '\0'; 

				

				}
			
			}//개행을 만나면 이전에 만났던 오퍼레이터에 따라 계산을 함
			else if(temp[p] == '\0')
			{
				if(devideflag == 0 && dashflag == 0 && starflag == 0 & commaflag == 0)
				{//정수일때 (연산자 없을 때)
			
					_possi[i].arr[0] = atoi(temp);
					_possi[i].count = 1; 	
				
				}
				if(devideflag == 0 && dashflag == 0 && starflag == 0 & commaflag == 1)
				{//1,2,3



					imsi =_Arr[arrind].count; 
					_Arr[arrind].arr[imsi] = atoi(ptr5);
					_Arr[arrind].count++; 
					
					putin_possi(&_possi[i], _Arr, arrind);
				}
				if(devideflag == 0 && dashflag == 0 && starflag == 1 & commaflag == 0)
				{//별이 들어왔을 때 , *
					fillfulltime(i, &_Arr[arrind]); 
					putin_possi(&_possi[i], _Arr, arrind);
					starflag = 0; 
				}
				else if(devideflag == 1)
				{// '/'가 있을때

					ptr1 = temp+p-1; 
					while(isdigit(*ptr1))
						ptr1--; 

					ptr1++; 

					// '/'연산을 수행하는 함수, 주어진 크기만큼 배열에 넣음
					devidecalculate(ptr1, &_Arr[arrind]);

					//가능한 경우의 수를 구조체에 넣음
					putin_possi(&_possi[i], _Arr, arrind);
				
					devideflag = 0; 
				}
				else if(dashflag == 1)
				{
					//1) 2-10/3은 /를 볼 때 dashflag를 0으로 만들기 때문에 고려하지 않음
					//2) 3-10 이런걸 고려해서 만들었음
					
					
					ptr1 = temp + p-1; 
		
					while(isdigit(*ptr1))
						--ptr1; 
					++ptr1; 
					
					
					dashcalculate(ptr1, ptr2, &_Arr[arrind]);
					putin_possi(&_possi[i], _Arr, arrind);
					
				}
			
			}

	
		
		}


	}
/*
	printf("~~~~~~~~~~~~~~~~~~~~~결과 확인~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	for(int foo=0; foo<5; foo++)
	{
		for(int fee=0; fee<_possi[foo].count; fee++)
			printf("%d  ", _possi[foo].arr[fee]);

		printf("\n");
		printf("count : %d\n", _possi[foo].count);

	}
	printf("\n");
*/
	possiblecase(_possi, command, timeinfo);
}


//각 토큰의 경우의 수에 따라 현재 시간과 주어진 시간이 같다면 쓰레드 실행하는 함수
int possiblecase(Arr *_possi, char command[BUFLEN], char fulldata[BUFLEN]){


	int a,b,c,d,e;
	int count =1; 
	int sec; 	
	
	_command gocommand;
	strcpy(gocommand.fulldata, fulldata); 
	
	_time curTime;
	pthread_t tid;

	a=0; b=0; c=0; d=0; e=0;

	strcpy(gocommand.command, command); 
	getcurTime(&curTime);

	//0초에 실행해야 하기 때문
	if(curTime.tm_sec != 0)
		return 1;

	//각 토큰의 경우의 수에 따라 현재 시간과 주어진 시간이 같다면 쓰레드 실행
	for(a=0; a<_possi[0].count; a++)
	{
		for(b=0; b<_possi[1].count; b++)
		{
			for(c=0; c<_possi[2].count; c++)
			{	
				for(d=0; d<_possi[3].count; d++)
				{	
					for(e=0; e<_possi[4].count; e++)
					{
											

						if(curTime.min == _possi[0].arr[a]&&curTime.hr == _possi[1].arr[b]&&curTime.mday == _possi[2].arr[c]&&curTime.tm_mon == _possi[3].arr[d]&&curTime.tm_wday == _possi[4].arr[e])
						{
							//쓰레드 생성합니다
							if(pthread_create(&tid, NULL, systemcommand, (void*)&gocommand)!=0){
								fprintf(stderr, "system command thread error\n");
							}
							
						}
					}
				}
			}
		}
	}

}
 
 
//system함수를 실행시키는 쓰레드의 함수 
void *systemcommand(void *arg){
	int ret; 
	int length; 
	FILE *fp;
        char putfp[BUFLEN];

	_command *ptr;
	ptr = (_command *)arg;
	
	ret = system(ptr->command);

	/**log에 텍스트 쓰기**/ 

	writeinlogdir(ptr->command);

}

//crontab에서 명령어가 삭제됐을 경우 crond의 노드도 삭제해주는 함수
void removenode(char *data){

        finfo *tmp= list;
        finfo *cur = list;
        finfo *pre = NULL;
	int cnt = 0; 
	int num = 0; 
	int flag = 0; 

	//링크드리스트를 검색하며 원하는 데이터를 찾음 
	while(tmp!=NULL){
		
		if(!strcmp(tmp->data, data))
			break; 
		tmp=tmp->next;
		cnt++;
	}



	for(int i=0; i<cnt; i++){
                pre = cur;
                cur = cur->next;
        }
        if(cnt==0)
                list = list->next;
        else
                pre->next = cur->next;

	free(cur);
	traverse();
   
    
        return;


}

//log에 찍기위해 형식에 맞게 주어진 시간을 구하는함수
void writeinlogdir(char *data){

	struct tm Tm;
        time_t current_time;
	//정수로 주어지기 때문에 인덱스로 여김
	char *week[7] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};
	char *month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	char *curweek;
	char *curmonth;
	char curTime[BUFLEN];
	char info[BUFLEN];
	FILE *fp;


        current_time = time(NULL);

        localtime_r(&current_time, &Tm);

	curweek = week[Tm.tm_wday];
	curmonth = month[Tm.tm_mon]; 

	sprintf(curTime, "[%s %s %02d %02d:%02d:%02d %04d] run %s\n", curweek, curmonth, Tm.tm_mday, Tm.tm_hour, Tm.tm_min, Tm.tm_sec, Tm.tm_year+1900,data); 
	
	fp = fopen(logdir, "a+");
	
	fputs(curTime, fp); 
	
	fclose(fp);

}
//가능한 경우의 수를 구조체에 저장하는 함수
void putin_possi(Arr *possi, Arr *array, int arrind){

	int tot=0; 

	for(int i=0; i<=arrind; i++)
	{
		for(int p=0; p<array[i].count; p++)
		{
			possi->arr[tot] = array[i].arr[p]; 
			++tot; 
		}

	}
	
	possi->count = tot;

}

//별이 들어올 경우 토큰에 주어진 시간만큼 채워주는 함수
void fillfulltime(int step, Arr *array){
	int i, ind = 0; 
	int start, end; 
	
	if(step == 0)
	{
		start = 0; end = 59; 
		//분 : 0~59
	}
	else if(step==1)
	{
		start = 0; end = 59; 
		//시 : 0~59

	}
	else if(step==2)
	{
		start = 1; end = 31; 
		//일 : 1-31

	}
	else if(step==3)
	{
		start = 1; end = 12; 
		//월 : 1-12

	}
	else if(step==4)
	{
		start = 0; end = 6; 
		//요일 : 0~6
	}

	for(i=start; i<=end; i++)
	{
		array->count++; 
		array->arr[ind++] = i; 
	}

}

//슬래시가 올경우 값만큼 나눠서 배열에 넣는 함수
void devidecalculate(char *ptr, Arr *array){
	
	int src , i, p; 
	int start; int end; 
	int first; 
	int cnt = 0; 
	char tmp[BUFLEN];
	strcpy(tmp, ptr); 


	src = atoi(tmp); 

	//나눔 연산을 할 처음값
	start = array->arr[0];
	p = array->count; 
	//끝값
	end = array->arr[p-1]; 
	memset(array->arr, 0, BUFLEN); 

	p=0; 

	//차이/src만큼 구해서 배열에 저장
	first = start-1+src; 
	for(i = first; i<=end; i+=src)
	{
		array->arr[p++] = i; 	
		++cnt; 
	}

	array->count = cnt; 
	

}

//-양옆의 수의 범위만큼 배열에 저장하는 함수
void dashcalculate(char *ptr1, char *ptr2, Arr *array){
	char temp1[BUFLEN];
	char temp2[BUFLEN];
	int start, end;
	int i=0;
	int p=0; 

	strcpy(temp2, ptr1);
	strcpy(temp1, ptr2);


	//printf("이제부터 dashcalcu입니다\n");
	start = atoi(temp1);
	end = atoi(temp2); 


	//처음과 끝만큼 저장함
	for(i=start; i<=end; i++)
	{
		array->arr[p] = i; 
		++p; 
	}
	
	array->count = p;
}
	


//디렉토리가 추가됐을 경우 링크드 리스트에 추가하는 함수
void add(char *data){
        int cnt = 0;  
        char n;  
        char putfp[BUFLEN];
        FILE *fp; 
	char *curTime;
	finfo *newnode; 

	getcurtime(curTime);
       
	//동적할당
	newnode = (finfo*)malloc(sizeof(finfo));
        strcpy(newnode->data, data); 
        strcpy(newnode->createtime, curTime); 
	
	newnode->next = NULL; 

        if(list == NULL){
                list = newnode;
        }
        else {
                finfo *cur = list;
                while(cur){
                        ++cnt; //n개가 있다=> Index는 N-1
                        if(cur->next == NULL){
                                cur->next = newnode;//헤드와 가까운 노드가 가장 먼저 저장된것임
                                break;
                        }
                        cur=cur->next;
                }
        }
	traverse();
}
//현재시간을 가져오는함수
void getcurtime(char *curTime){
	
	struct tm Tm;
        time_t current_time;

	current_time = time(NULL);

        localtime_r(&current_time, &Tm);

        //형식에 맞는 시간 
        sprintf(curTime,"%02d 시[%02d], 일[%02d], 월[%02d], 요일[%02d]", Tm.tm_min,Tm.tm_hour,Tm.tm_mday,Tm.tm_mon,Tm.tm_wday);
	

}

//현재시간을 가져와 주어진 구조체에 저장하는 함수
void getcurTime(_time *curTime){
	
	struct tm Tm;
        time_t current_time;

	current_time = time(NULL);

        localtime_r(&current_time, &Tm);

	
	curTime->min = Tm.tm_min;
	curTime->hr = Tm.tm_hour;
	curTime->mday = Tm.tm_mday;
	curTime->tm_mon = Tm.tm_mon;
	curTime->tm_mon++;//0-11이기 때문에 하나 증가 
	
	curTime->tm_wday = Tm.tm_wday;
	curTime->tm_sec = Tm.tm_sec;

	
	
}
//0초에 실행하기 위해 현재시간의 초를 구하는 함수
void getsec(int *sec){
     
        struct tm Tm;
        time_t current_time;

        current_time = time(NULL);

        localtime_r(&current_time, &Tm);
    
        *sec = Tm.tm_sec;

     
}

//디버깅용 함수
void traverse(){

	//printf("traverse() checking\n");
/*
        finfo *cur = list;
        while(cur!=NULL){
                printf("%s", cur->data);
                cur=cur->next;
        }
        printf("\n");*/
}

















