# Simple Functions Scheduler (SFS)

## Description

A lightweight, cooperative task scheduler and free-run counter library for C. It is specifically designed for embedded systems and other environments where the C standard library is not available or desirable.

## Key Features

*   **Zero Dependencies**: Written in C89 (`-ansi`) and does not depend on any standard libraries.
*   **Deterministic Memory Usage**: Uses a static memory pool for task management, avoiding `malloc`/`free` for predictable and stable long-term operation.
*   **Portable**: Hardware-dependent operations (like interrupt control) are injectable via function pointers, making the core logic highly portable.
*   **Cooperative Scheduling**: A simple, non-preemptive scheduler manages tasks based on a priority order.

## Components

This library consists of four main components:

*   **SFS (Simple Functions Scheduler)**: The core scheduler. It manages the lifecycle of tasks (creation, dispatching, and termination).
*   **FRCC (Free Run Counter)**: A utility for timekeeping. It provides counter functionalities with overflow handling and support for atomic access, which is crucial for timer interrupts.
*   **FIFO (First-In, First-Out)**: A general-purpose FIFO queue with a fixed element size, designed for inter-task communication and event queuing.
*   **Ring Buffer**: A flexible byte-stream ring buffer for handling continuous data streams, supporting custom read/write functions for hardware optimization (e.g., DMA).

## Requirements

*   A C89-compliant C compiler (e.g., `gcc`).
*   `make` for building the project.
*   The `pthreads` library is required to build and run the sample programs, as they use it to simulate timer interrupts.

## How to Build

Build the library and all sample programs by running `make`:

```bash
make
```

This command will compile and automatically run each sample program.

To clean up build artifacts, run:

```bash
make clean
```

The Makefile also includes targets for profiling (`make gprof`) and coverage analysis (`make gcov`).

**Note on macOS Profiling:**
On macOS, `make gprof` uses the `xctrace` utility (part of Xcode) instead of `gprof`. It generates a `.trace` file that can be opened with Instruments.

## Sample Programs

The sample programs demonstrate the key functionalities of the library.

*   **sample00.c:** Demonstrates basic scheduling of two independent tasks using `SFS_initialize`, `SFS_fork`, and `SFS_dispatch`.
*   **sample01.c:** Shows inter-task communication using the `SFS_work` and `SFS_change` functions.
*   **sample02.c:** A task terminates itself from the schedule using `SFS_kill`.
*   **sample03.c:** A "master" task controls the state of a "slave" task using `SFS_otherWork`.
*   **sample04.c:** Demonstrates using the FIFO library for safe inter-task communication between a producer and a consumer.
*   **sample05.c:** Verifies the Ring Buffer library functionalities, including basic read/write, overwrite mode, and dependency injection for custom data copy functions.
*   **sample_frcc01.c:** Demonstrates using the FRCC module for time-based task control. It uses the `gFreeRunCounterMini` variable as a time source and the `GetFreeRunGapMini` function to measure elapsed time.

## Future Plans

*   Event control function.

## License

The MIT License (MIT)  
Copyright (c) 2015 Kouichi Shiroma.  

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:  

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.  

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.