/************************************************
  Sample03.c - SFS Inter-Task Communication Demo
  
  This sample demonstrates:
  - Using SFS_otherWork() to access other task's workspace
  - Inter-task communication and coordination
  - Task termination based on shared data
  - Priority-based task execution (TASK1:1, TASK2:1)
  
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

title Sample03.c - SFS Inter-Task Communication Demo

package "Main Program" {
  class main
  class task1
  class task2
  class regist_task2
}

package "Data Structure" {
  class workspace
}

package "SFS API" {
  class SFS_initialize
  class SFS_fork
  class SFS_dispatch
  class SFS_work
  class SFS_otherWork
  class SFS_change
  class SFS_kill
}

main --> SFS_initialize : calls
SFS_initialize --> SFS_fork : calls (TASK1, priority 1)
SFS_fork --> SFS_fork : calls (Regist TASK2, priority 0)
SFS_fork --> SFS_dispatch : calls until all tasks end

SFS_dispatch --> regist_task2 : executes first (priority 0)
regist_task2 --> SFS_work : gets workspace
regist_task2 --> workspace : sets count = 5
regist_task2 --> SFS_change : changes to task2

SFS_dispatch --> task1 : executes (priority 1)
SFS_dispatch --> task2 : executes (priority 1)
task1 --> SFS_otherWork : gets TASK2's workspace
task2 --> SFS_work : gets own workspace
task1 --> workspace : reads/decrements count
task2 --> workspace : reads count
task1 --> SFS_kill : when count reaches 0
task2 --> SFS_kill : when count reaches 0

note right of main
  Main program flow:
  1. Initialize SFS
  2. Register TASK1 (priority 1)
  3. Register TASK2 setup (priority 0)
  4. Execute until all tasks terminate
end note

note right of task1
  Task1 (priority 1):
  - Accesses TASK2's workspace via SFS_otherWork
  - Decrements TASK2's count
  - Terminates when count reaches 0
end note

note right of task2
  Task2 (priority 1):
  - Accesses own workspace via SFS_work
  - Displays current count
  - Terminates when count reaches 0
end note

note right of regist_task2
  TASK2 registration (priority 0):
  - Gets workspace
  - Sets initial count = 5
  - Changes to task2 function
end note

note right of workspace
  Task workspace structure:
  - count: Shared counter between tasks
end note

note right of SFS_otherWork
  Allows task to access
  another task's workspace
end note
@enduml

- Usage Flow -
  ------------------------------
  1. SFS_initialize() - Setup scheduler
  2. SFS_fork("TASK1",1,task1) - Register task1 (priority 1)
  3. SFS_fork("Regist TASK2",0,regist_task2) - Register setup (priority 0)
  4. regist_task2: Initialize TASK2 workspace and change to task2
  5. task1: Decrement TASK2's counter via SFS_otherWork
  6. task2: Display counter value
  7. Both tasks terminate when counter reaches 0
  ------------------------------
*******************************/

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

	if(ws->count){
		printf("task2 count:%d\n",ws->count);
	}else{
		printf("task2 end\n");
		SFS_kill();
	}
}

void regist_task2(void)
{
	struct workspace *ws;
	ws = SFS_work();

	ws->count = 5;
	SFS_change("TASK2", 1, task2);
}

int main(void)
{
	SFS_initialize();

	SFS_fork("TASK1",1,task1);
	SFS_fork("Regist TASK2",0,regist_task2);

	while(SFS_dispatch());

	printf("All tasks was end.\n");

	return 0;
}
