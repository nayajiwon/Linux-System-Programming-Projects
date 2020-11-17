#include "ssu_mntr.h"

int  indent2 = 0;
//int cccnt =0; 

void Tree(){

	makeTree(eval_Dir);

	//전역 변수 초기화 
	indent2 = 0;//깊이 
	//ccccnt = 0;
}


void makeTree(char *fname)
{

    struct dirent **items;
    int nitems, i, j;
    struct stat fstat;
    char per;

   

    //해당 디렉토리로 주소를 변경 
    if (chdir(fname) < 0)
    {
        perror("chdir ");
        exit(1);
    }

   
    //디렉토리의 파일, 디렉토리 개수만큼 
    nitems = scandir(".", &items, NULL, alphasort);

  
    for (i = 0; i < nitems; i++)
    {
        struct stat fstat;

        if ( (!strcmp(items[i]->d_name, ".")) || (!strcmp(items[i]->d_name, "..")) )
        {
            continue;
        }

	//파일인지 디렉토리인지 판별하기 위해 
        lstat(items[i]->d_name, &fstat);

	//파일이나 디렉토리라면 깊이만큼 찍음 
        for (j = 0; j < indent2; j++)
        {
	       if(j==0)
		       printf("|");//check 디렉토리의 1깊이의 자식들을 모두 잇기 위해 
		printf("     \t");
        }


	printf("|\n");	
        for (j = 0; j < indent2; j++)
		printf("     \t");
	printf("ㄴ------%s\n", items[i]->d_name);
	

  
	//디렉토리라면 재귀를 들어감 
        if (S_ISDIR(fstat.st_mode))
        {
            indent2 ++;
            makeTree(items[i]->d_name);   
        }
    }


    indent2 --;
    chdir("..");
}






