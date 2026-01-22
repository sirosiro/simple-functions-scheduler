/************************************************
  Simple Function Scheduler
  
  $Id$
*************************************************/
/*
#include <stdio.h>
#define dbg_printf(s) printf(s);
*/
#define dbg_printf(s)   /**/

#include "sfs.h"

#define HEAD 0
#define TAIL 7
#define BODY (TAIL+1)
#define SFS_NULL ((struct SFS_tg *)0)

/*-------------------- public function --------------------*/
short SFS_initialize(void);
short SFS_dispatch(void);
short SFS_fork(char *,short,void (*)());
void *SFS_work(void);
void *SFS_otherWork(char *);
short SFS_kill(void);
short SFS_change(char *,short,void (*)());

/*-------------------- static function & variable --------------------*/
static struct SFS_tg SFS[BODY];
static struct SFS_tg *pTask;
static struct SFS_tg *pPool;
static struct SFS_tg *exe;

static void none(void){ return; }
static struct SFS_tg * SFS_obtain(void);
static void SFS_regist(struct SFS_tg *);
static void SFS_giveup(void);
static struct SFS_tg * SFS_find(char *);
/*------------------------------*/
static char *strncpy(char *,char *,unsigned int);
static int strcnt(char *);
static int strcmp(char *,char *);

/*-------------------- public function define --------------------*/
short SFS_initialize(void)
{
  short iLoop = 1;

  SFS[HEAD].name[0] = 0x00;
  SFS[HEAD].order = 0x0000;
  SFS[HEAD].pFront = SFS_NULL;
  SFS[HEAD].pBack = &SFS[iLoop];
  SFS[HEAD].pFunction = none;
  while(iLoop<TAIL){
    SFS[iLoop].name[0] = 0x00;
    SFS[iLoop].order = 0x0000;
    SFS[iLoop].pFront = &SFS[iLoop-1];
    SFS[iLoop].pBack = &SFS[iLoop+1];
    SFS[iLoop].pFunction = none;
    iLoop++;
  }
  SFS[iLoop].name[0] = 0x00;
  SFS[iLoop].order = 0x0000;
  SFS[iLoop].pFront = &SFS[iLoop-1];
  SFS[iLoop].pBack = SFS_NULL;
  SFS[iLoop].pFunction = none;

  pPool = &SFS[HEAD];
  pTask = SFS_NULL;

  return 0;
}

short SFS_dispatch(void)
{
  short tcnt=0;
  static struct SFS_tg * next;
  
  exe = pTask;
  while(exe!=SFS_NULL){
    next = exe->pBack;
    (*exe->pFunction)();
    exe = next;
    tcnt++;
  }
  exe = SFS_NULL;
  
  return tcnt;
}

short SFS_fork(char *name,short order,void (*func)())
{
  struct SFS_tg * sfs;
  short retf = 0;
  
  sfs = SFS_obtain();

  if(sfs!=SFS_NULL){
    strncpy(sfs->name,name,SFS_NAME_SIZE);
    sfs->order = order;
    sfs->pFunction = func;
    SFS_regist(sfs);
    retf = -1;
  }
dbg_printf(name);
dbg_printf(" fork !\n");
  return retf;
}

void *SFS_work(void)
{
  return (void *)exe->work;
}

void *SFS_otherWork(char *name)
{
  struct SFS_tg * sfs;
  
  sfs = SFS_find(name);

  if(sfs==SFS_NULL)
    return (void *)0;

  return (void *)sfs->work;
}

short SFS_kill(void)
{
  if(exe!=SFS_NULL){
    exe->pFunction = SFS_giveup;
  }
dbg_printf("kill !\n");
  return 0;
}

short SFS_change(char *name,short order,void (*func)())
{
  if(exe!=SFS_NULL){
    strncpy(exe->name,name,SFS_NAME_SIZE);
    exe->order = order;
    exe->pFunction = func;
  }
dbg_printf("change !!\n");
  return 0;
}

/*-------------------- static functions --------------------*/
static struct SFS_tg * SFS_obtain(void)
{
  struct SFS_tg * sfs;
  
  if(pPool==SFS_NULL)
    return SFS_NULL;

  sfs = pPool;
  pPool = pPool->pBack;

  return sfs;
}

static void SFS_regist(struct SFS_tg *sfs)
{
  struct SFS_tg * entry;
  short order = sfs->order;
/*  short flag = 1; */
  
  if(pTask==SFS_NULL){
    pTask = sfs;
    sfs->pBack = SFS_NULL;
  }else{
    entry = pTask;
    while(entry->pBack!=SFS_NULL){
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
      if(entry->pFront == SFS_NULL){  /* inserts in the head. */
        pTask = sfs;
        sfs->pFront = SFS_NULL;
        sfs->pBack = entry;
        entry->pFront = sfs;
      }else{                          /* inserts in the entry point front of a main part. */
        sfs->pFront = entry->pFront;
        sfs->pBack = entry;
        entry->pFront = sfs;
      }
    }else{                            /* to back.. */
      if(entry->pBack == SFS_NULL){   /* inserts in a tail. */
        entry->pBack = sfs;
        sfs->pBack = SFS_NULL;
      }else{                          /* inserts behind the entry point of a main part. */
        sfs->pBack = entry->pBack;
        sfs->pFront = entry;
        entry->pBack = sfs;
      }
    }
  }
}

static void SFS_release(struct SFS_tg *sfs)
{
  struct SFS_tg * entry;

  if(pPool==SFS_NULL){
    pPool = sfs;
    sfs->pFront = SFS_NULL;
    sfs->pBack = SFS_NULL;
  }else{
    entry = pPool;
    while(entry->pBack!=SFS_NULL){
      entry = entry->pBack;
    }
    entry->pBack = sfs;
    sfs->pBack = SFS_NULL;
  }
}

static void SFS_giveup(void)
{
  struct SFS_tg *front_sfs = exe->pFront;
  struct SFS_tg *back_sfs = exe->pBack;

  if(front_sfs==SFS_NULL){
    if(back_sfs==SFS_NULL){
      pTask = SFS_NULL;
    }else{
      back_sfs->pFront = SFS_NULL;
      pTask = back_sfs;
    }
    SFS_release(exe);
  }else if(back_sfs==SFS_NULL){
    front_sfs->pBack = SFS_NULL;
    SFS_release(exe);
  }else{
    front_sfs->pBack = back_sfs;
    back_sfs->pFront = front_sfs;
    SFS_release(exe);
  }
dbg_printf("give up !\n");
}

static struct SFS_tg * SFS_find(char *name)
{
  struct SFS_tg * sfs;
  
  if(pTask==SFS_NULL)
    return SFS_NULL;
  
  sfs = pTask;
  
  while(sfs!=SFS_NULL){
    if(!strcmp(sfs->name,name))
      break;
    sfs = sfs->pBack;
  }
  
  return sfs;
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
