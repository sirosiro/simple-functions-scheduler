#ifndef __FRCC_INC__
#define __FRCC_INC__

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
extern unsigned char gFreeRunCounterMini;
extern unsigned char GetFreeRunGapMini(unsigned char);


/* -------------------------------------------------- */

extern unsigned long gFreeRunCounter;

typedef struct FRCGapChk_tg {
  char OneShot;
  unsigned long StartPoint;
  unsigned long StopGap;
}FRC;
extern void FRCInterrupt(void (*)(void),void (*)(void));
extern unsigned long GetFreeRunCounter(void);
extern unsigned long GetFreeRunGap(unsigned long ,unsigned long );
extern void FRCGapCheckStart(FRC *,unsigned long );
extern unsigned long FRCGapCheck(FRC *);
extern void FRCGapCheckStop(FRC *);

#endif

