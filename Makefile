COMMTOOLS=sfs.c frcc.c ring_buffer.c
CSRCS=sample00.c sample01.c sample02.c sample03.c sample_frcc01.c

OBJS=$(CSRCS:.c=.o) $(COMMTOOLS:.c=.o)
PROGS=$(CSRCS:.c=.exe)

# Use gcc by default, but allow overriding from environment/command line
CC ?= gcc
# Use a variable for gcov, derived from the compiler command
GCOV ?= $(subst gcc,gcov,$(CC))

# Base CFLAGS. -pg is added conditionally below.
# -fno-builtin-strncpy is added to suppress warnings about the custom strncpy.
CFLAGS = -c -ansi -O -Wall -coverage -fno-builtin-strncpy

# Generic LDFLAGS for gcov
LDFLAGS = --coverage
LIBS =
# Use the same command for linking by default
LD ?= $(CC)

# OS detection
UNAME_S := $(shell uname -s)


# Add -pg for gprof support on non-macOS systems
ifneq ($(UNAME_S), Darwin)
    CFLAGS += -pg
endif

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY : all
all: $(PROGS)

$(PROGS) : $(OBJS)
	$(CC) $(@:.exe=.o) $(COMMTOOLS:.c=.o) -o $@ $(LDFLAGS)
	./$@

clean :
	rm -rf $(OBJS) $(PROGS)
	rm -rf *.g* *.out *.prof *.trace

gcov:
	$(GCOV) *.gcda

.PHONY: gprof gcov
# Conditional gprof target
ifeq ($(UNAME_S), Darwin)
# On macOS, use xctrace. Note: This profiles only sample00.exe as an example.
gprof:
	@echo "Profiling sample00.exe with xctrace on macOS..."
	@echo "To profile a different sample, run the command manually."
	xcrun xctrace record --template 'Time Profiler' --output sample00.trace --launch -- ./sample00.exe
	@echo "---"
	@echo "Profiling complete."
	@echo "To view results, open the .trace file with Instruments: open sample00.trace"
else
# On Linux and other systems, use gprof.
gprof:
	gprof sample00.exe gmon.out > sample00.prof
	gprof sample01.exe gmon.out > sample01.prof
	gprof sample02.exe gmon.out > sample02.prof
	gprof sample03.exe gmon.out > sample03.prof
	gprof sample_frcc01.exe gmon.out > sample_frcc01.prof
	@echo "Profiling complete. Results are in *.prof files."
endif