/*
  frcc.c - Free Run Counter Module

  This module provides two types of free run counters:
    - Mini FRC: A simple `unsigned char` based counter.
    - Standard FRC: A more complex `unsigned long` based counter with interrupt handling and gap checking features.
*/
#include "frcc.h"

/*******************************
[ function organization - PlantUML ]

@startuml
!theme plain
skinparam packageStyle rectangle
skinparam defaultFontName Arial
skinparam defaultFontSize 9
skinparam ranksep 120
skinparam nodesep 80
skinparam packagePadding 16

title frcc.c - Free Run Counter Module

package "Mini FRC" {
  class GetFreeRunGapMini
  class gFreeRunCounterMini
}

package "Standard FRC" {
  class GetFreeRunCounter
  class GetFreeRunGap
  class FRCGapCheckStart
  class FRCGapCheck
  class FRCGapCheckStop
  class FRCInterrupt
  class gFreeRunCounter
  class FRC
  class "_di" as di_ptr
  class "_ei" as ei_ptr
}

' Mini FRC
GetFreeRunGapMini -down-> gFreeRunCounterMini : uses

' Standard FRC
FRCGapCheckStart -down-> GetFreeRunCounter : calls
FRCGapCheck -down-> GetFreeRunCounter : calls
FRCGapCheck -down-> GetFreeRunGap : calls

FRCGapCheckStart -down-> FRC : modifies
FRCGapCheck -down-> FRC : modifies
FRCGapCheckStop -down-> FRC : modifies

GetFreeRunCounter -down-> gFreeRunCounter : reads
GetFreeRunCounter -down-> di_ptr : calls
GetFreeRunCounter -down-> ei_ptr : calls

FRCInterrupt -down-> di_ptr : sets
FRCInterrupt -down-> ei_ptr : sets

@enduml
*******************************/

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
