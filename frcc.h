#ifndef __FRCC_INC__
#define __FRCC_INC__

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

