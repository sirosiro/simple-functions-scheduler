/************************************************
  Simple Function Scheduler
  
  $Id:$
*************************************************/
#ifndef __SFS_INC__
#define __SFS_INC__

#define SFS_NAME_SIZE 16
#define SFS_WORK_SIZE 32
/* Task Control Block */
struct SFS_tg {
  char name[SFS_NAME_SIZE];
  unsigned short order;
  struct SFS_tg *pFront;
  struct SFS_tg *pBack;
  /* ---------- */
  void (*pFunction)(void);
  char work[SFS_WORK_SIZE];
};
/*******************************
[ function organization - PlantUML ]

@startuml
!theme plain
skinparam packageStyle rectangle
skinparam defaultFontName Arial
skinparam defaultFontSize 10
skinparam ranksep 80
skinparam nodesep 40
skinparam packagePadding 10

package "SFS Public API" {
  class SFS_initialize
  class SFS_dispatch
  class SFS_fork
  class SFS_kill
  class SFS_change
  class SFS_work
  class SFS_otherWork
}

package "Internal Functions" {
  class SFS_obtain
  class SFS_regist
  class SFS_release
  class SFS_giveup
  class SFS_find
  class none
}

package "Utility Functions" {
  class strncpy
  class strcnt
  class strcmp
}

' Vertical layout - main flow
SFS_initialize --> SFS_dispatch
SFS_dispatch --> SFS_fork
SFS_fork --> SFS_kill
SFS_kill --> SFS_change

' Internal function calls
SFS_fork --> SFS_obtain : calls
SFS_fork --> SFS_regist : calls
SFS_kill --> SFS_giveup : calls
SFS_giveup --> SFS_release : calls
SFS_otherWork --> SFS_find : calls
SFS_change --> strncpy : calls
SFS_find --> strcmp : calls
strncpy --> strcnt : calls

' Additional internal dependencies
SFS_regist --> strncpy : calls
SFS_find --> strcmp : calls

note right of SFS_initialize
  Initialize task management system
  Setup pool and task list
end note

note right of SFS_dispatch
  Execute all tasks
  Call functions in order
end note

note right of SFS_fork
  Create and register new task
  Obtain task block from pool
end note

note right of SFS_kill
  Terminate current running task
  Call giveup function on next execution
end note
@enduml

- Usage -
  ------------------------------
  SFS_initialize();
  SFS_fork("TASK1",0,task1);
  SFS_fork("TASK2",0,task2);
  while(1)
    SFS_dispatch();
  ------------------------------
*******************************/
/* Function required before using it */
extern short SFS_initialize(void);
extern short SFS_dispatch(void);
extern short SFS_fork(char *,short,void (*)());
/* Effective function within a task */
extern void *SFS_work(void);
extern void *SFS_otherWork(char *);
extern short SFS_kill(void);
extern short SFS_change(char *,short,void (*)());

#endif
/* [EOF] */
