#include "frcc.h"

/* The Mini FRC is an atomically control version of a free run counter. */
unsigned char gFreeRunCounterMini;

unsigned char GetFreeRunGapMini(unsigned char vPastCount)
{
  unsigned char iNowCount;
	
  iNowCount = gFreeRunCounterMini;
  
  if(iNowCount >= vPastCount){
    return iNowCount - vPastCount;
  }else{
    return ((unsigned char)-1)-vPastCount+iNowCount;
  }
}


/* -------------------------------------------------- */
unsigned long gFreeRunCounter;

static void (*_di)(void);
static void (*_ei)(void);
void FRCInterrupt(void (*)(void),void (*)(void));

unsigned long GetFreeRunCounter(void);
unsigned long GetFreeRunGap(unsigned long,unsigned long);
void FRCGapCheckStart(FRC *,unsigned long);
unsigned long FRCGapCheck(FRC *);
void FRCGapCheckStop(FRC *vGapChk);

void FRCInterrupt(void (*di)(void),void (*ei)(void))
{
  _di = di;
  _ei = ei;
}

unsigned long GetFreeRunCounter(void)
{
  unsigned long iRet;

  (_di)();
  iRet = gFreeRunCounter;
  (_ei)();

  return iRet;
}

unsigned long GetFreeRunGap(unsigned long vFarstCount,unsigned long vSecondCount)
{
  unsigned long iRet;

  if(vSecondCount >= vFarstCount){
    iRet = vSecondCount - vFarstCount;
  }else{
    iRet = (unsigned long)-1l - vFarstCount + vSecondCount;
  }

  return iRet;
}

void FRCGapCheckStart(FRC *vGapChk,unsigned long vStopGap)
{
  vGapChk->OneShot = 1;
  vGapChk->StopGap = vStopGap;
  vGapChk->StartPoint = GetFreeRunCounter();
}

unsigned long FRCGapCheck(FRC *vGapChk)
{
  unsigned long iChk;
  unsigned long iChkFRC;

  iChkFRC = GetFreeRunCounter();
  iChk = GetFreeRunGap(vGapChk->StartPoint,iChkFRC);

  switch(vGapChk->OneShot){
  case 1:
    if(iChk >= vGapChk->StopGap){
      vGapChk->OneShot = 0;
    }
    break;
  case 0:
    vGapChk->OneShot = -1;
  default:
    break;
  }

  return iChk >= vGapChk->StopGap ? 0l:vGapChk->StopGap - iChk;
}

void FRCGapCheckStop(FRC *vGapChk)
{
  vGapChk->OneShot = -1;
  vGapChk->StopGap = 0;
}
/* [eof] */
