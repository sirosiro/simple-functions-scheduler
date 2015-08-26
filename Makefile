COMMTOOLS=tcb.c
CSRCS=test.c

OBJS=$(CSRCS:.c=.o) $(COMMTOOLS:.c=.o)
PROGS=$(CSRCS:.c=.exe)

CC = gcc
CFLAGS = -c -ansi -O -Wall
LDFLAGS =
LIBS =
LD = gcc

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY : all
all: $(PROGS)

$(PROGS) : $(OBJS)
	$(LD) $(@:.exe=.o) $(COMMTOOLS:.c=.o) -o $@ $(LDFLAGS)

clean :
	rm -rf $(OBJS) $(PROGS)

