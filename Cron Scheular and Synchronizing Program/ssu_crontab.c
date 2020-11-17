#include "ssu_crontab.h"

char dir[BUFLEN]; 
char logdir[BUFLEN]; 
int totalcnt; 
node *list; 

int main(){
        struct stat statbuf;
        struct timeval start_time;
        struct timeval end_time;
        double execution_time;

        gettimeofday(&start_time, NULL);


	char buf[BUFLEN]; 
	list = NULL; 
	FILE *fp; 

	getcwd(buf, BUFLEN);
	
	sprintf(dir, "%s/%s", buf, "ssu_crontab_file"); 
	sprintf(logdir, "%s/%s",buf , "ssu_crontab_log"); 

	//로그디렉토리가 존재하지 않을 경우 생성
	if(access(logdir, F_OK)<0)
	{
		fp = fopen(logdir, "w");
		fclose(fp);
	}

	//크론탭 파일이 존재하지 않을 경우 생성 
	if(access(dir, F_OK)<0)
	{
		fp = fopen(dir, "w");
		fclose(fp);
	}
	else 	//링크드리스트 초기화
		listinit(); 

	while(1){
		printf("20170771> ");
		if(crontab()<0)//exit받으면 탈출해서 시간ᄍᆔᆨ
			break;
	}

	gettimeofday(&end_time, NULL);

	printf("%f\n", (double)(end_time.tv_usec)-(double)(start_time.tv_usec));




}


int crontab(){
	char choice[7]; 
	char buf[BUFLEN];
	char input[BUFLEN]; 
	char *ptr1;
	char *ptr2; 
	int p;
	int i; 

	fgets(buf, BUFLEN, stdin); 
	

	if(buf[0]=='\n')
		return 1; 

	p = strlen(buf)-1;
	buf[p] = '\0';


	//exit을 입력했을 경우 시간 구하고 프로세스 탈출
	if(!strcmp(buf, "exit")){
		return -1; 
	}



	ptr2 = buf; 
	for(i=0; i<p+1; i++){
		if(buf[i] == ' '){
			ptr1 = buf+i;
			*ptr1 = '\0';
			strcpy(choice, ptr2); 
			break;
		}
	}

	ptr1=buf+i+1; 
	strcpy(input, ptr1); 
	

	//add를 입력받았을 경우
	if(!strcmp(choice, "add"))
	{
		//예외처리
		if(timeinputcheck(input)<0)
			return 1; 

		add(input);//링크드리스트 추가 
		totalcnt ++; 
		
		
		
		traverse();
		
	}
	//remove입력 받았을 때 
	else if(!strcmp(choice, "remove"))
	{

		if(input[0] == '\0')
		{
			fprintf(stderr, "번호 입력을 안하셨습니다\n"); 
			return 1; 
		}
	
		p = atoi(input);
	
		if(p<0 || p>=totalcnt)//범위를 벗어났을 때 예외처리
		{
			fprintf(stderr, " 없는 번호 입니다\n"); 
			return 1; 
		}

		
		
		//링크드리스트에서 노드 삭제 
		removenode(p);
		totalcnt --; 



		traverse();

		readd();//파일에 다시 적음

	}
	else
	{
		fprintf(stderr, "%s는 잘못된 명령어 입니다\n",choice);
		return 1;
	}
	
	
}

//입력된 시간을 확인하는 함수
int timeinputcheck(char *buf){

	int i;
	char intime[6][TIMELEN];
	char input[BUFLEN];
	int length; 
	int operatorflag; 
		
	char *ptr1;
	char *ptr2; 
	char operation; 
	int digit;
	
	i=0; 

	strcpy(input, buf);

	char *ptr = strtok(input, " "); 
	
	//토큰으로 나눔
	while(ptr!=NULL)
	{
		strcpy(intime[i], ptr);
		length = strlen(intime[i]);
		intime[i][length] = '\0';//operationException에서 사용하기 위해
		++i;	

		ptr = strtok(NULL, " ");
		if(i==5)
			break;
	}

	for(int p = 0; p<5; p++)
	{

		length = strlen(intime[p])+1;
		/**	+1을 하는 이유는 NULL은 strlen에 카운트가  안되지만
		  아래에 NULL을 사용해야 하는 경우가 생기기 때문이다	**/
		ptr1 = intime[p];
		ptr2 = intime[p];
			
		operatorflag = 0; 


		if(!strcmp(intime[p] , "*"))
			continue;

		for(int j = 0; j<length; j++)
		{
			
		
			
			if(intime[p][j] == '\0')
			{

				digit = atoi(ptr1);
				
				if(operatorflag == 0){
					/***문자열이 끝나지 이전에 연산자가 오지 않았다면
					  	= 숫자로만 이루어져 있다면 ****/
					if(operationException(p, 0, digit)<0)
						return -1;

					break; 
				}

				//예외처리 확인
				if(operationException(p, 0, digit)<0)
					return -1;	
				break; 
			}//연산자를 받았을 경우
			else if(!isdigit(intime[p][j]))
			{
				operatorflag = 1; 

			
				operation = intime[p][j];
				digit = atoi(ptr1);
				ptr1 = intime[p] + j + 1; 

				intime[p][j] = '\0';

				//익셉션 확인해봄 
				if(operationException(p, operation, digit)<0)
					return -1; 	
			}
			else
			{
				//break를 해주지 않으면 첫번째 조건문에 들어가기 때문에 
			}
		

		}
	}

	return 1; 

}

//예외처리 하는 함수
int operationException(int timeflag, char op, int digit){

	if(op == '/')
	{
		
	}
	else if(op=='-')
	{

	}
	else if(op==',')
	{

	}
	else if(op=='*')
	{
		return 1; 
	}
	else if(op == 0)//숫자로만 이루어져 있음  
	{
	}
	else
	{
		fprintf(stderr, "%c는 잘못된 연산자 입니다\n", op); 
		return -1; 
	}

	
	if(timeflag==0)
	{
		if(digit<0 || digit >59)
		{
			fprintf(stderr, " %d 는 '분' 단위에 맞지 않습니다\n", digit);
			return -1;
		}
	}
	else if(timeflag==1)
	{
		if(digit<0 || digit>23)
		{
			fprintf(stderr, " %d 는 '시' 단위에 맞지 않습니다\n", digit);
			return -1; 
		}
	}
	else if(timeflag==2)
	{
		if(digit<1 || digit>31)
		{
			fprintf(stderr, " %d 는 '일' 단위에 맞지 않습니다\n", digit);
			return -1; 
		}
	}
	else if(timeflag==3)
	{
		if(digit<1 || digit>12)
		{
			fprintf(stderr, " %d 는 '월' 단위에 맞지 않습니다\n", digit);
			return -1; 
		}
	}
	else if(timeflag==4)
	{
		if(digit<0 || digit>7)
		{
			fprintf(stderr, " %d 는 '요일' 단위에 맞지 않습니다\n", digit);
			return -1; 
		}
	}

	return 1; 

}

//링크드리스트의 노드를 삭제함
void removenode(int num){


	if(num<0){
		printf("해당 번호가 없습니다\n");
		return; 
	}

	node *cur = list;
	node *pre = NULL;

	//해당하는 번호로 가서 노드를 삭제함
	for(int i=0; i<num; i++){
		pre = cur;
		cur = cur->next;
	}

	//이전노드와 다음 노드를 잇는 과정
	if(num==0)
		list = list->next;
	else
		pre->next = cur->next;

	

	writeinlog("remove", cur->data); 	

	free(cur);
	
	
	return;
	
}

//다시 추가하는 함수
void readd(){
	FILE *fp;
	int cnt = 0; 
	if((fp = fopen(dir, "w"))==NULL){
		fprintf(stderr, "fopen err\n");
		exit(1);
	}
	node *cur = list; 
	while(cur!=NULL){
//		rewrite(fp, cur->data);


		fprintf(fp, "%d. %s\n", cnt, cur->data);
//		fprintf(fp, ". %s\n", cur->data);
/*		if(fputs("why???", fp)==EOF){
			printf("...\n");
		}
		*/
		cur=cur->next;
		cnt++;
	}
	
	fclose(fp);
}
void rewrite(FILE *fp, char *data){

	int cnt = 0; 
	char num[3]; 
	

}

//노드에 새로 추가하는 함수
void add(char *data){
	int cnt = 0; 
	char n; 
	char putfp[BUFLEN];
	FILE *fp; 

	node *newnode = (node*)malloc(sizeof(node));
	strcpy(newnode->data, data); 
	newnode->next = NULL; 

	if(list == NULL){
		list = newnode;
	}
	else {//가장 최근에 받은 값은 헤드에서 가장 멀리 떨어져있음
		node *cur = list;
		while(cur){
			++cnt; //n개가 있다=> Index는 N-1
			if(cur->next == NULL){
				cur->next = newnode;
				break;
			}
			cur=cur->next;
		}
	}

	//파일을 열고 내용을 쓴다
	fp = fopen(dir, "a+"); 

	n = '\0'; 
	sprintf(putfp, "%d. %s\n", cnt,data); 
	fputs(putfp, fp);
	fputs(&n, fp);

	fclose(fp); 

	writeinlog("add", data); 	
	
}

//형식대로 로그에쓰는 함수
void writeinlog(char order[BUFLEN], char data[BUFLEN]){
	
	FILE *fp;
	char putfp[BUFLEN]; 
	char curTime[BUFLEN]; 
	struct tm Tm;
        time_t current_time;
	int len; 

	time(&current_time);

        //형식에 맞는 시간 

	sprintf(curTime, "%s", ctime(&current_time)); 

	fp = fopen(logdir, "a+"); 

	len = strlen(curTime); 
	curTime[len-1] = '\0';

	sprintf(putfp, "[%s] %s %s\n", curTime, order, data);



	fputs(putfp, fp);



	fclose(fp);

	
}

//디버깅용 함수
void traverse(){

	node *cur = list; 
	while(cur!=NULL){
		cur=cur->next;
	}
}
//프로그램이 시작됐을 때 파일을 초기화 하는 함수
void listinit(){

	FILE *fp; 
	char buf[BUFLEN];
	char *ptr;
	int length; 
	int i; 
	
	//파일이 없다면 안해도 됨
	if(access(dir, F_OK)<0){
		return; 
	}

	if((fp = fopen(dir, "r"))==NULL){
		fprintf(stderr, "no file\n");
		exit(1);
	}

	while(!feof(fp)){
	
		i = 0; 
		fgets(buf,BUFLEN,fp);
		length = strlen(buf); 
		
		//fgets는 개행도 받기때문에 처리해줌 
		if(buf[0] == '\n' || buf[0] == '\0')
			break;

		buf[length-1] = '\0'; 
	
		while(buf[i] != '.')
			i++; 
		i+=2; 	
		ptr = buf+i; 

		//한줄씩 노드에추가
		addinit(ptr); 
		memset(buf, 0, BUFLEN);
		
		totalcnt++;
	}

	traverse();
	fclose(fp); 

}


//프로그램을 처음 시작했을 때 노드를 추가하는 함수
void addinit(char *data){
	
	node *newnode = (node*)malloc(sizeof(node));
	strcpy(newnode->data, data); 
	newnode->next = NULL; 

	if(list == NULL){
		list = newnode;
	}
	else {
		node *cur = list;
		while(cur){
			
			if(cur->next == NULL){
				cur->next = newnode;
				break;
			}
			cur=cur->next;
		}
	}


}
