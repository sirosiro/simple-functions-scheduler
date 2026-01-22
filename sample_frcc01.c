/*
  sample_frcc01.c - SFS + FreeRunCounter Demo

  This sample demonstrates:
    - Simple timer interrupt and task execution using SFS + FreeRunCounter
    - Pseudo timer thread generation using pthread
    - Task termination by checking elapsed time and calling SFS_kill
    - Sharing FreeRunCounter value via global variable
*/
#include <stdio.h>
#include <unistd.h>   /* this is necessary for usleep. */
#include <pthread.h>
#include "sfs.h"
#include "frcc.h"

/*******************************
[ function organization - PlantUML ]

@startuml
!theme plain
skinparam packageStyle rectangle
skinparam defaultFontName Arial
skinparam defaultFontSize 9
skinparam ranksep 140
skinparam nodesep 80
skinparam packagePadding 16

title sample_frcc01.c - SFS + FreeRunCounter Demo

' Packages are declared in vertical order for vertical layout
package "Main Program" {
  class main
  class pseudo_timer_interrupt
  class task1
  class task2
}

package "Thread" {
  class pthread_create
  class pthread_join
}

package "SFS API" {
  class SFS_initialize
  class SFS_fork
  class SFS_dispatch
  class SFS_kill
}

package "FreeRunCounter" {
  class GetFreeRunGapMini
  class gFreeRunCounterMini
  class gFRC_base
}

' Main flow (vertical)
main -down-> SFS_initialize : calls
main -down-> pthread_create : creates timer thread
pthread_create -down-> pseudo_timer_interrupt : runs
main -down-> SFS_fork : calls (TASK1)
SFS_fork -down-> SFS_fork : calls (TASK2)
SFS_fork -down-> SFS_dispatch : loop
SFS_dispatch -down-> task1 : executes
SFS_dispatch -down-> task2 : executes
main -down-> pthread_join : waits for timer thread
main -down-> gFreeRunCounterMini : resets
main -down-> gFRC_base : resets
main -down-> gRunFlag : sets (stop)

pseudo_timer_interrupt -down-> gFreeRunCounterMini : increments (while gRunFlag)

' Task flow (vertical)
task1 -down-> GetFreeRunGapMini : calls
GetFreeRunGapMini -down-> gFRC_base : uses
task1 -down-> SFS_kill : if >= 100

task2 -down-> GetFreeRunGapMini : calls
GetFreeRunGapMini -down-> gFRC_base : uses
task2 -down-> SFS_kill : if >= 200

@enduml
*******************************/

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
