#include <stdio.h>
#include "sfs.h"

struct workspace {
	unsigned int count;
};

void task1(void)
{
	struct workspace *ows;
	ows = SFS_otherWork("TASK2");

	if(ows->count){
		printf("task1 subtract the counter of task2.\n");
		ows->count--;
	}else{
		printf("task1 end\n");
		SFS_kill();
	}
}

void task2(void)
{
	struct workspace *ws;
	ws = SFS_work();

	if(!ws->count){
		printf("task2 end\n");
		SFS_kill();
	}else{
		printf("task2 count:%d\n",ws->count);
	}
}

void regist_task2(void)
{
	struct workspace *ws;
	ws = SFS_work();

	ws->count = 5;
	SFS_change("TASK2", task2);
}

int main(void)
{
	SFS_initialize();

	SFS_fork("Regist TASK2",0,regist_task2);
	SFS_fork("TASK1",0,task1);

	while(SFS_dispatch());

	printf("All tasks was end.\n");

	return 0;
}
