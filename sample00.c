/************************************************
  Sample00.c - Simple Function Scheduler Demo
  
  This sample demonstrates basic usage of SFS:
  - Initialize the scheduler
  - Create two tasks (task1 and task2)
  - Execute tasks in a loop
  
  Function Organization:
*************************************************/
#include <stdio.h>
#include "sfs.h"

/*******************************
[ function organization - PlantUML ]

@startuml
!theme plain
skinparam packageStyle rectangle
skinparam defaultFontName Arial
skinparam defaultFontSize 9
skinparam ranksep 100
skinparam nodesep 50
skinparam packagePadding 8

title Sample00.c - Simple Function Scheduler Demo

package "Main Program" {
  class main
  class task1
  class task2
}

package "SFS API" {
  class SFS_initialize
  class SFS_fork
  class SFS_dispatch
}

main --> SFS_initialize : calls
SFS_initialize --> SFS_fork : calls (TASK1)
SFS_fork --> SFS_fork : calls (TASK2)
SFS_fork --> SFS_dispatch : calls in loop

SFS_dispatch --> task1 : executes
SFS_dispatch --> task2 : executes

note right of main
  Main program flow:
  1. Initialize SFS
  2. Create two tasks
  3. Execute in loop (5 times)
end note

note right of task1
  Simple task function
  Prints "task1!"
end note

note right of task2
  Simple task function
  Prints "task2!"
end note

note right of SFS_dispatch
  Executes all registered tasks
  in order of priority
end note
@enduml

- Usage Flow -
  ------------------------------
  1. SFS_initialize() - Setup scheduler
  2. SFS_fork("TASK1",0,task1) - Register task1
  3. SFS_fork("TASK2",0,task2) - Register task2
  4. while(loop--) SFS_dispatch() - Execute tasks
  ------------------------------
*******************************/

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
	int loop=5;

	SFS_initialize();

	SFS_fork("TASK1",0,task1);
	SFS_fork("TASK2",0,task2);

	while(loop--)
		SFS_dispatch();

	return 0;
}
