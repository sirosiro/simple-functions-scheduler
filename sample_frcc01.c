#include <stdio.h>
#include <unistd.h>   /* this is necessary for usleep. */
#include <pthread.h>
#include "sfs.h"
#include "frcc.h"

unsigned char gRunFlag = 1;

void * pseudo_timer_interrupt(void *arg)
{
	printf("Pseudo Timer Start.\n");

	while(gRunFlag){
		gFreeRunCounterMini++;
		usleep(10*1000);
	}

	printf("Pseudo Timer End.\n");

	return(arg);
}

unsigned char gFRC_base = 0;

void task1(void)
{
	unsigned char past_time = GetFreeRunGapMini(gFRC_base);

	if(past_time >= 100){
		printf("task1! %dms\n", past_time * 10);
		SFS_kill();
	}
}

void task2(void)
{
	unsigned char past_time = GetFreeRunGapMini(gFRC_base);

	if(past_time >= 200){
		printf("task2! %dms\n", past_time * 10);
		SFS_kill();
	}
}

int main(void)
{
	pthread_t tid;

	SFS_initialize();
	gFreeRunCounterMini = 0;
	gFRC_base = 0;

	pthread_create(&tid,NULL,pseudo_timer_interrupt,(void *)NULL);
	SFS_fork("TASK1",0,task1);
	SFS_fork("TASK2",0,task2);

	while(SFS_dispatch());

	gRunFlag = 0;
	pthread_join(tid, NULL);

	return 0;
}
