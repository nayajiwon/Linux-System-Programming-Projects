
#include "ssu_mntr.h"
//extern char eval_Dir[BUF_LEN];
//extern char saved_path[BUF_LEN];



long int total_size = 0;
int  indent = 0;
void getDIRsize(char *fName, char depth[FILE_LEN] ){
	char rPath[BUF_LEN];
	int dpth; 
	char ans[BUF_LEN];
	char dAns[BUF_LEN];
        struct stat fstat;
	strcpy(rPath, fName);
	srepath(rPath);//절대 경로를 계산합니다 

	
	
	//access 함수로 존재하는지 안하는지 여부를 알아내야함 

	if(access(rPath, F_OK) < 0){ 
                fprintf(stderr, "size 할 파일이 이 존재하지 않습니다\n");
                return;
        }
		
	if(!strcmp(depth, "no")){ //옵션을 받지 않았을 때 
        	
		lstat(rPath, &fstat);
        	
		if (!S_ISDIR(fstat.st_mode))//디렉토리가 아니라면 stat 구조체를 통해 바로 파일 사이즈를 구한다 
			total_size = fstat.st_size;
		else//디렉토리라면 재귀를 통해 하위 파일의 크기의 합을 구한다 
			getsize(rPath);	
	}
	else 	//-d옵션을 받았을 때 
		dsizeOption(rPath, atoi(depth)-1, dAns);
	

	relativePath(rPath, ans);//상대경로를 구하기 위한 함수 

	/**************답 출력*****************/
	printf("%ld	%s\n", total_size,ans);
	/**************답 출력****************/
	

	//전역변수 초기
	total_size = 0;
	indent = 0;

}

/**출력하기 위한 상대경로 구하는 함수**/
void relativePath(char realPath[BUF_LEN], char ans[BUF_LEN]){
       
	  //1.절대 경로인 거 확실히 할 것. 슬래시 차로 구함 
        
	char Exple[BUF_LEN];
	char Eval[BUF_LEN];
       	char relative[BUF_LEN];
	int len;
	int i;
	char *ptr;

	strcpy(Exple, saved_path);//현재 실행 디렉토리 
        strcpy(Eval, realPath);//현재 참조하는 파일의 절대경로 
	
	len = strlen(realPath);

	for(i=0; i<len; i++){
		if(Exple[i] != Eval[i])//절대경로가 실행 디렉토리보다 큰 시점에서 break
			break;
	}

	ptr = Eval;

	//그 이후의 디렉토리는 상대경로이다 
	ptr += i;
	
	sprintf(relative, ".%s", ptr);
	
	strcpy(ans, relative);

}

 
/*전체 크기를 구하는 함수*/
void getsize(char *wd)
{
    

	
	struct dirent **items;
	int nitems, i, j;
    	struct stat fstat;
   	char per;
   
    	// 인자로 받은 디렉토리로 이동한다.
   	if (chdir(wd) < 0){
     		  
		fprintf(stderr, "size 하기 위한 디렉토리가 존재하지 않습니다 \n");
		return; 
    	}

	// 모든 파일과 디렉토리의 내용을 가져온다.
	nitems = scandir(".", &items, NULL, alphasort);


    	// 만약 해당 파일이 디렉토리 일경우
	for (i = 0; i < nitems; i++)
	{
		struct stat fstat;

      		  // ., ..  는 무시한다.
        	if ( (!strcmp(items[i]->d_name, ".")) || (!strcmp(items[i]->d_name, "..")) )
	        	   	 continue;


        	// 파일의 상태를 얻어와서 fstat 로 저장한다. 
		lstat(items[i]->d_name, &fstat);

 	
		if (!S_ISDIR(fstat.st_mode)){ //파일이면 크기를 더한디 
        
			total_size += fstat.st_size;

		}
      
		// 그리고 디렉토리의 depth 레벨을 1 증가 한다.
       
		if (S_ISDIR(fstat.st_mode)){	
			indent ++;
       			getsize(items[i]->d_name);        
		}	
    
	
	}


    	// 디렉토리의 depth 레벨을 1 감소시키고
    	// 하위 디렉토리로 이동한다.
    
	indent --;
    
	chdir("..");
}

/**원하는 깊이의 디렉토리 합을 구하는 함수**/
void dsizeOption(char *wd, int height, char ans[BUF_LEN])
{
	struct dirent **items;
   
	int nitems, i, j;
   
	struct stat fstat;
    
	char per;
	
	char rl[BUF_LEN];

   
    	// 인자로 받은 디렉토리로 이동한다.
	if (chdir(wd) < 0){
       
		fprintf(stderr, "size 하기 위한 디렉토리가 존재하지 않습니다 \n");

		return; 
    
	}


    	// 모든 파일과 디렉토리의 내용을 가져온다.
    	nitems = scandir(".", &items, NULL, alphasort);

   
    
	//재귀 돌리기 
	for (i = 0; i < nitems; i++)
	{
        
		struct stat fstat;

		// ., ..  는 무시한다
		if ( (!strcmp(items[i]->d_name, ".")) || (!strcmp(items[i]->d_name, "..")) )
 		           continue;

        	// 파일의 상태를 얻어와서 fstat 로 저장한다.  
		lstat(items[i]->d_name, &fstat);


		if (!S_ISDIR(fstat.st_mode)){//파일이면 크기를 더한디 

			total_size += fstat.st_size;
	    
			if(indent < height)//원하는 깊이이하의 모든 디렉토리 및 파일들의 합을 출력한다 
			{			
				realpath(items[i]->d_name, rl);
				relativePath(rl, ans);//상대주소로 출력한다 
				printf("%ld	%s\n", fstat.st_size, ans);

			}
		}

		if (S_ISDIR(fstat.st_mode))
		{
          
			indent ++;//디렉토리라면 재귀를 돌아 다음 깊이의 디렉토리 사이즈를 구한다 
			dsizeOption(items[i]->d_name, height, ans);
	    
			if(indent < height)//원하는 깊이 이하라면 
			{
       				realpath(items[i]->d_name, rl);
				relativePath(rl, ans);//상대주소로 출력 
				printf("%ld	%s\n",total_size, ans);

			}
 
		}
    
	}

    // 디렉토리의 depth 레벨을 1 감소시키고
    // 하위 디렉토리로 이동한다.
 
	indent --;
  
	chdir("..");
}

/**입력이 어떤 경로이든 절대경로로 바꿔주는 함수**/
void srepath(char path[BUF_LEN]){

        char Exple[BUF_LEN];
        char Eval[BUF_LEN];
        char Eval2[BUF_LEN];
        char temp[BUF_LEN];
        int len;
        int flag=0;
        int i = 0;

        strcpy(Exple, saved_path);//현재 실행 디렉토리 
        strcpy(Eval, path);//파일 
        strcpy(Eval2, path);//파일 

        len = strlen(saved_path);


        for(i=0; i<len; i++){//실행 디렉토리와 대상을 비교함 
                if(saved_path[i] != path[i])
                        break;
        }

        if(i == len){//절대경로
        }
    
        if(i < len)//대상 파일의 경로가 더 작다면 상대경로이다  
        {

                char * ptr = strtok(Eval, "/");
                if(!strcmp(ptr, ".")){
                        ptr = Eval2;
                        ptr += 2;
                        strcpy(Eval2, ptr);
                }

                strcpy(temp, saved_path);//현재 실행 절대 경로에 상대경로를 잇는다 
                strcat(temp, "/");
                strcat(temp, Eval2);

                strcpy(path, temp);
        }


        return;


}

