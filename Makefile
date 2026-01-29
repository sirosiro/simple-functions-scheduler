COMMTOOLS=sfs.c libs/frcc/frcc.c libs/fifo/fifo.c libs/ring_buffer/ring_buffer.c libs/matrix/state_machine.c
CSRCS=tests/sample00.c tests/sample01.c tests/sample02.c tests/sample03.c tests/sample04.c tests/sample05.c tests/sample_frcc01.c tests/sample06.c 

OBJS=$(CSRCS:.c=.o) $(COMMTOOLS:.c=.o)
PROGS=$(CSRCS:.c=.exe)

# Use gcc by default, but allow overriding from environment/command line
CC ?= gcc
# Use a variable for gcov, derived from the compiler command
GCOV ?= $(subst gcc,gcov,$(CC))

# Base CFLAGS. -pg is added conditionally below.
# -fno-builtin-strncpy is added to suppress warnings about the custom strncpy.
# Added include paths for separated libraries and root (for sfs.h)
CFLAGS = -c -ansi -O -Wall -coverage -fno-builtin-strncpy -I. -Ilibs/fifo -Ilibs/frcc -Ilibs/ring_buffer -Ilibs/matrix

# Generic LDFLAGS for gcov
# Added -lpthread for sample04 and timer simulation
LDFLAGS = --coverage -lpthread
LIBS =
# Use the same command for linking by default
LD ?= $(CC)

# OS detection
UNAME_S := $(shell uname -s)


# Add -pg for gprof support on non-macOS systems
ifneq ($(UNAME_S), Darwin)
    CFLAGS += -pg
    LDFLAGS += -pg
endif

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY : all
all: $(PROGS)

$(PROGS) : $(OBJS)
	$(CC) $(@:.exe=.o) $(COMMTOOLS:.c=.o) -o $@ $(LDFLAGS)
	./$@

clean :
	@echo "Cleaning up generated files..."
	rm -f *.o *.exe *.gcda *.gcno *.gcov gmon.out *.prof *.trace
	find libs tests -type f \( -name "*.o" -o -name "*.exe" -o -name "*.gcda" -o -name "*.gcno" \) -delete
	@echo "Clean complete."

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
	gprof sample04.exe gmon.out > sample04.prof
	gprof sample05.exe gmon.out > sample05.prof
	gprof sample_frcc01.exe gmon.out > sample_frcc01.prof
	gprof sample06.exe gmon.out > sample06.prof
	@echo "Profiling complete. Results are in *.prof files."
endif
