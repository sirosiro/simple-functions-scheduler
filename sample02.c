#include <stdio.h>
#include "sfs.h"

int execution_count_of_task1 = 6;
int execution_count_of_task2 = 3;

void task1(void)
{
	printf("task1-%d!\n",execution_count_of_task1--);

	if(!execution_count_of_task1)
		SFS_kill();
}
void task2(void)
{
	printf("task2-%d!\n",execution_count_of_task2--);

	if(!execution_count_of_task2)
		SFS_kill();
}
int main(void)
{
	SFS_initialize();

	SFS_fork("TASK1",0,task1);
	SFS_fork("TASK2",0,task2);

	while(SFS_dispatch());

	printf("All tasks was end.\n");

	return 0;
}
