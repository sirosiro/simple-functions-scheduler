/************************************************
  Simple Function Scheduler
  
  $Id$
*************************************************/
/*
#include <stdio.h>
#define dbg_printf(s) printf(s);
*/
#define dbg_printf(s)   /**/

#include "tcb.h"

#define HEAD 0
#define TAIL 7
#define BODY (TAIL+1)
#define TCB_NULL ((struct TCB_tg *)0)

/*-------------------- public function --------------------*/
short TCB_initialize(void);
short TCB_dispatch(void);
short TCB_fork(char *,short,void (*)());
void *TCB_work(void);
void *TCB_otherWork(char *);
short TCB_kill(void);
short TCB_change(char *,void (*)());

/*-------------------- static function & variable --------------------*/
static struct TCB_tg TCB[BODY];
static struct TCB_tg *pTask;
static struct TCB_tg *pPool;
static struct TCB_tg *exe;

static void none(void){ return; }
static struct TCB_tg * TCB_obtain(void);
static void TCB_regist(struct TCB_tg *);
static void TCB_giveup(void);
static struct TCB_tg * TCB_find(char *);
/*------------------------------*/
static char *strncpy(char *,char *,unsigned int);
static int strcnt(char *);
static int strcmp(char *,char *);

/*-------------------- public function define --------------------*/
short TCB_initialize(void)
{
  short iLoop = 1;

  TCB[HEAD].name[0] = 0x00;
  TCB[HEAD].order = 0x0000;
  TCB[HEAD].pFront = TCB_NULL;
  TCB[HEAD].pBack = &TCB[iLoop];
  TCB[HEAD].pFunction = none;
  while(iLoop<TAIL){
    TCB[iLoop].name[0] = 0x00;
    TCB[iLoop].order = 0x0000;
    TCB[iLoop].pFront = &TCB[iLoop-1];
    TCB[iLoop].pBack = &TCB[iLoop+1];
    TCB[iLoop].pFunction = none;
    iLoop++;
  }
  TCB[iLoop].name[0] = 0x00;
  TCB[iLoop].order = 0x0000;
  TCB[iLoop].pFront = &TCB[iLoop-1];
  TCB[iLoop].pBack = TCB_NULL;
  TCB[iLoop].pFunction = none;

  pPool = &TCB[HEAD];
  pTask = TCB_NULL;

  return 0;
}

short TCB_dispatch(void)
{
  short tcnt=0;
  static struct TCB_tg * next;
  
  exe = pTask;
  while(exe!=TCB_NULL){
    next = exe->pBack;
    (*exe->pFunction)();
    exe = next;
    tcnt++;
  }
  exe = TCB_NULL;
  
  return tcnt;
}

short TCB_fork(char *name,short order,void (*func)())
{
  struct TCB_tg * tcb;
  short retf = 0;
  
  tcb = TCB_obtain();

  if(tcb!=TCB_NULL){
    strncpy(tcb->name,name,TCB_NAME_SIZE);
    tcb->order = order;
    tcb->pFunction = func;
    TCB_regist(tcb);
    retf = -1;
  }
dbg_printf(name);
dbg_printf(" fork !\n");
  return retf;
}

void *TCB_work(void)
{
  return (void *)exe->work;
}

void *TCB_otherWork(char *name)
{
  struct TCB_tg * tcb;
  
  tcb = TCB_find(name);

  if(tcb==TCB_NULL)
    return (void *)0;

  return (void *)tcb->work;
}

short TCB_kill(void)
{
  if(exe!=TCB_NULL){
    exe->pFunction = TCB_giveup;
  }
dbg_printf("kill !\n");
  return 0;
}

short TCB_change(char *name,void (*func)())
{
  if(exe!=TCB_NULL){
    strncpy(exe->name,name,TCB_NAME_SIZE);
    exe->pFunction = func;
  }
dbg_printf("change !!\n");
  return 0;
}

/*-------------------- function static function --------------------*/
static struct TCB_tg * TCB_obtain(void)
{
  struct TCB_tg * tcb;
  
  if(pPool==TCB_NULL)
    return TCB_NULL;

  tcb = pPool;
  pPool = pPool->pBack;

  return tcb;
}

static void TCB_regist(struct TCB_tg *tcb)
{
  struct TCB_tg * entry;
  short order = tcb->order;
/*  short flag = 1; */
  
  if(pTask==TCB_NULL){
    pTask = tcb;
    tcb->pBack = TCB_NULL;
  }else{
    entry = pTask;
    while(entry->pBack!=TCB_NULL){
      if(order < entry->order){
        break;
      }else if(order >= entry->order){
        if(order < entry->pBack->order){
          break;
        }
      }
      entry = entry->pBack;
    }
    if(order < entry->order){         /* befor.. */
      if(entry->pFront == TCB_NULL){  /* inserts in the head. */
        pTask = tcb;
        tcb->pFront = TCB_NULL;
        tcb->pBack = entry;
        entry->pFront = tcb;
      }else{                          /* inserts in the entry point front of a main part. */
        tcb->pFront = entry->pFront;
        tcb->pBack = entry;
        entry->pFront = tcb;
      }
    }else{                            /* to back.. */
      if(entry->pBack == TCB_NULL){   /* inserts in a tail. */
        entry->pBack = tcb;
        tcb->pBack = TCB_NULL;
      }else{                          /* inserts behind the entry point of a main part. */
        tcb->pBack = entry->pBack;
        tcb->pFront = entry;
        entry->pBack = tcb;
      }
    }
  }
}

static void TCB_release(struct TCB_tg *tcb)
{
  struct TCB_tg * entry;

  if(pPool==TCB_NULL){
    pPool = tcb;
    tcb->pFront = TCB_NULL;
    tcb->pBack = TCB_NULL;
  }else{
    entry = pPool;
    while(entry->pBack!=TCB_NULL){
      entry = entry->pBack;
    }
    entry->pBack = tcb;
    tcb->pBack = TCB_NULL;
  }
}

static void TCB_giveup(void)
{
  struct TCB_tg *front_tcb = exe->pFront;
  struct TCB_tg *back_tcb = exe->pBack;

  if(front_tcb==TCB_NULL){
    if(back_tcb==TCB_NULL){
      pTask = TCB_NULL;
    }else{
      back_tcb->pFront = TCB_NULL;
      pTask = back_tcb;
    }
    TCB_release(exe);
  }else if(back_tcb==TCB_NULL){
    front_tcb->pBack = TCB_NULL;
    TCB_release(exe);
  }else{
    front_tcb->pBack = back_tcb;
    back_tcb->pFront = front_tcb;
    TCB_release(exe);
  }
dbg_printf("give up !\n");
}

static struct TCB_tg * TCB_find(char *name)
{
  struct TCB_tg * tcb;
  
  if(pTask==TCB_NULL)
    return TCB_NULL;
  
  tcb = pTask;
  
  while(tcb!=TCB_NULL){
    if(!strcmp(tcb->name,name))
      break;
    tcb = tcb->pBack;
  }
  
  return tcb;
}

/*------------------------------*/

static char *strncpy(char *s1,char *s2,unsigned int n)
{
  char *_s1=s1;
  unsigned int _n;

  if( s1 < s2 ){
    while( n-- && *s2 )
      *s1++ = *s2++;
    *s1 = 0;
  }else{
    _n = strcnt(s2);
    n = _n < n ? _n:n;
    s2 += n;
    s1 += n;
    *(s1) = 0;
    while( n-- )
      *--s1 = *--s2;
  }
  return _s1;
}

static int strcnt(char *s)
{
  int cnt;

  for(cnt=0;*(s+cnt);cnt++);

  return cnt;
}

static int strcmp(char *s1,char *s2)
{
  int retf;
  
  while( *s1 && *s1 == *s2 ){
    s1++;
    s2++;
  }
  
  retf = *s1 - *s2;
  
  return retf;
}

/* ----[EOF]---- */
