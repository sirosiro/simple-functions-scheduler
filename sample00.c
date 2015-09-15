#include <stdio.h>
#include "sfs.h"

void task1(void)
{
	printf("task1!\n");
}
void task2(void)
{
	printf("task2!\n");
}
int main(void)
{
	int loop=10;

	SFS_initialize();

	SFS_fork("TASK1",0,task1);
	SFS_fork("TASK2",0,task2);

	while(loop--)
		SFS_dispatch();

	return 1;
}
