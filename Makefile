COMMTOOLS=sfs.c
CSRCS=sample00.c sample01.c sample02.c

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

