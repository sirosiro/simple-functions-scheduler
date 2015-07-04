#include <stdio.h>
#include "tcb.h"

void task1(void)
{
	printf("task1!\n");
	return;
}
void task2(void)
{
	printf("task2!\n");
	return;
}
int main(void)
{
	int loop=10;

	TCB_initialize();

	TCB_fork("TASK1",0,task1);
	TCB_fork("TASK2",0,task2);

	while(loop--)
		TCB_dispatch();

	return 1;
}
