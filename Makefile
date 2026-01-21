COMMTOOLS=sfs.c frcc.c fifo.c
CSRCS=sample00.c sample01.c sample02.c sample03.c sample_frcc01.c sample04.c

OBJS=$(CSRCS:.c=.o) $(COMMTOOLS:.c=.o)
PROGS=$(CSRCS:.c=.exe)

CC = gcc
CFLAGS = -c -ansi -O -Wall --coverage -pg
LDFLAGS = --coverage -pg -lpthread
LIBS =
LD = gcc

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY : all
all: $(PROGS)

$(PROGS) : $(OBJS)
	$(LD) $(@:.exe=.o) $(COMMTOOLS:.c=.o) -o $@ $(LDFLAGS)
	./$@

clean :
	rm -rf $(OBJS) $(PROGS)
	rm -rf *.g* *.out *.prof

gcov :
	gcov *.gcda

gprof :
	gprof sample00.exe gmon.out > sample00.prof
	gprof sample01.exe gmon.out > sample01.prof
	gprof sample02.exe gmon.out > sample02.prof
	gprof sample03.exe gmon.out > sample03.prof
	gprof sample_frcc01.exe gmon.out > sample_frcc01.prof
	gprof sample04.exe gmon.out > sample04.prof
