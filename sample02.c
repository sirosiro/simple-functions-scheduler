/************************************************
  Sample02.c - SFS Task Termination Demo
  
  This sample demonstrates:
  - Using SFS_kill() to terminate tasks
  - Task termination based on execution count
  - Automatic scheduler termination when all tasks end
  
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

title Sample02.c - SFS Task Termination Demo

package "Main Program" {
  class main
  class task1
  class task2
}

package "Global Variables" {
  class execution_count_of_task1
  class execution_count_of_task2
}

package "SFS API" {
  class SFS_initialize
  class SFS_fork
  class SFS_dispatch
  class SFS_kill
}

main --> SFS_initialize : calls
SFS_initialize --> SFS_fork : calls (TASK1)
SFS_fork --> SFS_fork : calls (TASK2)
SFS_fork --> SFS_dispatch : calls until all tasks end

SFS_dispatch --> task1 : executes
SFS_dispatch --> task2 : executes
task1 --> execution_count_of_task1 : decrements
task2 --> execution_count_of_task2 : decrements
task1 --> SFS_kill : when count reaches 0
task2 --> SFS_kill : when count reaches 0

task1 --> execution_count_of_task1 : reads/writes
task2 --> execution_count_of_task2 : reads/writes

note right of main
  Main program flow:
  1. Initialize SFS
  2. Register two tasks with counters
  3. Execute until all tasks terminate
  4. Print completion message
end note

note right of task1
  Task1 execution:
  - Prints "task1-{count}!"
  - Decrements execution_count_of_task1
  - Calls SFS_kill() when count reaches 0
end note

note right of task2
  Task2 execution:
  - Prints "task2-{count}!"
  - Decrements execution_count_of_task2
  - Calls SFS_kill() when count reaches 0
end note

note right of execution_count_of_task1
  Initial value: 6
  Decremented by task1
end note

note right of execution_count_of_task2
  Initial value: 3
  Decremented by task2
end note

note right of SFS_dispatch
  Returns number of executed tasks
  Returns 0 when no tasks remain
end note
@enduml

- Usage Flow -
  ------------------------------
  1. SFS_initialize() - Setup scheduler
  2. SFS_fork("TASK1",0,task1) - Register task1 (6 executions)
  3. SFS_fork("TASK2",0,task2) - Register task2 (3 executions)
  4. while(SFS_dispatch()) - Execute until all tasks end
  5. Print "All tasks was end."
  ------------------------------
*******************************/

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
