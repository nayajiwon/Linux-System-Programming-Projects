#include "ssu_mntr.h"

char ch[BUF_LEN];//찾는 파일 

/**L옵션을 입력받았을 경우 오래된 파일의 순서대로 출력하는 함수**/
void printLoption(char fname[BUF_LEN]){
	comp compare[BUF_LEN];
	comp temp;
	int ind; 

	//구조체에 오래된 순서대로 담아옴
	ind = getDtimeinfo(compare);
	
	/**************************************답 출력**********************************/
	for(int i=0; i<ind; i++)
		printf("%d. %s		 %s", i+1, compare[i].fname, compare[i].Dtime);
	/**************************************답 출력**********************************/

	//입력받은 파일을 복구시킴
	recover(fname); 

}
/**옵션이 주어졌을 경우 D: 에 관련된 정보들을 가져오는 함수**/
int getDtimeinfo(comp compare[BUF_LEN]){
        
	FILE *fp;
        struct dirent **namelist;
        int cnt;
        int count;
        char *ptr;
        int samecnt = 0;
        char infoFile[BUF_LEN];
        char buf[BUF_LEN];

        int ind =0;


        //info디렉토리의 모든 파일을 검사하다 
        if((count = scandir(InfoDir, &namelist, NULL, alphasort)) == -1)
                return -1;
        
	for(int i=0; i<count; i++){
                cnt = 0;
		//. , .. 은 무시
                if ( (!strcmp(namelist[i]->d_name, ".")) || (!strcmp(namelist[i]->d_name, "..")) )

                 {
                   continue;
                 }

		
		//info디렉토리를 엶
                sprintf(infoFile, "%s/%s", InfoDir, namelist[i]->d_name);

		//info디렉토리의 info파일을 엶
                fp = fopen(infoFile, "r");

		//파일이 끝날때 까지 읽는다 
                while(!feof(fp)){

                        memset(buf,0,256);

			fgets(buf, 256, fp);//한줄씩 읽음
			//세번째 줄의 D 정보를 읽어 구조체 배열에 복사한다 
                        if(cnt == 2){
                                strcpy(compare[ind].fname, namelist[i]->d_name);//이름
                                strcpy(compare[ind].Dtime, buf);//삭제시간
				++ind;
                        }
                        ++cnt;
                }
        
	}

	//시간 순서대로 sort 한 뒤, 그 정보를 compare구조체에 저장하여 리턴한다 
	sortDtimeinfo(compare, ind);
	
	//찾는 파일의 개수 만큼 리턴 
	return ind;

}
/***시간순으로 sort하는 함수***/
void sortDtimeinfo(comp compare[BUF_LEN], int ind){
		

        comp temp;
	

        //문자열 순서대로 sort 
	//삭제정보를 가지고 있는 구조체 배열을 삭제 시간이 오래된 순서대로 (숫자가 작으면 앞에) sort한다
	//숫자가 작다 = 더 먼저 삭제 됐다 
	for(int i=0; i<ind-1; i++){
                for(int p = i+1; p<ind; p++)
                {
                        if(strcmp(compare[i].Dtime, compare[p].Dtime)>0)//앞에 것이 크다 == 최근에 삭제됐다 
                        {
				//구조체이기 때문에 안의 모든 멤버들을 swap해준다 
                                strcpy(temp.Dtime, compare[i].Dtime);
                                strcpy(temp.fname, compare[i].fname);

                                strcpy(compare[i].Dtime, compare[p].Dtime);
                                strcpy(compare[i].fname, compare[p].fname);

                                strcpy(compare[p].Dtime, temp.Dtime);
                                strcpy(compare[p].fname, temp.fname);



                        }
                }
	}


	return;

}

/**원하는 파일의 동일한 이름의 개수에 따라 사용자가 선택 할 수 있게 하는 함수 **/ 
void recover(char fname[BUF_LEN]){
	int cnt = 0;	
	
	int flen;
	int dlen;
	int mlen;

	char ans[3];

	strcpy(ch, fname);
	

	lapInfo sameinfo[FILE_LEN];
	

	//_이후 동일한 파일의 이름의 개수 
	cnt = getInfo_information( InfoDir,fname,sameinfo);


	// 동일한 파일이 없다면 ( = 찾는 파일이 없다면 ) 출력
	if(cnt == 0)
	{
		printf("There is no '%s' in the 'trash' directory\n", ch);
		return; 
	}


	//동일한 파일의 이름만큼 
	for(int i=0; i<cnt; i++){
		//printf("%d\n%s\n%s\n%s\n%s\n\n",i,sameinfo[i].dirname ,sameinfo[i].D,sameinfo[i].M, sameinfo[i].fname);
	

		flen = strlen( sameinfo[i].dirname);
		dlen = strlen( sameinfo[i].D);
		mlen = strlen( sameinfo[i].M);

		//개행을 지워줌 
		sameinfo[i].dirname[flen-1] = '\0';
		sameinfo[i].D[dlen-1] = '\0';
		sameinfo[i].M[mlen-1] = '\0';
	}

	
	//찾는 파일이 한개라면 바로 recover 실행 
	if(cnt == 1)
		putbackinCheck(&sameinfo[0]);

	//2개 이상이라면 옵션을 제공함 
	else if(cnt >1)
	{
		/****************************답 출력*******************************/
		for(int i=0; i<cnt; i++)
		printf("%d. %s %s %s\n", i+1, fname, sameinfo[i].D, sameinfo[i].M); 
		/****************************답 출력*******************************/
	
		printf("Choose : ");
		fgets(ans, 3, stdin);
	}
		
	//선택한 옵션을 recover 
	putbackinCheck(&sameinfo[atoi(ans)-1]);
	
	//printf("** %s\n", fname);
	
	
}
/**_이후의 이름이 동일하다면 _를 포함한 이름을 구조체 배열에 저장하는 함수**/
int getInfo_information(char dir[BUF_LEN], char fname[BUF_LEN], lapInfo sameInfo[BUF_LEN]){
	//dir: Info디렉토리 라고 생각할것 

	//printf("인포 주소: %s\n",dir);
	//printf("찾는 파일 %s\n", fname);

	struct dirent **namelist;
        int count;
        char *ptr;
	int samecnt = 0;
	char temp[BUF_LEN];


	//인포 디렉토리의 파일들을 하나하나씩 검사 
        if((count = scandir(dir, &namelist, NULL, alphasort)) == -1)
                return -1;

	for(int i=0; i<count; i++){
		//. , ..일경우 건너뜀
                if ( (!strcmp(namelist[i]->d_name, ".")) || (!strcmp(namelist[i]->d_name, "..")) )

                 {
                   continue;
                 }

		memset(temp, 0, BUF_LEN);
		
		strcpy(temp, namelist[i]->d_name); 
		
		char *ptr = strrchr(temp, '_');  //_이후의 실제 파일 이름만 가리킴
		ptr += 1; 


		if(!strcmp(ptr, fname)){//recover 할 파일을 info에서 찾았다면 
			fgetsInfofile(namelist[i]->d_name, &sameInfo[samecnt]);//info안의 정보들을 구조체 안에 저장하고 
			strcpy(sameInfo[samecnt].fname, namelist[i]->d_name); //파일의 이름(_포함)을 저장함
			samecnt++;
		}
    
	}
                 
					 
	


	
	return samecnt;
}
	
/**_ 이후의 동일한 이름을 가진 인포 파일을 열고 파일의 원래 절대주소, 삭제시간, 수정시간을 순서대로 구조체 변수에 복사하는 함수**/
int fgetsInfofile(char fname[BUF_LEN],  lapInfo* sameInfo)
{
	//printf("********\n");
	FILE *pFile = NULL;
	int cnt = 0; 
	char fdir[BUF_LEN];

	//절대주소로 만듦
	strcpy(fdir, InfoDir);
	strcat(fdir, "/");
	strcat(fdir, fname); 


	//인포 파일을 읽기모드로 연다
	pFile = fopen( fdir, "r" );

	if( pFile != NULL )
	{
		char strTemp[BUF_LEN];
		char *pStr;

		while( !feof( pFile ) )//파일의 끝을 만날 때 까지 
		{
			
			pStr = fgets( strTemp, sizeof(strTemp), pFile );//한줄씩 읽는다 
			if(cnt == 1)
				strcpy(sameInfo->dirname, strTemp);//절대주소
			if(cnt == 2)
				strcpy(sameInfo->D, strTemp);//삭제시간
			if(cnt == 3)
				strcpy(sameInfo->M, strTemp);//변형시간

			++cnt;
		}
		fclose( pFile );//인포파일을 닫음 
	}
	else
	{
		printf("-error-\n");//파일이 없다면 에러처리한다 
		//에러 처리
	}

	return 0;
}

/**info, files 에서 삭제하고, 원래 주소로 복귀  하는 함수**/ 
void putbackinCheck(lapInfo* sameInfo ){


	//printf("pbc--> %s	\n", ch);

		//printf("%s\n%s\n%s\n%s\n",sameInfo->dirname ,sameInfo->D,sameInfo->M, sameInfo->fname);

	struct dirent **namelist;
        
	int count;
        char *ptr;
        int samecnt = 0;
        char temp[BUF_LEN];
        char temp2[BUF_LEN];
	char temp3[BUF_LEN];
	int ind;
	char imsi[BUF_LEN];
        char imsi2[3];

	char byeinfo[BUF_LEN+10];




        //path: trash
        if((count = scandir(FileDir, &namelist, NULL, alphasort)) == -1) 
                return; 

        for(int i=0; i<count; i++)
	{
                //. , .. 는 무
		if ( (!strcmp(namelist[i]->d_name, ".")) || (!strcmp(namelist[i]->d_name, "..")) )

                 {
                   continue;
                 }
		
		//printf(" 검사 중1 %s\n", namelist[i]->d_name);

		if(!strcmp(namelist[i]->d_name, sameInfo->fname)){
			sprintf(temp, "%s/files/%s", trashAddr, namelist[i]->d_name);

			ind = checkFinalNum();// 동일한 이름을 가진 파일 중 _이전의 숫자중 최고값 중 하나 큰 수 리턴
			if(ind < 0){//check에 똑같은 이름이 없으면 _앞에 번호를 붙이지 않고 북귀

				sprintf(byeinfo, "%s/%s", InfoDir, sameInfo->fname);// info의 지정 파일의 절대주소
				rename(temp, sameInfo->dirname); //트래시 디렉토리에 있는 파일을 Info에 적어둔 원래 주소로 복귀
				remove(byeinfo);//  info의 파일 삭제 
				
				return; 
			}

			//똑같은 이름이 있다면 _ 앞에 알맞은 번호를 붙이고 ㅂ복귀 
			sprintf(imsi, "%s/%d_%s", eval_Dir, ind, ch);//동일한 이름을 가진 파일 중 _이전의 숫자중 최고값 보다 하나 큰 수_이름 형식의 절대 주소를 만듦
			sprintf(byeinfo, "%s/%s", InfoDir, sameInfo->fname); //삭제할 info의 절대주소 
			rename(temp, imsi);//복귀 		
			remove(byeinfo);//info삭제 
				
		}

	}

}


int checkFinalNum(){
	//printf("cFinalnum\n");	

        struct dirent **namelist;

        int count;
        char *ptr;
        int samecnt = 0;
        char temp[BUF_LEN];
	int len;
	char num[3];
	int flag = 0; 


        //path
        if((count = scandir(eval_Dir, &namelist, NULL, alphasort)) == -1)
                return -1;

        for(int i=0; i<count; i++){
                if ( (!strcmp(namelist[i]->d_name, ".")) || (!strcmp(namelist[i]->d_name, "..")) )

                 {
                   continue;
                 }



	        memset(temp, 0, BUF_LEN);

                strcpy(temp, namelist[i]->d_name);

               // printf("리스트: %s, %s---\n", namelist[i]->d_name, temp);

                char *ptr1 = strrchr(temp, '_');
		
		if(ptr1 == NULL){
			//따로 비교를 해줘야함 
			if(!strcmp(temp, ch)){ //3.txt 형식으로, _없이  
				//printf("_가 없으니, 1을 붙인다\n");
				if(flag != 1)//_이전에 숫자가 있었다면, trash파일에 들어갈 파일은 무조건 어떤 숫자가 나와야 하기 때문에 
					flag = 2;//이 플래그가 바뀌면 
			}

			continue; //incase,  2.txt 검사 하고, 1_2.txt 가 다음에 존재할 수 있으니.. 
		
		}

                //printf("-->%s\n",ptr1);
                ptr1 += 1;
		

		if(!strcmp(ptr1, ch)){
                       char *ptr2 = strtok(temp, "_");
                       len = strlen(num);
                       memset(num, 0, len);
         	       strcpy(num, ptr2);//num를 알아냄
        	       flag = 1;
		}



	}
	if(flag == 2)
		return 1; //하나가 이미 존재 하니, 가서 1_로 만들어라 


	if(flag == 0)
		return -1;//아무것도 없으니 그냥 a.txt 형식으로 만들어라 

	return atoi(num)+1;
}












