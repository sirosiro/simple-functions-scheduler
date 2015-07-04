/************************************************
  Task Controler
  
  $Id:$
*************************************************/
#ifndef __TCB_INC__
#define __TCB_INC__

#define TCB_NAME_SIZE 16
#define TCB_WORK_SIZE 32
/* Task Control Block */
struct TCB_tg {
  char name[TCB_NAME_SIZE];
  unsigned short order;
  struct TCB_tg *pFront;
  struct TCB_tg *pBack;
  /* ---------- */
  void (*pFunction)(void);
  char work[TCB_WORK_SIZE];
};
/*******************************
[ function  organization ]

 TCB
 +- TCB_initialize
 |
 +- TCB_dispatch
 |
 +- TCB_fork 
 |  |
 |  +- TCB_obtain
 |  |
 |  `- TCB_regist
 |
 +- TCB_kill 
 |  |
 |  +- TCB_giveup 
 |  |
 |  `- TCB_release
 |
 +- TCB_change
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
  TCB_initialize();
  TCB_fork("TASK1",0,task1);
  TCB_fork("TASK2",0,task2);
  while(1)
    TCB_dispatch();
  ------------------------------
*******************************/
/* Function required before using it */
extern short TCB_initialize(void);
extern short TCB_dispatch(void);
extern short TCB_fork(char *,short,void (*)());
/* Effective function within a task */
extern void *TCB_work(void);
extern void *TCB_otherWork(char *);
extern short TCB_kill(void);
extern short TCB_change(char *,void (*)());

#endif
/* [EOF] */
