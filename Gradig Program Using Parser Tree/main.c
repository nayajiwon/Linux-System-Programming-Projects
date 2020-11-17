#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_score.h"

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(int argc, char *argv[])
{
	/*
	#include <sys/time.h>
	#include <unistd.h>
	int gettimeofday(struct timeval *tv, struct timezone *tz);
	성공하면 0 실패하면 -1을 리턴한다.


	struct timezone
	{
		int tz_minuteswest:  // 그리니치 서측분차
		int tz_dsttime       // DST 보정타입(일광 절약시간)
	}

	
	*/
	struct timeval begin_t, end_t;
	gettimeofday(&begin_t, NULL); //시작시간 

	ssu_score(argc, argv);

	gettimeofday(&end_t, NULL); //끝시간 
	ssu_runtime(&begin_t, &end_t); 

	exit(0);
}

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
	end_t->tv_sec -= begin_t->tv_sec; //초단위의 끝시간에서 시작시간을 빼준다 

	if(end_t->tv_usec < begin_t->tv_usec){//마이크로 보다 초가 더 클 경우 
		end_t->tv_sec--; //초를 마이크로초로 변환하기 위해 초를 -1 해줌 
		end_t->tv_usec += SECOND_TO_MICRO; //초를 마이크로 초로 변환한다 
	}

	end_t->tv_usec -= begin_t->tv_usec;//초단위의 끝시간에서 시작시간을 빼준다
	printf("Runtime: %ld:%06ld(sec:usec)\n", end_t->tv_sec, end_t->tv_usec);
}
