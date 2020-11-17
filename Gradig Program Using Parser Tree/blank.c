
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "blank.h"

char datatype[DATATYPE_SIZE][MINLEN] = { "int", "char", "double", "float", "long"
			, "short", "ushort", "FILE", "DIR","pid"
			,"key_t", "ssize_t", "mode_t", "ino_t", "dev_t"
			, "nlink_t", "uid_t", "gid_t", "time_t", "blksize_t"
			, "blkcnt_t", "pid_t", "pthread_mutex_t", "pthread_cond_t", "pthread_t"
			, "void", "size_t", "unsigned", "sigset_t", "sigjmp_buf"
			, "rlim_t", "jmp_buf", "sig_atomic_t", "clock_t", "struct" };

//precedence 값이 작을수록 연산자 우선순위가 높음  
//precedence 값이 클수록 우선순위가 낮음 
operator_precedence operators[OPERATOR_CNT] = {
	{"(", 0}, {")", 0}
	,{"->", 1}
	,{"*", 4}	,{"/", 3}	,{"%", 2}
	,{"+", 6}	,{"-", 5}
	,{"<", 7}	,{"<=", 7}	,{">", 7}	,{">=", 7}
	,{"==", 8}	,{"!=", 8}
	,{"&", 9}
	,{"^", 10}
	,{"|", 11}
	,{"&&", 12}
	,{"||", 13}
	,{"=", 14}	,{"+=", 14}	,{"-=", 14}	,{"&=", 14}	,{"|=", 14}
};
/*STD_DIR 의 답과 ANS_DIR 의 답을 비교하는 함수*/
void compare_tree(node *root1, node *root2, int *result)
{
	//root1 : STD_DIR 의 답의 루트
	//root2 : ANS_DIR 의 답의 루트 
	node *tmp;
	int cnt1, cnt2;

	//STD_DIR나 ANS_DIR 가 비었다면 
	if (root1 == NULL || root2 == NULL) {
		//*result 는 false 이다 
		*result = false;
		//ssu_core.c 로 돌아간다 
		return;
	}

	//STD_DIR나 ANS_DIR 가 비지 않았다면  

	//STD_DIR 의 루트노드가 대소비교 연산자일 경우에 
	if (!strcmp(root1->name, "<") || !strcmp(root1->name, ">") || !strcmp(root1->name, "<=") || !strcmp(root1->name, ">=")) {

		//STD_DIR 의 루트노드와 ANS_DIR 의 루트노드가 같지 않을 때 
		//root1 과 root2 가 같지 않을 경우에 
		if (strcmp(root1->name, root2->name) != 0) {

			//root2가 < 라면 
			if (!strncmp(root2->name, "<", 1))
				//root2 에 > 를 대입 
				strncpy(root2->name, ">", 1);

			//root2 가 > 라면
			else if (!strncmp(root2->name, ">", 1))
				//root2에 < 대입
				strncpy(root2->name, "<", 1);

			//root2 가 <= 라면
			else if (!strncmp(root2->name, "<=", 2))
				//root2에 >= 대입
				strncpy(root2->name, ">=", 2);

			//root2 가 >= 라면
			else if (!strncmp(root2->name, ">=", 2))
				//root2에 <= 대입
				strncpy(root2->name, "<=", 2);

			//root2의 순서를 바꿈 
				//비교때문이낙.. 
					//이유 모호성을 갖는 순서의 경우의 수를 모두 고려하기 위해서/....
			root2 = change_sibling(root2);
		}
	}

	//root1 과 root2 노드의 이름이 다를경우 
	if (strcmp(root1->name, root2->name) != 0) {
		*result = false; //틀림 
		return;
	}

	//각각의 root1 과 root2 의 child_head 중 하나만 비었을 경우 
	if ((root1->child_head != NULL && root2->child_head == NULL)
		|| (root1->child_head == NULL && root2->child_head != NULL)) {
		*result = false;  //틀림
		return;
	}

	//root1의 child_head가 존재할 경우 
	else if (root1->child_head != NULL) {

		//root1과 root2의 형제노드의 개수가 다를경우   
		if (get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)) {
			*result = false; //틀린답 
			return;
		}

		//root1이 대입연산자 중 == 이거나 != 일 경우
		if (!strcmp(root1->name, "==") || !strcmp(root1->name, "!="))
		{
			//자식노드 compare  
			compare_tree(root1->child_head, root2->child_head, result);
			//자식노드가 틀린답일 경우 *result == false 후 return 됨 

			//*result 가 false 일 경우 root2의 자식노드의 위치를 바꾼 후 한번더 compare 
				//이유: 다수의 답이 정답이 되는 경우, 모호성을 갖는 연산자가 계산이 존재하기 때문 
					//ex) if(A==B), if(B==A) 동일함 
			if (*result == false)
			{
				*result = true;
				root2 = change_sibling(root2); //root2 의 prev 와 next 로 연결돼있는 노드들의 위치를 바꾼다
				compare_tree(root1->child_head, root2->child_head, result); //바뀐 root2들을 기준으로 다시 compare한다
			}
		}

		//(a+b)*(c+d) 의 *
		//노드의 종류가 아래와 같을 경우
		//이건 해본 .. get_operator 를 통해 얻은 연산자 (상위노드의 연산자 ) 의 종류가 아래와 같을 경우 

		//root2(정답)노드의 연산자 노드의 자식노드들의 순서를 바꿔가며 root1(학생답)의 연산자노드의 자식들과 비교하는 조건문 
			//이유: 아래 연산자는 순서가 바뀌어도 답이 동일한 경우가 존재하기 때문에
		else if (!strcmp(root1->name, "+") || !strcmp(root1->name, "*")
			|| !strcmp(root1->name, "|") || !strcmp(root1->name, "&")
			|| !strcmp(root1->name, "||") || !strcmp(root1->name, "&&"))
		{
			//root1과 root2의 형제노드의 개수가 다를경우   
			if (get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)) {
				*result = false; //틀림
				return;
			}

			//tmp는 root2의 자식노드
			tmp = root2->child_head;

			//tmp->prev가 없을 때 까지 tmp를 prev로 옮긴다 
				//이유: prev와 next로 연결된 모든 노드들을 돌아가며 root1(학생답)의 동일한 level에 있는 노드들과 비교하기 위해서
			while (tmp->prev != NULL)
				tmp = tmp->prev;

			//tmp가 null일때까지 
				//tmp와 prev, next 로 이루어진 노드를 모두 탐색할 때 까지		
			while (tmp != NULL)
			{

				//root1(학생답) 연산자의 child_head 노드와 root2의 자식노드를 compare
				compare_tree(root1->child_head, tmp, result);

				//같은 것이 있다면 
				if (*result == true)
					break; //그만 비교

				//같은것이 없다면 
				else {

					//tmp의 next 가 존재한다면
						//위의 compare_tree의 *result결과가 false 라고 해도 tmp의 next 가 존재한다면 true로 바꿈

						//이유: 맞나 안맞나 몰라..확인할것 .. 
					if (tmp->next != NULL)
						*result = true;

					//tmp(ans)를 tmp->next 로 옮긴다 
					tmp = tmp->next;
				}
			}
		}
		//대소비교일 경우 
		else {
			//대소비교는 순서가 중요하기 떄문에 위의 if문처럼 경우의 수를 고려하지 않음  
			compare_tree(root1->child_head, root2->child_head, result);
		}
	}

	//root1 의 child_head를 먼저 탐색 한 뒤 next 를 탐색 
	//root1-> next 가 존재한다면 
	if (root1->next != NULL) {

		// root1과 root2 의 각각의 prev, next 로 연결된 노드들의 개수가 같지 않다면  
		if (get_sibling_cnt(root1) != get_sibling_cnt(root2)) {
			*result = false; //틀림
			return;
		}

		//..내생각엔 
		//. 상위노드에 연산자 노드가 존재한다면 stud 의 그 밑에 노드 의 prev, next 로 연결돼있는 노드들 중에서 
		//ans 의 같은 레벨에 있는 노드랑 일치하는게 하나라도 존재한다면 

		if (*result == true)
		{

			//tmp는 root1의 가장 prev한 노드의 parent, 즉 연산자의 주소값을 가리킴 
			tmp = get_operator(root1);

			//tmp가 아래 연산자들 중 하나라면 
			if (!strcmp(tmp->name, "+") || !strcmp(tmp->name, "*")
				|| !strcmp(tmp->name, "|") || !strcmp(tmp->name, "&")
				|| !strcmp(tmp->name, "||") || !strcmp(tmp->name, "&&"))
			{
				//tmp에 정답(root2)노드의 주소값을 대입 
				tmp = root2;

				//tmp를 tmp의 가장 prev한 노드의 주소값을 가리키게 하는 while문 
				while (tmp->prev != NULL)
					tmp = tmp->prev;

				//tmp가 더이상 존재하지 않을 때 까지 tmp의 next를 탐색하는 while문 
				while (tmp != NULL)
				{
					//root1->next 노드와 tmp노드를 tmp가 더이상 존재하지 않을 때 까지 compare
					compare_tree(root1->next, tmp, result);

					//compare를 통해 *result 가 true가 될 경우
					if (*result == true)
						break;
					//false 이더라도 tmp를 next 하며 비교
					else {
						if (tmp->next != NULL)
							*result = true;
						tmp = tmp->next;
					}
				}
			}

			else
				//노드를 비교
				compare_tree(root1->next, root2->next, result);
		}
	}
}

int make_tokens(char *str, char tokens[TOKEN_CNT][MINLEN])
{
	/*#include <string.h>
		char *strpbrk(const char *, const char *);
		const char * : 검색을 수행할 문자열
		const char * : 검색어들을 포함하고 있는 문자열
		리턴 : str2 의 문자들 중 str1 의 문자들과 첫번째로 일치하는 문자를 가리키게 된다.
				만일 str1 의 널 문자 이전 까지 일치하는 것이 없다면 널을 리턴한다.
		출력시 첫문자가 일치하는 문자부터 null문자까지 출력

	*/
	/*
	#include <string.h>
		char* strncat(char* destination, char* source, size_t num);
		char* destination : 배열을 가리키는 포인터로, C 문자열을 보관하며 (널 문자를 포함한)
		합쳐진 문자열이 들어갈 만큼 충분히 크기가 커야만 한다.
		char* source
		size_t num
		source 의 num개의 문자들이 destination끝에 덧붙임
	*/
	/*
	strstr
	*/
	char *start, *end;
	char tmp[BUFLEN];
	char str2[BUFLEN];
	char *op = "(),;><=!|&^/+-*\""; 
	int row = 0;
	int i;
 	int isPointer;
	int lcount, rcount;
	int p_str;
	
	//초기화 memset 0으로 
	clear_tokens(tokens);

	start = str;
	
	//검사하는 문자열(str) 에 "gcc" 나 datatype이 존재하는지 판단하는 조건문
	if(is_typeStatement(str) == 0) 
		return false;	
	
	while(1)
	{
		//검사하는 문자열(str)에 특수문자가 존재할 때 첫 특수문자의 포인터를 end에 대입
		if((end = strpbrk(start, op)) == NULL)
			break;//없으면 while문으 빠져나감 
		
		//특수문자가 문자열의 처음에 존재할 때 
		//특수문자가 있는 주소부터 다음 특수문자가 있기 전까지 검사하여 조건에 맞게 토큰에 저장하기 위해 
		if(start == end){
			//--나 ++가 문자열의 처음에 존재하는 경우
			
			//--나 ++로 시작하는 경우
			if(!strncmp(start, "--", 2) || !strncmp(start, "++", 2)){
				if(!strncmp(start, "++++", 4)||!strncmp(start,"----",4))//++++, ----처럼 틀린 연산자를 사용 할 경우 false 를 리턴  
					return false;

				//대입 연산자 에 공백이 있을 때 공백을 제거함
				//왼쪽 공백을 제거한 문자열의 첫 주소가 숫자, 소문자, 혹은 대문자의 주소일 경우
				if(is_character(*ltrim(start + 2))){
					if(row > 0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]))
						return false; 
						
					//다음 특수문자를 찾음 
					end = strpbrk(start + 2, op);//다음 특수문자가 발견된 포인터주소를 end에 대입 
					if(end == NULL)//다음 특수문자가 없다면 
						end = &str[strlen(str)];//end는 검사하는 문자열의 마지막주소를 가리킴 
					//두번째 특수문자의 주소가 첫 시작주소와 같아질 때까지 
					while(start < end) {
						if(*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
							return false;
						//특수문자부터 숫자까지 tokens[row]에 대입
						//ex) ++3; tokens[row] => + + 3 이 들어감  
						else if(*start != ' ')
							strncat(tokens[row], start, 1);//tokens 배열뒤에 시작주소가 가리키는 문자를 하나 붙인다 
						start++;	
					}
				}
				
				//그전 토큰의 마지막 문자가  숫자 혹은 문자인 경우 
				else if(row>0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){
					//++, 혹은 --가 그전 토큰이 있다면 false 리턴  	
					if(strstr(tokens[row - 1], "++") != NULL || strstr(tokens[row - 1], "--") != NULL)	
						return false;


					memset(tmp, 0, sizeof(tmp));
					strncpy(tmp, start, 2);//tmp에 연산자 ++ or --를 복사  
					strcat(tokens[row - 1], tmp); // 이전token에 연산자를 붙여넣는다
					start += 2;//연산자의 주소값을 넘어감 
					row--;// 토큰 ++뒤에 문자를 추가하기 위해 같은 행을 탐색해야 함 
						//따라서 row--를 해줌
				}
				//그전 토큰의 마지막 문자가  숫자 혹은 문자가 아닌 경우
					//++x
				else{
					memset(tmp, 0, sizeof(tmp));
					strncpy(tmp, start, 2); //연산자를 tmp에 복사하고 
					strcat(tokens[row], tmp);//현재행의 뒤에 붙여넣는다 
					start += 2;
				}
			}
			//ex ) fcntl(fd1, fd_cloexec) == -1
			//에서 strat 가 ==의 첫 =의 주소값일 때 
			else if(!strncmp(start, "==", 2) || !strncmp(start, "!=", 2) || !strncmp(start, "<=", 2)
				|| !strncmp(start, ">=", 2) || !strncmp(start, "||", 2) || !strncmp(start, "&&", 2) 
				|| !strncmp(start, "&=", 2) || !strncmp(start, "^=", 2) || !strncmp(start, "!=", 2) 
				|| !strncmp(start, "|=", 2) || !strncmp(start, "+=", 2)	|| !strncmp(start, "-=", 2) 
				|| !strncmp(start, "*=", 2) || !strncmp(start, "/=", 2)){
				
				//tokens에 ==를 넣는다
				strncpy(tokens[row], start, 2);
				//staart 를 == 뒤로 옮긴다 
				start += 2;
			}
			//구조체 포인터일 경우 
			else if(!strncmp(start, "->", 2)) 
			{
				//end 는 -> 이후에 나오는 특수문자
				end = strpbrk(start + 2, op);
				
				//특수문자가 없다면
				if(end == NULL)
					end = &str[strlen(str)]; //마지막 문자를 가리킴

				//공백을 제외하고 tokens에 특수문자 혹은 문자열의 끝까지 넣어줌 
				while(start < end){
					if(*start != ' ') //공백이 아니라면 
						strncat(tokens[row - 1], start, 1); //넣어줌 
					start++;
				}
				row--; //같은 행에 저장하기 위해 
			}
			/*&와 &이후의 단어를 tokens에 저장*/
			else if(*end == '&')
			{
				// 답의 처음에 &가 나오거나
					//&가 나오기 하나 전 단어에 특수문자가 있는 경우--> ex ) &&
				if(row == 0 || (strpbrk(tokens[row - 1], op) != NULL)){
					end = strpbrk(start + 1, op);
					if (end == NULL) //&이후에 특수문자가 없다면 (&이후에 단어가 나오고 끝난다면) end포인터를 &이후 나온 단어의 null로 옮김
						//ex) creat&tid2 => end를 
						end = &str[strlen(str)];

					strncat(tokens[row], start, 1); //tokens에 &를 먼저 넣음
					start++; //&다음 문자를 가리킴

				//&이후에 null이 나오기 전이나 다음 특수문자가 나오기 전 문자들을 tokens의 한 행에 넣는 while문  
					while (start < end) {//start가 문자열의 끝에(null)에 도달하기 전까지 검사 
						if (*(start - 1) == ' ' && tokens[row][strlen(tokens[row]) - 1] != '&')
							// &jiwonshin 에서 start 가 &를 검사하고 j로 왔는데 &부분에 ' '가 있는 경우 
							//바로 전 과정에서 &를 tokens[row]에 넣었는데, &가 들어가지 않는 경우 
							return false; //false를 return 
						else if (*start != ' ') //&이후에 문자가 나온다면 
							strncat(tokens[row], start, 1); //tokens의 &뒤로 넣는다 
						start++;
					}
				}
				// 현재 검사하는 주소값 이전에 token 배열에 저장된 문자가 있고
				// &이전 문자에 특수문자가 없는경우 
				// ex) flag & end --> flag, &, end  따로 저장 
				else {
					strncpy(tokens[row], start, 1); //tokens에 &를 저장한다 
					start += 1;
				}
				
			}
		  	else if(*end == '*')
			{
				isPointer=0;//int isPointer 

				//답의 처음이 아닌 위치에서 '*'가 있는 경우 
				if(row > 0)
				{
					//'*'가 포인터로 쓰인경우
					for (i = 0; i < DATATYPE_SIZE; i++) { //DATATYPE_SIZE 35
						if (strstr(tokens[row - 1], datatype[i]) != NULL) { //그 전 토큰에 데이터타입이 존재한다면 
							strcat(tokens[row - 1], "*");//'*'를 데이터타입이 있는 그 전 토큰에  넣는다 
							start += 1;
							isPointer = 1;
							break;
						}
					}
					if (isPointer == 1) //'*데이터타입'을 tokens에 넣고 다음 while문으로 넘어간다 
						continue;

					//'*'가 데이터 타입 + 포인터 에 쓰이지 않은경우 
					//ex) 34 * abc
					if (*(start + 1) != 0) //'*'다음이 0이 아닌경우에
						end = start + 1; //end는 '*'다음 문자를 가리킨다

					//row가 1보다 크고 두번째 토큰 전의 값이 *이고 전 토큰이 모두 *일 때 
					if(row>1 && !strcmp(tokens[row - 2], "*") && (all_star(tokens[row - 1]) == 1)){
						strncat(tokens[row - 1], start, end - start); //token에 *을 대입 
						row--;
					}
					
					//'*' 가 곱하기로 쓰였을 경우 
					// 전 토큰이 소문자, 대문자 혹은 0~9일때
					// ex) 34 * 67 일 때  tokens[1] 은 '*'이다
					//tokens 에 *대입 
					else if(is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]) == 1){ 
						strncat(tokens[row], start, end - start);   
					}

					// * 바로 이전 토큰에 특수문자가 있다면 
					else if(strpbrk(tokens[row - 1], op) != NULL){		
						strncat(tokens[row] , start, end - start); 
							
					}
					else
						strncat(tokens[row], start, end - start);

					start += (end - start);
				}
				/* row == 0
				*가 답의 처음에 나왔을 때 ex) *a + *b 에서 tokens[0] 에 *a 가 대입
				*가 답의 처음 부분에서 두개이상 나왔을 때 ex) **a 에서 tokens[0] 에 **가 대입
				*/
			 	else if(row == 0) //*가 답의 처음에 나왔을 때 ex) *abc&&jiwon 
				{
					//end ==> '*' 이후에 나오는 특수문자의 주소
						//*이후 다른 특수문자가 있는지 검사
					//있으면 end가 가리킴
					if ((end = strpbrk(start + 1, op)) == NULL) { // '*'이후의 특수문자가 존재하지 않는다면 
						strncat(tokens[row], start, 1); // '*' 를 tokens에 넣는다  
						start += 1; //start '*'다음으로 옮김 
					}
					else { //'*'이후에 특수문자가 존재한다면 
						//'*'부터 특수문자가 나오기 전까지의 문자열을 tokens배열에 저장하는 while문  
						// **a 의 start 가 *(첫번째)를 가리킬 경우 end 는 *(두번째)를 가리켜 while문이 한번 돔 
						while (start < end) {
							// ex) **a
							if (*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
								return false;
							else if (*start != ' ') //공백이 아니라면 
								strncat(tokens[row], start, 1); //tokens배열에 넣는다 
								//tokens[0] => '*'
							start++;
						}
						if (all_star(tokens[row]))
							//	ex) **a --> tokens[0] 에 이미 첫번째 '*'가 있음, 이때 두번째 '*'ㅇ
							//연속해서 '*'이 나온 경우 하나의 토큰에 그 연속한 '*'을 저장하기 위해 row--를 해줌  
							row--;
						
					}
				}
			}
			else if(*end == '(') 
			{/* ex) tid = pthread_self()
			 pthread_self(), (, )
			 */
				 lcount = 0;//여는 괄호의 개수
				 rcount = 0;//닫는 괄호의 개수 

				 // 괄호 이전에 '&' 나 '*' 이 나온 경우 
				// ex) flag & ((jiwon || 24) && wow ))  
				if(row>0 && (strcmp(tokens[row - 1],"&") == 0 || strcmp(tokens[row - 1], "*") == 0)){
					
					//첫번째 '(' 지나고 연속해서 나오는 여는 괄호의 개수
					//ex) flag & ((jiwon || 24) && wow )) ==> lcount 1개  
					while(*(end + lcount + 1) == '(')
						lcount++;
					// start ==> 여는 괄호 중 가장 안쪽에 있는 여는괄호의 주소
					//			(jiwon || 24) && wow ))
					start += lcount;

					//닫는 괄호 중 가장 안쪽에있는 닫는 괄호
					//바로 위에서 계산한 start 를 닫을 수 있는 닫는 괄호 
					end = strpbrk(start + 1, ")");//end는 ')'의 주소를 가리킨다 
					
					//닫는괄호가 없다면 완벽한 괄호가 아니기 때문에 false  
					if(end == NULL)
						return false;
					else{//완벽한 괄호가 된다면
						while(*(end + rcount +1) == ')')
							rcount++;
						end += rcount;

						if(lcount != rcount)//완벽한 괄호가 되지 않는다면 틀림
							return false;
						//ex) b*(c+d) 
						// 현재 '(' 이전에 토큰이 두개이상 존재할 때,  '*' 나 '&' 이전에 나오는 문자가 '0'~'9' , 소문자, 대문자일 경우  
						// 혹은 '(' 이전에 토큰이 하나일 경우 
						if( (row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1])) || row == 1){ 
							strncat(tokens[row - 1], start + 1, end - start - rcount - 1);
							row--;
							start = end + 1;
						}
						else{//a&(b+c)
							strncat(tokens[row], start, 1);
							start += 1;
						}
					}
						
				}
				//row가 0인 경우 
				//row가 0보다 크고, 이전에 '&'나 '*'가 나오지 않은 경우 = 문자가 나온경우 ex) open(filename, 0_RDONLY)
				//우선순위 때문
				else{
					strncat(tokens[row], start, 1); //토큰에 '(' 혹은 ')'를 넣는다
					start += 1;
				}

			}
			//*end가 "(큰따옴표)를 가리킬 경우   
			else if(*end == '\"') 
			{
				//end는 다음 "(큰따옴표)를 가리킨다 
				//ex) "(*end)   "		-->		  "      "(*end) 
				end = strpbrk(start + 1, "\"");
				
				//"(큰따옴표)가 쌍을 이루지 않을 경우 false 이다 
				if(end == NULL)
					return false;
				//"(큰따옴표)가 쌍을 이룰 경우
				//"(큰따옴표) 안의 문자들을 tokens에 저장한다 
				else{
					strncat(tokens[row], start, end - start + 1);
					start = end + 1;//start를 "(큰따옴표) 다음으로 옮긴다 
				}

			}

			else{
				//이전에 ++이 오는 경우 , **== 안됨
				if(row > 0 && !strcmp(tokens[row - 1], "++"))
					return false;

				//이전에 --가 오는 경우, --==안됨
				if(row > 0 && !strcmp(tokens[row - 1], "--"))
					return false;
	
				//tokens에 start 로부터 한 문자를 받아 붙여넣는다 
				strncat(tokens[row], start, 1);
				start += 1;
				
			
				
				//현재 토큰이 -,+,--,++인 경우 
				if(!strcmp(tokens[row], "-") || !strcmp(tokens[row], "+") || !strcmp(tokens[row], "--") || !strcmp(tokens[row], "++")){

					if(row == 0) //이후에 나오는 문자를 함께 저장하기 위해 row--
						row--;

					//그전 토큰의 마지막 문자가 문자 혹은 숫자가 아닌경우
					else if(!is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){
						
						//++도 아니고, --도 아니라면 
						if(strstr(tokens[row - 1], "++") == NULL && strstr(tokens[row - 1], "--") == NULL)
							row--; //이후에 나오는 문자를 함께 저장하기 위해 row--
					}
				}
			}
		}
		//start!=end 
		//특수문자가 처음에 존재하지 않을 때 
		//문자로 시작할 때 
		else{//처음이랑 특수문자 있는 부분의 위치가 같지 않다면 

			//tokens에 '*'가 존재하고  
			if(all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1]))   
				row--;				

			//tokens[0]이 모두 '*'로 이루어져있다면 
			if(all_star(tokens[row - 1]) && row == 1)   
				row--;	//row--
			
			// 문자의 위치부터 첫 위치까지
			//특수문자 앞의 문자열동안 재귀
			//ex) jiwonshin(what) , start ==> jiwonshin(what) , end ==> (what)
			for(i = 0; i < end - start; i++){
				if(i > 0 && *(start + i) == '.'){//문자열의 처음을 제외하고 . 이 있는 경우--> 소수인 경우 
					strncat(tokens[row], start + i, 1);
					//.뒤에 공백이 있을 경우 i는 공백을 건너뛴다 
					while( *(start + i +1) == ' ' && i< end - start )//주소가 null
						i++; 
				}
				//특수문자가 나오기 전 공백의 index를 모두 넘어감
				//token배열에 저장할 때 이 공백은 저장하지 않기 위해 건너뛰는 while문 
				//ex) "a' '<' '3" 중 start 의 주소값이 ' '를 가리키고 있을 때 i를 1 증가하여 다음 문자를 가리키게 함, token엔 a만 저장됨 
				else if(start[i] == ' '){
					while(start[i] == ' ')
						i++;
					break;
				}
				else
					//상위 while문이 돌며 특수문자를 만나기 전까지 문자를 tokens 배열에 한 문자씩 저장
					//ex) abc(def)==> tokens[row]에 a,b,c, 가 while 문을 돌며 차례대로 저장됨 
					strncat(tokens[row], start + i, 1);
			}

			if(start[0] == ' '){ // 문자를 저장하기 위해 처음 부분의 공백을 건너뜀
				start += i;
				continue;
			}
			//다음 특수문자가 있는 주소로 가기위해 start 에서 다음 특수문자가 있는 거리만큼 더함 
			start += i;
		}
			
		strcpy(tokens[row], ltrim(rtrim(tokens[row])));//tokens 배열의 왼쪽 오른쪽 여백을 제거한 뒤 tokens에 다시 대입(덮어씀)
		//ex) jiwon && 24 --> tokens[0] 에는 jiwon만 들어감(&&전의 공백 제거)

		//row 가 1이상 && 현재 토큰의 마지막 문자가 소문자 대자 혹은 0~9 && 하나 이전 토큰에 gcc나 타입이 존재하거나
		//그 전 토큰의 마지막 문자가 소문자 대문자 혹은 0~9이거나
		//그전 토큰의 마지막 문자가 . 일 경우
		//ex) 3.2		

		 if(row > 0 && is_character(tokens[row][strlen(tokens[row]) - 1]) 
				&& (is_typeStatement(tokens[row - 1]) == 2 
					|| is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])
					|| tokens[row - 1][strlen(tokens[row - 1]) - 1] == '.' ) ){

			 //전 토큰에 datatype배열의 struct 나 unsigned가 있는 경우 
			//ex) ( strcuct int  ...  
			//위의 경우가 아니라면 false 리턴 
			if(row > 1 && strcmp(tokens[row - 2],"(") == 0)
			{
				if(strcmp(tokens[row - 1], "struct") != 0 && strcmp(tokens[row - 1],"unsigned") != 0)
					return false;
			}
			//현재 토큰의 마지막 문자가 소문자, 대문자, 혹은 0~9인 경우 
			//ex) extern int 
			//ex) unsigned int
			//위의 경우가 아니라면 false 리턴
			else if(row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				if(strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)	
					return false;
			}
			//extern, unsigned 이후에 바로 숫자, 문자가 나오는 경우
			else if(row > 1 && is_typeStatement(tokens[row - 1]) == 2){
				if(strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false;
			}
			
		}
		 // 첫 단어가 gcc라면 
		if((row == 0 && !strcmp(tokens[row], "gcc")) ){
			clear_tokens(tokens); //토큰 초기화 
			strcpy(tokens[0], str);	//token[0] 에 str 복사 
			return 1;
		} 

		row++;//다음 단어 혹은 특수문자를 저장하기 위해 row++한다 .  
		 //while문이 끝남  == 문자열을 모두 검사함 
	}

	//  *가 곱하기로 쓰이지 않고 포인터로 쓰인 경우 포인터의 '*'와 뒤 문자를 함께 저장한다 
	// ex) if(B&&*A) ==> tokens[row] 에 *A저장 
	if (all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1]))
		row--; //이전 row 의 '*'와 함께 저장하기 위해 row--를 해줌 				
	//'*'가 답의 맨 앞에서 포인터로 쓰인 경우에도 뒤 문자와 함께 저장한다 
	if (all_star(tokens[row - 1]) && row == 1)
		row--;

	//현재 참조하는 주소가 가리키는 문자열
	for (i = 0; i < strlen(start); i++)
	{
		//공백이 있을 경우 
		if (start[i] == ' ')
		{
			while (start[i] == ' ')
				i++;
			if (start[0] == ' ') {
				start += i; // 현재 참조하는 문자의 시작 주소가 공백의 주소라면 
				//start 를 공백 이후에 공백이 없어지는 첫 주소를 가리키게 한다   
				i = 0;
			}
			else
				row++;

			i--;
		}
		//공백이 없을 경우 
		else
		{
			//토큰에 저장 
			strncat(tokens[row], start + i, 1);
			if (start[i] == '.' && i < strlen(start)) { // start[i]가 '.'이고 i가 전체 길이 보다 작은 경우에
				while (start[i + 1] == ' ' && i < strlen(start)) //공백을 없앤다 
					i++;

			}
		}
		//token의 왼쪽, 오른쪽 공백을 없앤다
		strcpy(tokens[row], ltrim(rtrim(tokens[row])));
		//thread로 돌리는 경우라면
		if (!strcmp(tokens[row], "lpthread") && row > 0 && !strcmp(tokens[row - 1], "-")) {
			strcat(tokens[row - 1], tokens[row]); //token뒤에 붙이고 
			memset(tokens[row], 0, sizeof(tokens[row]));//초기화 
			row--;
		}
		//thread로 돌리는 경우가 아니라면
		else if (row > 0 && is_character(tokens[row][strlen(tokens[row]) - 1])
			&& (is_typeStatement(tokens[row - 1]) == 2
				|| is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])
				|| tokens[row - 1][strlen(tokens[row - 1]) - 1] == '.')) {

			//'('가 처음 나오는 경우 
			if (row > 1 && strcmp(tokens[row - 2], "(") == 0)
			{
				// '(' 이전에 struct 혹은 unsigned 가 나오는 경우  
					//ex) struct ( 
				if (strcmp(tokens[row - 1], "struct") != 0 && strcmp(tokens[row - 1], "unsigned") != 0)
					return false; //false 리턴
			}
			//참조하는 토큰의 하나 전 토큰이 unsigned ,extern인 경우 
				//ex) extern 문자 (x) extern 데이터타입 문자(o) 
			else if (row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				if (strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)
					return false;
			}
			//참조하는 token이 데이터타입인데 두 토큰 전 값이 unsigned ,extern인 경우
			//ex) unsigned int int (x) 
			else if (row > 1 && is_typeStatement(tokens[row - 1]) == 2) {
				if (strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false;//return false 
			}
		}
	}


	//row가 양수일 때
	if (row > 0)
	{
		//이전 토큰이 #include 이거나 include,struct 인 경우 
		if (strcmp(tokens[0], "#include") == 0 || strcmp(tokens[0], "include") == 0 || strcmp(tokens[0], "struct") == 0) {
			clear_tokens(tokens);//토큰을 모두 초기화 하고 
			strcpy(tokens[0], remove_extraspace(str));//공백을 지운다 
		}
	}

	// 첫번째 토큰에 "gcc" 혹은 type이 있거나 extern이 있다면 
	if (is_typeStatement(tokens[0]) == 2 || strstr(tokens[0], "extern") != NULL) {
		for (i = 1; i < TOKEN_CNT; i++) {
			//다음 토큰이 비었다면 break
			if (strcmp(tokens[i], "") == 0)
				break;
			//i를 토큰의 최대 개수 전이라면 
			if (i != TOKEN_CNT - 1)
				strcat(tokens[0], " "); //첫번째 토큰에 공백을 넣고 
			strcat(tokens[0], tokens[i]); // "gcc" 혹은 type, extern 뒤에 나온 문자열을 함께 저장 
			memset(tokens[i], 0, sizeof(tokens[i])); //초기화 
		}
	}

	//tokens의 타입이 존재할 동안 
	while ((p_str = find_typeSpecifier(tokens)) != -1) {
		if (!reset_tokens(p_str, tokens)) //완벽한 형태가 아니라면 reset 
			return false;
	}

	//datatype 배열의 struct 이후에 나오는 토큰이 존재할 경우 
	while ((p_str = find_typeSpecifier2(tokens)) != -1) {
		if (!reset_tokens(p_str, tokens))//완벽한 형태가 아니라면 reset 
			return false;
	}

	return true;
}

node *make_tree(node *root, char(*tokens)[MINLEN], int *idx, int parentheses)
{
	node *cur = root;
	node *new;
	node *saved_operator;
	node *operator;
	int fstart;
	int i;

	while (1)
	{
		//더이상 참조할 토큰이 존재하지 않을 때 while문을 탈출 
		if (strcmp(tokens[*idx], "") == 0)
			break;

		//닫는 괄호 이후에 참조할 토큰이 존재 할 경우 닫는 괄호 이전의 토큰을 리턴 
		//tokens[*idx]가 닫는괄호')'라면 cur가 가리키는 노드를 리턴
		//ex)  "(abc)" 의 ')'를 만나기 바로 전 while문에서 cur 가 abc를 가리키고 있음
		if (!strcmp(tokens[*idx], ")"))
			return get_root(cur);

		//tokens[*idx] 가 ','를 만나면 cur의 최상위 노드를 리턴 
		//ex) sigprocmask(SIG_UNBLOCK , 의 ,를 만났다면, SIG_UNBLOCK를 리턴
		else if (!strcmp(tokens[*idx], ","))
			return get_root(cur);

		//tokens[*idx] 가 여는 괄호라면 
		else if (!strcmp(tokens[*idx], "("))
		{
			//괄호 이전 토큰이 is_operator 가 아니고 
			//',' 가 아니라면 
			//ex) abc(de) 주소값이 (에 왔을 때 
			if (*idx > 0 && !is_operator(tokens[*idx - 1]) && strcmp(tokens[*idx - 1], ",") != 0) {
				//'('로 시작함
				fstart = true;

				while (1)
				{
					//다음 토큰 참조 ( 여는괄호 이후 토큰 참조 )
					*idx += 1;

					//닫는 괄호가 왔으면 괄호안의 문자를 모두 검사했기 때문에 while문 빠져나감 
					if (!strcmp(tokens[*idx], ")"))
						break;

					new = make_tree(NULL, tokens, idx, parentheses + 1);

					if (new != NULL) {
						//'(' 이전에 나오는 노드가 parent, 
						// 직후 나오는 노드 한개가 child_head 
						//ex) sigprocmask(SIG_UNBLCOK, &SIG_SET,NULL) 
						//parent(sigprocmask) <----> childhead(SIG_UNBLOCK)
						if (fstart == true) {
							cur->child_head = new;
							new->parent = cur;

							fstart = false;
						}
						else {
							//childhead 이후로 나오는 노드를 서로 연결 
							cur->next = new;
							new->prev = cur;
						}
						//다음 노드를 현재 노드에 이어주기 위해 
						//이전 make_tree에서 리턴받은 new 노드에 다음에 나오는 노드를 이어주기 위해 
						cur = new;
					}
					//while을 탈출
					//괄호 안의 문자들의 토큰을 모두 검사했을 때 
					if (!strcmp(tokens[*idx], ")"))
						break;
				}
			}
			//*idx가 0이라면
			else {
				*idx += 1;//*idx를 1 증가한다 

				//자기 자신에 재귀 
				//ex) (abc) 에서 abc 를 이름으로 하는 노드를 return get_root 로 return 한거  
				new = make_tree(NULL, tokens, idx, parentheses + 1);

				//cur 가 비었다면 null을 가리킴
				if (cur == NULL)
					cur = new;

				//new와 cur 가 같을 때 
				else if (!strcmp(new->name, cur->name)) {
					//new 가 아래의 조건일때 
					if (!strcmp(new->name, "|") || !strcmp(new->name, "||")
						|| !strcmp(new->name, "&") || !strcmp(new->name, "&&"))
					{
						//new 를 연산자로 하는 문자를 가리키는 노드를 cur 가 가리킨다
						cur = get_last_child(cur);

						//new 의 자식노드가 있다면 
						if (new->child_head != NULL) {
							new = new->child_head;//자식노드로 이동 

							//자식, 부모 관계를 끊고
							new->parent->child_head = NULL;
							new->parent = NULL;
							//위의 연산자의 대상(연산자를 사용해서 계산을 하는 노드) next prev 관계를 유지한다 
							new->prev = cur;
							cur->next = new;
						}
					}
					//새로운 노드가 + 나 *일때
					else if (!strcmp(new->name, "+") || !strcmp(new->name, "*"))
					{
						i = 0;

						while (1)
						{
							//연산자 이후의 토큰이 비어있다면
							//연산의 대상이 없으므로 break 
							if (!strcmp(tokens[*idx + i], ""))
								break;
							
							//다음 토큰이 연산자이고 )가 아니라면 break
							//다음 나오는 연산자를 찾는다
							if (is_operator(tokens[*idx + i]) && strcmp(tokens[*idx + i], ")") != 0)
								break;

							i++;
						}
						//연산자의 우선순위를 계산해서
						//연산자 우선순위가 tokens[*idx + i]이 *, +보다 높다면
						if (get_precedence(tokens[*idx + i]) < get_precedence(new->name))
						{
							//현재노드는 마지막 child 를 가리키고
							cur = get_last_child(cur);
							//현재노드와 새로운 노드(*, +)를 연결한다
							cur->next = new;
							new->prev = cur;
							//cur 는 새로운 노드를 가리킨다 
							cur = new;
						}
						//연산자 우선순위가 +나 *가 높다면 
						else
						{
							//cur 는 마지막 child_head && 마지막 next 
							cur = get_last_child(cur);
							//*,+의 자식노드가 있다면
							if (new->child_head != NULL) {
								//자식 노드로 간 뒤 
								new = new->child_head;
								//부모노드(*,+)와의 관계를 끊고
								new->parent->child_head = NULL;
								new->parent = NULL;
								//cur 와 잇는다
								new->prev = cur;
								cur->next = new;
							}
						}
					}
					//새로운 노드가 + 나 *가 아닐 때 
					else {
						//cur 는 마지막 child_head && 마지막 next 
						cur = get_last_child(cur);
						//그 노드가 새로운 노드와 연결함 
						cur->next = new;
						new->prev = cur;
						cur = new;
					}
				}
				//new->name 과 cur->name 이 다를 때 
				else
				{
					//cur 는 마지막 child_head && 마지막 next 
					cur = get_last_child(cur);
					//cur 와 new 를 잇는다
					cur->next = new;
					new->prev = cur;

					cur = new;
				}
			}
		}
		//operator 라면 
		else if (is_operator(tokens[*idx]))
		{
			if (!strcmp(tokens[*idx], "||") || !strcmp(tokens[*idx], "&&")
				|| !strcmp(tokens[*idx], "|") || !strcmp(tokens[*idx], "&")
				|| !strcmp(tokens[*idx], "+") || !strcmp(tokens[*idx], "*"))
			{
				if (is_operator(cur->name) == true && !strcmp(cur->name, tokens[*idx]))
					operator = cur;

				else
				{
					new = create_node(tokens[*idx], parentheses);
					//.최상위인지, 아니면 operator 앞의 문자인지 하면서 더 확인해볼것 
					//operator 가 최상위 노드를 가리키게 함 
					//ex) flag & O_APPEND 의 &를 참조하고 있을 경우 operator 는 flag의 주소값 
					operator = get_most_high_precedence_node(cur, new);

					//operator 가 최상위 노드라면 
					if (operator->parent == NULL && operator->prev == NULL) {

						//operator 의 우선순위가 new의 우선순위 보다 높을 때 
						//1) operators 가 문자열로 이루어져있다면 -1을 리턴하기 때문에 두번째 인자값이 더 커짐
						//ex) ex) flag & O_APPEND 의 &를 참조하고 있을 경우
						//2) 우선순위가 다른 operators 가 왔을 경우 우선순위를 결정해줌 						
						//ex) glo_val < 3 || gloval > 6 에서,  < 가 || 보다  우선순위가 높음 
						if (get_precedence(operator->name) < get_precedence(new->name)) { //precedence 값이 클수록 우선순위가 낮음 
							//new노드와 operator노드의 위치를 바꾸고 new 를 리턴
							//ex) 전 : operator --> flags , new --> & 
							//	  후 : new -->flag, operator --> & 
							cur = insert_node(operator, new);
						}

						//연산자의 우선순위보다 new노드의 우선순위가 더 높을 때 
						else if (get_precedence(operator->name) > get_precedence(new->name))
						{
							//연산자의 자식노드, 즉 연산의 대상이 되는 노드가 있다면
							if (operator->child_head != NULL) {
								//마지막 child_head && 마지막 next를 가리키고 
								operator = get_last_child(operator);
								cur = insert_node(operator, new);//새로운 노드를 삽입한 뒤 cur 가 가리킨다  
							}
						}
						//우선순위가 같다면
						else
						{
							//현재노드가 연산자인지 판단하기 위해 
							operator = cur;

							while (1)
							{
								//연산자이고 검사하는 토큰과 같다면 
									//연산자의 중복이기 때문에 break
								if (is_operator(operator->name) == true && !strcmp(operator->name, tokens[*idx]))
									break; 
								
								//연산자 전 노드가 있다면
								if (operator->prev != NULL)
									//operator 는 이전 노드를 가리킴
									operator = operator->prev;
								//연산자 전 노드가 없다면
								else//연산자 홀로 계산 불가능 하기 때문에 break 
									break;
							}
							//operator 의 이름이 현재 검사하는 토큰과 다르다면 
							if (strcmp(operator->name, tokens[*idx]) != 0)
								//부모 노드로 이동 
								operator = operator->parent;
							//operator 가 null이 아니라면
							if (operator != NULL) {
								//operator 의 이름이 현재 검사하는 토큰과 같다면
								if (!strcmp(operator->name, tokens[*idx]))
									//cur 는 operator 를 가리킴 
									cur = operator;
							}
						}
					}

					else//최상위 노드가 아닐 때
						cur = insert_node(operator, new); //새로운 노드를 삽입 
				}

			}
			//대소비교 일 때 
			else
			{
				//대소비교의 노드를 생성
				new = create_node(tokens[*idx], parentheses);

				//이전 토큰을 노드로 만든 적이 없을 때 
				if (cur == NULL)
					cur = new;

				//이전의 토큰의 노드가 있을 때
				else
				{
					// operator = 현재 생성된 연산자 노드(new) 보다 상위에 있는 연산자 
					operator = get_most_high_precedence_node(cur, new);

					// operator 의 괄호가 new 의 괄호보다 큰 경우 안쪽에 있는 경우 
					if (operator->parentheses > new->parentheses)
						cur = insert_node(operator, new);

					//operate 가 최상위 노드일 때 , operator 이전에 토큰이 하나 존재할 때
					//operator 가 트리의 가장 상위에 있을 때 
					//ex) glo_val < 3 || abc > 6 일 때 ||
					else if (operator->parent == NULL && operator->prev == NULL) {

						//1. new->name이 문자열 operator->name이 operator 일 때
						//2. new 의 연산자 선순위가 operator 보다 높을 때 
						if (get_precedence(operator->name) > get_precedence(new->name))
						{
							//operator 의 child_head 가 존재할 경우 
							//ex) glo_val < 3 || abc > 6 의 경우 operator(||)의 child_head(<)
							if (operator->child_head != NULL) {
								//child_node를 리턴한다 

								//ex) glo_val < 3 || abc > 6  의 경우 new 가 abc 노드를 가리킬 때, operator 가 < 를 가리키고 있음 
								operator = get_last_child(operator);

								//기존의  와 new->prev와의 연결관계애서 new 노드 자리에 operator 노드를 대체하고 operator 노드와
								//new노드사이에 parent 와 child_head 관계를 만듬
								cur = insert_node(operator, new);
							}
						}

						//operator 의 연산자 우선순위가 new 의 연산자 우선순위 보다 높을 경우 
						else
							//두 노드를 연결하고
							//부모와 자식 관계를 바꾼다 
							//new :parent , operatpor : childhead 
							cur = insert_node(operator, new);
					}

					else
						cur = insert_node(operator, new);
				}
			}
		}
		//문자일 때 
		else
		{
			//ex) (abc)의 abc 참조중 
			new = create_node(tokens[*idx], parentheses);

			//cur 노드의 초기화
			//cur 노드가 null이라면 새로 만든 노드를 가리킴 
			if (cur == NULL)
				cur = new;

			//child 가 없을 때 
			else if (cur->child_head == NULL) {
				cur->child_head = new; //new 를 가리킴 
				new->parent = cur;//new 의 parent 는 cur를 가리킴 

				cur = new;
			}
			//child 가 있을 때 
			else {
				//cur 의 next와 child_head가 빌때까지 next 와 child_head로 옮긴 cur  
				//방금 만든 문자열 노드를 지금까지 만는 노드들과 잇는다  
				cur = get_last_child(cur);

				cur->next = new;
				new->prev = cur;

				cur = new;
			}
		}

		*idx += 1;
	}
	//현재 참조하고 있는 cur 노드 ==> 마지막으로 참조한 노드의 최상위 노드를 리턴함 
	return get_root(cur);
}

/*prev, next 로 이어져있는 노드들의 자리를 서로 바꾸는 함수 */
node *change_sibling(node *parent)
{
	node *tmp;

	//tmp는 부모의 자식노드를 가리킴
	tmp = parent->child_head;

	//부모의 자식노드 부모의 자식노드의 다음노드를 연결 
	parent->child_head = parent->child_head->next;
	//부모의 자식노드의 부모 = 인자값으로 드러온 노드 
	parent->child_head->parent = parent;
	parent->child_head->prev = NULL;

	//인자값으로 들어온 노드를의 자식노드와 기존의 부모의 자식노드를 연결
	parent->child_head->next = tmp;
	//prev 노드를 인자값으로 들어온 노드를의 자식드와 연결
	parent->child_head->next->prev = parent->child_head;
	//다음 노드를 null로 초기화 
	parent->child_head->next->next = NULL;
	parent->child_head->next->parent = NULL;

	return parent;
}

/*새로운 노드를 생성하고 초기화 시키는 함수*/
node *create_node(char *name, int parentheses)
{
	//새로운 노드 생성 후 동적할당 
	node *new;
	new = (node *)malloc(sizeof(node));

	//노드의 name 변수에 tokens[*idx] 저장
	new->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(new->name, name);
	//.
	// new 의 node 구조체의 변수를 초기화 
	new->parentheses = parentheses;
	new->parent = NULL;
	new->child_head = NULL;
	new->prev = NULL;
	new->next = NULL;

	//초기화 한 노드 리턴 
	return new;
}

/*char *op의 값에 operators 배열의 값이 있는지 확인하는 함수( ‘(‘, ‘)’ 제외)*/
int get_precedence(char *op)
{
	int i;
	//'('와 ')' 제외 
	for (i = 2; i < OPERATOR_CNT; i++) {
		//operators[i] 랑 op랑 같다면 
		if (!strcmp(operators[i].operator, op))
			//int precedence 값을 return 
			return operators[i].precedence;
	}
	//operators 가 아닐 경우 false 리턴
	return false;
}

//else if(is_operator(tokens[*idx]))
/*현재 토큰이 오퍼레이터인지 확인하는 함수 */
int is_operator(char *op)
{
	int i;

	for (i = 0; i < OPERATOR_CNT; i++)
	{
		//char *op 가 operator 인지 확인
		if (operators[i].operator == NULL)
			break; //없다면 false
		if (!strcmp(operators[i].operator, op)) {
			return true; //있다면 true 
		}
	}
	//없다면 false
	return false;
}

/*노드를 조건에 따라 프린트 하는 함수 */
void print(node *cur)
{
	if (cur->child_head != NULL) { //자식노드가 있다면 
		print(cur->child_head);//자식노드를 프린트하고 
		printf("\n");//개행을 프린트 하고
	}
	//next 노드가 있다면 
	if (cur->next != NULL) {
		print(cur->next); //next 노드를 프린트 하고
		printf("\t");// tab 만큼 출력
	}
	printf("%s", cur->name); //현재 노드의 이름을 출력
}
/*cur 가 prev노드로 움직여 연산자인 parent 노드를 가리키게 하는 함수*/
node *get_operator(node *cur)
{
	//cur 가 null이라면 
	if (cur == NULL)
		return cur;

	//cur 가 null이 아니고 , cur->prev가 존재한다면
	if (cur->prev != NULL)
		//cur->prev가 존재할때까지
		while (cur->prev != NULL)
			//cur 를 cur->prev로 옮긴다
			cur = cur->prev;

	//가장 prev한 노드의 parent (operator 노드) 를 리턴
	return cur->parent;
}
/*최상위 노드를 알아내는 함수*/
node *get_root(node *cur)
{
	if (cur == NULL)
		return cur;
	//현재 노드의 전 노드가 존재할 때 까지 
	//전 노드로 간다 
	while (cur->prev != NULL)
		cur = cur->prev;
	//현재 노드의 부모가 존재한다면 
	if (cur->parent != NULL)
		//그 부모의 부모가 존재하지 않을 때 까지 *get_root()함수를 재귀한다  
		cur = get_root(cur->parent);

	//최상위 노드를 리턴한다 
	return cur;
}

//오퍼레이터를 찾기까지 cur = cur->prev 와 cur->parent 를 하는 함수
node *get_high_precedence_node(node *cur, node *new)
{

	//현재 참조하는 노드가 operator 라면
	if (is_operator(cur->name))
		if (get_precedence(cur->name) < get_precedence(new->name)) //현재 노드의 연산자 우선순위가 new 보다 높을 경우에 
			return cur;

	//현재 참조하는 노드가 operator 가 아니라면 
	if (cur->prev != NULL) {
		while (cur->prev != NULL) {
			cur = cur->prev; //prev노드가 빌때까지 prev노드로 간다 

			//한단계 이전 노드에 대한 검사를 다시함
			return get_high_precedence_node(cur, new);
		}

		//부모노드가 존재한다면 
		if (cur->parent != NULL)
			//
			return get_high_precedence_node(cur->parent, new);
	}

	if (cur->parent == NULL) //cur가 최상위 노드라면 
		return cur;
}
/*   *saved_operator 가 최상위 노드를 가리키게 하는 함수*/
node *get_most_high_precedence_node(node *cur, node *new)
{
	node *operator = get_high_precedence_node(cur, new);
	node *saved_operator = operator;

	while (1)
	{
		//최상위 노드라면 break
		if (saved_operator->parent == NULL)
			break;

		//이전 노드가 있다면
		if (saved_operator->prev != NULL)
			operator = get_high_precedence_node(saved_operator->prev, new);

		//부모노드가 있다면
		else if (saved_operator->parent != NULL)
			operator = get_high_precedence_node(saved_operator->parent, new);

		//변경된 operator 를 가리킴 
		saved_operator = operator;
	}

	//변경된 operator 를 리턴
	return saved_operator;
}


//부모 노드 잘보기 
//new 를 추가하여 연결관계를 다시함 
	//old 와 old->prev 의 관계를 끊고 old의 prev노드와 new 노드가 서로 각각 prev next 관계를 갖음 
	//new 노드와 old노드는 서로 각각 parent 와 child_head 가 된다 
//old -> prev가 존재하지 않으면 new 와 old의 위치만 바꾼다//
node *insert_node(node *old, node *new)
{

	if (old->prev != NULL) {
		new->prev = old->prev;
		old->prev->next = new;
		old->prev = NULL;
	}

	new->child_head = old;
	old->parent = new;

	return new;
}

// 현재 노드에서 last child를 cur 로 리턴하는 함수 
// last child ==> 마지막 child_head && 마지막 next 
node *get_last_child(node *cur)
{
	// 현재의 childhead 노드가 존재한다면 
	if (cur->child_head != NULL)
		//cur는 cur->childhead 의 주소값으로 바꾼다
		cur = cur->child_head;

	//cur->next 가 null일 때까지  
	while (cur->next != NULL)
		//cur를 cur->next로 옮긴다 
		cur = cur->next;

	//cur를 return한다 
	return cur;
}

//형제노드의 개수를 세는 함수 (prev, next로 연결돼있는 노드의 수를 세는 함수) 
int get_sibling_cnt(node *cur)
{
	int i = 0;

	//prev가 존재할 때 까지 cur = cur->prev 
	while (cur->prev != NULL)
		cur = cur->prev;

	//next가 존재할 때 까지 cur = cur->next 
	while (cur->next != NULL) {
		cur = cur->next;

		//노드의 수를 셈
		i++;
	}

	//개수를 리턴 
	return i;
}
/*노드를 free 하는 함수 */
void free_node(node *cur)
{
	//현재 노드의 child_head가 있다면 
	if (cur->child_head != NULL)
		//child_head로 재귀 
		free_node(cur->child_head);

	//cur->next가 있다면 
	if (cur->next != NULL)
		free_node(cur->next);//cur->next로 재귀 

	//현재 노드가 있다면
	if (cur != NULL) {
		//노드가 가리키는 모든것을 NULL
		cur->prev = NULL;
		cur->next = NULL;
		cur->parent = NULL;
		cur->child_head = NULL;
		free(cur);//free 한다 
	}
}

/*숫자 , 소문자, 대문자가 손재하는지 여부를 판단*/
int is_character(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}


/*문자열에 "gcc" 나 데이터타입이 있는지 확인하는 함수*/
int is_typeStatement(char *str)
{ /*char *strstr(char * const _String, char const * const _SubString);
	const_String : 검사할 문자열
	const_Substring : 찾는 문자열
	문자열을 찾았으면 문자열로 시작하는 문자열의 포인터를 반환, 문자열이 없으면 NULL을 반환
		ef) 출력시 포인터가 가리키는 곳 부터 null까지 모두 출력
	*/

	char *start;
	char str2[BUFLEN] = { 0 };
	char tmp[BUFLEN] = { 0 };
	char tmp2[BUFLEN] = { 0 };
	int i;

	start = str;
	strncpy(str2, str, strlen(str));
	remove_space(str2);

	while (start[0] == ' ') //start의 앞에 공백이 있다면 지운다 
		start += 1;
	if (strstr(str2, "gcc") != NULL) //st2에 gcc 문자열이 있다면 
	{
		strncpy(tmp2, start, strlen("gcc")); //tmp2에 start의 "gcc"를 복사 

		//tmp2에 "gcc"문자열 복사가 온전하게 안됐을 때 return 0
		//잘 됐다면 return 2
		if (strcmp(tmp2, "gcc") != 0)
			return 0;
		else
			return 2;
	}

	//gcc 가 없을 때 
	//문자열에 datatype 이 존재하는지 판단하는 재귀문 
	for (i = 0; i < DATATYPE_SIZE; i++)
	{
		//str2에 datatype이 존재한다면 
		if (strstr(str2, datatype[i]) != NULL)
		{
			//tmp 에 str2에서 datatype[i]의 길이만큼 복사 
			strncpy(tmp, str2, strlen(datatype[i]));
			//tmp2 에 start에서 datatype[i]의 길이만큼 복사 
			strncpy(tmp2, start, strlen(datatype[i]));
			//tmp에 datatype이 잘 복사 됐을 때 
			if (strcmp(tmp, datatype[i]) == 0)
				if (strcmp(tmp, tmp2) != 0) //tmp 와 tmp2가 다르다면 
					return 0;
				else
					return 2;
		}

	}
	return 1;

}
/*특정 토큰이 있는 토큰 인덱스를 찾는 함수 */
int find_typeSpecifier(char tokens[TOKEN_CNT][MINLEN])
{
	int i, j;

	//토큰의 개수만큼 검사 
	for (i = 0; i < TOKEN_CNT; i++)
	{
		//타입의 전체 개수만큼 검사
		for (j = 0; j < DATATYPE_SIZE; j++)
		{
			//토큰에 데이터 타입이 있고, 첫번째토큰이 아니라면 
			if (strstr(tokens[i], datatype[j]) != NULL && i > 0)
			{
				//첫번째 토큰이 여는 괄호, 다음 다음 토큰이 닫는 괄호이고 그 이후 연산 기호 혹은 괄호가 나온다면  
					//ex) (x)&의 경우  
				if (!strcmp(tokens[i - 1], "(") && !strcmp(tokens[i + 1], ")")
					&& (tokens[i + 2][0] == '&' || tokens[i + 2][0] == '*'
						|| tokens[i + 2][0] == ')' || tokens[i + 2][0] == '('
						|| tokens[i + 2][0] == '-' || tokens[i + 2][0] == '+'
						|| is_character(tokens[i + 2][0])))
					return i; //토큰의 번호를 return 
			}
		}
	}
	return -1; //없다면 -1 리턴
}
/*struct 이후에 나오는 토큰을 검사*/
int find_typeSpecifier2(char tokens[TOKEN_CNT][MINLEN])
{
	int i, j;

	//토큰의 개수만큼 검사 
	for (i = 0; i < TOKEN_CNT; i++)
	{		
		//타입의 전체 개수만큼 검사
		for (j = 0; j < DATATYPE_SIZE; j++)
		{
			//struct 이후 문자, 혹은 숫자가 나온다면 
			if (!strcmp(tokens[i], "struct") && (i + 1) <= TOKEN_CNT && is_character(tokens[i + 1][strlen(tokens[i + 1]) - 1]))
				return i;
		}
	}
	return -1;//없다면 -1 리턴
}
/*tokens배열의 한 행 전체가 '*'인지 판단하는 함수 */
int all_star(char *str)
{
	int i;
	int length = strlen(str); //tokens배열의 길이

	if (length == 0) //배열의 길이가 이라면 0리턴
		return 0;

	for (i = 0; i < length; i++)
		if (str[i] != '*') //'*'가 없다면 
			return 0;//return 0
	return 1;//'*'가 존재한다면 return 1

}
/*모두 character 인지 검사하는 함수*/
int all_character(char *str)
{
	int i;
	//문자열의 길이만큼 검사한다
	for (i = 0; i < strlen(str); i++)
		if (is_character(str[i])) //함수호출의 결과에 따른 return 
			return 1;
	return 0;

}

int reset_tokens(int start, char tokens[TOKEN_CNT][MINLEN])
{
	int i;
	int j = start - 1;
	int lcount = 0, rcount = 0;
	int sub_lcount = 0, sub_rcount = 0;

	if (start > -1) {
		//tokens[0] 이 struct 일 때 
		if (!strcmp(tokens[start], "struct")) {
			strcat(tokens[start], " "); //공백을 붙이고 
			strcat(tokens[start], tokens[start + 1]);//다음 토큰도 붙인다 

			//토큰을 검사하며 하나 뒤의 토큰을 현재 토큰에 복사한다 
			for (i = start + 1; i < TOKEN_CNT - 1; i++) {
				strcpy(tokens[i], tokens[i + 1]);
				memset(tokens[i + 1], 0, sizeof(tokens[0])); //뒤의 토큰은 0으로 초기화 한다 
			}
		}

		//unsigned 이고 그 이후 토큰이 닫는 괄호일때 
		else if (!strcmp(tokens[start], "unsigned") && strcmp(tokens[start + 1], ")") != 0) {
			strcat(tokens[start], " "); //공백을 붙이고 
			strcat(tokens[start], tokens[start + 1]);//다음 토큰도 붙인다
			strcat(tokens[start], tokens[start + 2]);//그 다음 토큰도 붙인다

			//토큰을 검사하며 하나 뒤의 토큰을 현재 토큰에 복사한다 
			for (i = start + 1; i < TOKEN_CNT - 1; i++) {
				strcpy(tokens[i], tokens[i + 1]);
				memset(tokens[i + 1], 0, sizeof(tokens[0]));//뒤의 토큰은 0으로 초기화 한다 
			}
		}

		//j는 start 다음을 참조할때
		j = start + 1;
		//닫는 괄호가 아닐때까지 도는 while 문 
		while (!strcmp(tokens[j], ")")) {
			rcount++;//기준으로부터 닫는괄호가 나오기 전까지의 index 차 
			if (j == TOKEN_CNT) //j가 토큰의 최대 수와 같다면 
				break;//탈출 
			j++;
		}
		

		//j는 start 이전을 참조할때
		j = start - 1;
		while (!strcmp(tokens[j], "(")) { //여는괄호가 나오기 전까지 
			lcount++; //기준으로부터 여는괄호가 나오기 전까지의 index 차 
			if (j == 0) //기준으로부터 닫는 괄호가 나오기 전까지가 더 작다면  
				break;//탈출 
			j--;//start-1 이전에 여는 괄호가 나오기 전까지의 수를 --
		}

		//기준으로부터 닫는 괄호와 여는괄호 가 나오기 전까지의 차이가 다르고, j의 마지막 문자가 숫자나 알파벳 이거나 
		//기준으로부터 닫는 괄호와 여는괄호 가 나오기 전까지의 차이가 같을 때  
		if ((j != 0 && is_character(tokens[j][strlen(tokens[j]) - 1])) || j == 0)
			lcount = rcount;

		//위의 경우가 아니라면 false 리턴 
		if (lcount != rcount)
			return false;

		//start 와 여는 괄호까지의 거리가 있고, tokens[start - lcount - 1] 가 sizeof가 아니라면 
		//ex) sizeof(ㅁ, b)가 안되기 때문 
		if ((start - lcount) > 0 && !strcmp(tokens[start - lcount - 1], "sizeof")) {
			return true;
		}
		//토큰이 unsigned이거나,   (   ~~ struct) 형태라면
		else if ((!strcmp(tokens[start], "unsigned") || !strcmp(tokens[start], "struct")) && strcmp(tokens[start + 1], ")")) {
			strcat(tokens[start - lcount], tokens[start]); //차례대로 붙인다
			strcat(tokens[start - lcount], tokens[start + 1]);
			strcpy(tokens[start - lcount + 1], tokens[start + rcount]);

			//괄호의 시작부터 토큰의 개수까지 도는 for문
			for (int i = start - lcount + 1; i < TOKEN_CNT - lcount - rcount; i++) {
				strcpy(tokens[i], tokens[i + lcount + rcount]); //i번째에 )까지의 거리의 두배+i 번째 토큰을 복사한다 
				memset(tokens[i + lcount + rcount], 0, sizeof(tokens[0]));//0으로 초기화 한다 
			}


		}
		//그 외의 경우라면 
		else {
			//기준 토큰으로 부터 두 토큰 이후에 (로 시작하는 토큰이 있다면
			if (tokens[start + 2][0] == '(') {
				//j를 2 옮긴뒤 
				j = start + 2;
				//token[j]가 (를 찾을 때 까지 j값을 옮긴다 
				while (!strcmp(tokens[j], "(")) {
					sub_lcount++; //여는 괄호까지 거리
					j++;
				}
				//token[j+1]이 )을 찾았다면 i를 그 찾은 인덱스 값으로 바꾼다 				
				if (!strcmp(tokens[j + 1], ")")) {
					j = j + 1; 
					//tokens[j] 가 ) 일때까지 변수를 증가시킨다  
					while (!strcmp(tokens[j], ")")) {
						sub_rcount++; //닫는 괄호까지 거리
						j++;
					}
				}
				else
					//token[j+1]이 )을 찾지 못했다면 false 리턴 
					return false;
				
				//여는 괄호와 닫는 괄호가 조화를 이루지 않을 때 (완벽한 괄호 x)
				if (sub_lcount != sub_rcount)
					return false;//false 리턴 

				strcpy(tokens[start + 2], tokens[start + 2 + sub_lcount]); //여는 괄호까지의 거리에 있는 토큰을 복사한다 
				for (int i = start + 3; i < TOKEN_CNT; i++) //그 이후 토큰을 0으로 초기화 시킨다 
					memset(tokens[i], 0, sizeof(tokens[0]));

			}
			strcat(tokens[start - lcount], tokens[start]); //tokens[start - lcount]뒤에 tokens[start]를 붙여넣는다 
			strcat(tokens[start - lcount], tokens[start + 1]);//tokens[start - lcount]뒤에 tokens[start+1]를 붙여넣는다 
			strcat(tokens[start - lcount], tokens[start + rcount + 1]);//tokens[start - lcount]뒤에 tokens[start+ rcount+1]를 붙여넣는다 

			for (int i = start - lcount + 1; i < TOKEN_CNT - lcount - rcount - 1; i++) { 
				strcpy(tokens[i], tokens[i + lcount + rcount + 1]);// tokens[i + lcount + rcount + 1]를 tokens[i]에 복사한다 
				memset(tokens[i + lcount + rcount + 1], 0, sizeof(tokens[0]));//tokens[i + lcount + rcount + 1]를 0으로 초기화 시킨다

			}
		}
	}
	return true;
}
/*tokens을 초기화 하는 함수*/
void clear_tokens(char tokens[TOKEN_CNT][MINLEN])
{
	int i;

	for (i = 0; i < TOKEN_CNT; i++) //토큰의 개수만큼 
		memset(tokens[i], 0, sizeof(tokens[i])); //0으로 초기화 한다 
}

/*오른쪽의 공백을 제거하는 함수*/
char *rtrim(char *_str)
{
	char tmp[BUFLEN];
	char *end;

	strcpy(tmp, _str);
	end = tmp + strlen(tmp) - 1; //end가 tmp문자열의 마지막 문자의 주소값을 가리킨다
	while (end != _str && isspace(*end)) //뒤에서부터 검사하며 문자열을 다 검사할 때 까지 공백문자가 나오면 주소값을 왼쪽으로 하나씩 옮긴다
		//공백문자가 나오면 while문을 빠져나온다
		--end;

	*(end + 1) = '\0'; //마지막으로 검사한 공백문자의 자리를 '\0'로 대신한다 
	_str = tmp; //바꾼 tmp주소값을 _str로 옮긴다 
	return _str;
}

/*왼쪽의 공백을 제거하는 함수*/
char *ltrim(char *_str)
{
	char *start = _str; //char *start 가 _str의 주소값을 가리킨다 

	while (*start != '\0' && isspace(*start)) //왼쪽부터 검사하며 *start가 공백을 대신한 '\0'이 아니고, 공백이 이라면
		++start; //주소값을 오른쪽으로 옮긴다 
	_str = start; //바꾼 start의 주소값을 _str로 옮긴다 
	return _str;
}
/*추가적인 공백을 제거하는 함수*/
char* remove_extraspace(char *str)
{
	int i;
	char *str2 = (char*)malloc(sizeof(char) * BUFLEN);
	char *start, *end;
	char temp[BUFLEN] = "";
	int position;

	//검사하는 문자열에 "include<"가 존재한다면
	if (strstr(str, "include<") != NULL) {
		//start 는 incude< 가 있는 시작주소를 가리키고
		start = str;
		//end 는 <의 주소를 가리키고 
		end = strpbrk(str, "<");
		//position은 두 주소를 뺀 값 
		position = end - start;

		strncat(temp, str, position); //temp 에 str이 가리키는 문자열에 position이 가리키는 문자열을 붙인다
		strncat(temp, " ", 1);//공백을 붙인다
		strncat(temp, str + position, strlen(str) - position + 1);  //str + position이 가리키는 문자열에  strlen(str) - position + 1만큼 ㅂ ㅜㅌ인다 

		str = temp; //str 은 temp 를 가리킴
	}

	//str의 길이만큼 검사함
	for (i = 0; i < strlen(str); i++)
	{
		//공백이 존재한다면 
		if (str[i] == ' ')
		{
			//시작부터 공백이 존재한다면
			if (i == 0 && str[0] == ' ')
				//공백을 건너뛴다
				while (str[i + 1] == ' ')
					i++;
			//시작부터 공백이 존재하지 않는다면
			else {
				//문자가 최소한 하나는 있고, 시작에 공백이 없다면
				if (i > 0 && str[i - 1] != ' ')
					str2[strlen(str2)] = str[i]; //str2배열의 마지막 인덱스에 str[i] 저장 
				while (str[i + 1] == ' ') //공백이 없을 때까지 도는 while문
					i++;
			}
		}
		//공백이 없다면 
		else
			str2[strlen(str2)] = str[i];//str2배열의 마지막 인덱스에 str[i] 저장
	}

	return str2;
}


//문자열의 끝이 나오기 전까지 탐색하는 함수 
void remove_space(char *str)
{
	char* i = str;
	char* j = str;

	//*j가 //공백이나 null을 만나기 전까지 while 문을 돌린다  
	while (*j != 0)
	{
		*i = *j++;// *j는 *i가 탐색할 인덱스를 결정한다 
		if (*i != ' ') //공백이 아니라면 인덱스를 증가
			i++;
	}
	*i = 0;//초기화
}
/*완벽한 괄호(여는괄호 + 닫는괄호)에 대한 정보를 찾는 함수 */
int check_brackets(char *str)
{
	/*#include <string.h>
	const char* strpbrk(const char* str1, const char* str2);
	기능 : 문자열에서 다른 문자열에 들어 있는 문자들을 검색어로 생각하여 찾는다
	return :  가장 첫번째로 일치되는 문자를 가리키는 포인터를 반환한다.
				만일 일치되는 것이 없다면 NULL 을 반환하게 된다.
	const char* str1 : 검색을 수행할 문자열
	const char* str2 : 검색어들을 포함하고 있는 문자열
	*/
	char *start = str;
	int lcount = 0, rcount = 0;

	while (1) {
		if ((start = strpbrk(start, "()")) != NULL) { //start 문자열에서 "()" 이 존재할 때 
			if (*(start) == '(') //여는 괄호를 만났을 때 
				lcount++;
			else //닫는 괄호를 만났을 때 
				rcount++;

			start += 1;
		}
		else
			break;
	}

	if (lcount != rcount) //완벽한 괄호(여는 괄호 + 닫는괄호)가 아닐 때 
		return 0;
	else //완벽한 괄호(여는 괄호 + 닫는 괄호)일 때 
		return 1;
}
/*토큰의 개수를 세는 함수*/
int get_token_cnt(char tokens[TOKEN_CNT][MINLEN])
{
	int i;
	
	for (i = 0; i < TOKEN_CNT; i++)
		if (!strcmp(tokens[i], "")) //토큰이 비었다면
			break;//탈출

	return i;//비기 전까지 센 수를 return 
}
