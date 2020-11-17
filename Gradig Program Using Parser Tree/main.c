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
	�����ϸ� 0 �����ϸ� -1�� �����Ѵ�.


	struct timezone
	{
		int tz_minuteswest:  // �׸���ġ ��������
		int tz_dsttime       // DST ����Ÿ��(�ϱ� ����ð�)
	}

	
	*/
	struct timeval begin_t, end_t;
	gettimeofday(&begin_t, NULL); //���۽ð� 

	ssu_score(argc, argv);

	gettimeofday(&end_t, NULL); //���ð� 
	ssu_runtime(&begin_t, &end_t); 

	exit(0);
}

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
	end_t->tv_sec -= begin_t->tv_sec; //�ʴ����� ���ð����� ���۽ð��� ���ش� 

	if(end_t->tv_usec < begin_t->tv_usec){//����ũ�� ���� �ʰ� �� Ŭ ��� 
		end_t->tv_sec--; //�ʸ� ����ũ���ʷ� ��ȯ�ϱ� ���� �ʸ� -1 ���� 
		end_t->tv_usec += SECOND_TO_MICRO; //�ʸ� ����ũ�� �ʷ� ��ȯ�Ѵ� 
	}

	end_t->tv_usec -= begin_t->tv_usec;//�ʴ����� ���ð����� ���۽ð��� ���ش�
	printf("Runtime: %ld:%06ld(sec:usec)\n", end_t->tv_sec, end_t->tv_usec);
}
