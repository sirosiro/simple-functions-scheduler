/************************************************
  Sample01.c - SFS Workspace and Task Change Demo
  
  This sample demonstrates:
  - Using SFS_work() to access task workspace
  - Using SFS_change() to modify task functions
  - Common task function with different workspace data
  
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

title Sample01.c - SFS Workspace and Task Change Demo

package "Main Program" {
  class main
  class regist_task1
  class regist_task2
  class common_task
}

package "Data Structure" {
  class workspace
}

package "SFS API" {
  class SFS_initialize
  class SFS_fork
  class SFS_dispatch
  class SFS_work
  class SFS_change
}

main --> SFS_initialize : calls
SFS_initialize --> SFS_fork : calls (Regist TASK1)
SFS_fork --> SFS_fork : calls (Regist TASK2)
SFS_fork --> SFS_dispatch : calls in loop

SFS_dispatch --> regist_task1 : executes
SFS_dispatch --> regist_task2 : executes
regist_task1 --> SFS_work : gets workspace
regist_task2 --> SFS_work : gets workspace
regist_task1 --> workspace : sets task_no=1
regist_task2 --> workspace : sets task_no=2
regist_task1 --> SFS_change : changes to common_task
regist_task2 --> SFS_change : changes to common_task

SFS_dispatch --> common_task : executes (TASK1)
SFS_dispatch --> common_task : executes (TASK2)
common_task --> SFS_work : gets workspace
common_task --> workspace : reads task_no

note right of main
  Main program flow:
  1. Initialize SFS
  2. Register two registration tasks
  3. Execute in loop (5 times)
end note

note right of regist_task1
  Registration task for TASK1:
  1. Get workspace
  2. Set task_no = 1
  3. Change to common_task
end note

note right of regist_task2
  Registration task for TASK2:
  1. Get workspace
  2. Set task_no = 2
  3. Change to common_task
end note

note right of common_task
  Common task function:
  - Gets workspace
  - Prints "task{task_no}!"
end note

note right of workspace
  Task workspace structure:
  - task_no: Task identifier
end note
@enduml

- Usage Flow -
  ------------------------------
  1. SFS_initialize() - Setup scheduler
  2. SFS_fork("Regist TASK1",0,regist_task1) - Register task1 setup
  3. SFS_fork("Regist TASK2",0,regist_task2) - Register task2 setup
  4. regist_task1/2: Set workspace data and change to common_task
  5. common_task: Execute with different workspace data
  ------------------------------
*******************************/

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
	SFS_change("TASK1", 0, common_task);
}

void regist_task2(void)
{
	struct workspace *ws;
	ws = SFS_work();

	ws->task_no = 2;
	SFS_change("TASK2", 0, common_task);
}

int main(void)
{
	int loop=5;

	SFS_initialize();

	SFS_fork("Regist TASK1",0,regist_task1);
	SFS_fork("Regist TASK2",0,regist_task2);

	while(loop--)
		SFS_dispatch();

	return 0;
}
