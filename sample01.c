#include <stdio.h>
#include "sfs.h"

struct workspace {
	unsigned int task_no;
};

void common_task(void)
{
	struct workspace *ws;
	ws = SFS_work();

	printf("task%d!\n",ws->task_no);
}

void regist_task1(void)
{
	struct workspace *ws;
	ws = SFS_work();

	ws->task_no = 1;
	SFS_change("TASK1", common_task);
}

void regist_task2(void)
{
	struct workspace *ws;
	ws = SFS_work();

	ws->task_no = 2;
	SFS_change("TASK2", common_task);
}

int main(void)
{
	int loop=10;

	SFS_initialize();

	SFS_fork("Regist TASK1",0,regist_task1);
	SFS_fork("Regist TASK2",0,regist_task2);

	while(loop--)
		SFS_dispatch();

	return 1;
}
