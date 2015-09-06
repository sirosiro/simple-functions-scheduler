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
[ function  organization ]

 SFS
 +- SFS_initialize
 |
 +- SFS_dispatch
 |
 +- SFS_fork 
 |  |
 |  +- SFS_obtain
 |  |
 |  `- SFS_regist
 |
 +- SFS_kill 
 |  |
 |  +- SFS_giveup 
 |  |
 |  `- SFS_release
 |
 +- SFS_change
 |

 orher
 |
 +- strncpy 
 |  |
 |  `- strcnt
 |
 `- strcmp

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
extern short SFS_change(char *,void (*)());

#endif
/* [EOF] */
