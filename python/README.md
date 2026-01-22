
# Simple Functions Scheduler (Python Port)

This project is a Python port of the `simple-functions-scheduler` originally implemented in C. It aims to replicate the core functionality and design principles outlined in the `ARCHAEOLOGY_REPORT.md`, `ARCHITECTURE_MANIFEST.md`, and `DESIGN_PHILOSOPHY.md` documents.

## Project Structure

*   `sfs.py`: Implements the Simple Function Scheduler (SFS) module, providing cooperative multitasking for a fixed number of tasks.
*   `frcc.py`: Implements the Free Run Clock Counter (FRCC) module, simulating a hardware-based free-running timer with rollover handling and gap checking.
*   `sample.py`: A demonstration and basic test case showing how to use the SFS and FRCC modules together.

## Features

### SFS (Simple Function Scheduler)

*   **Cooperative Multitasking:** Tasks yield control explicitly.
*   **Fixed Task Count:** Supports a predefined maximum number of tasks (`BODY`).
*   **Priority-based Dispatch:** Tasks are executed based on their assigned `order` (priority).
*   **Work Buffers:** Each task has a dedicated `work` buffer for local data.
*   **Task Management:** APIs for initializing, forking (creating), killing (terminating), and changing tasks.

### FRCC (Free Run Clock Counter)

*   **Simulated Free-Running Counter:** Provides 8-bit and 32-bit counter simulations.
*   **Rollover Handling:** Accurately calculates time differences even when the counter wraps around.
*   **Interrupt-Safe Access Simulation:** Includes dummy functions for interrupt disable/enable to maintain API compatibility with the original C design.
*   **Gap Checking:** Mechanism to check for elapsed time intervals (one-shot or periodic).

## How to Run the Sample

1.  **Ensure Python 3 is installed.**
2.  **Navigate to the project directory:**
    ```bash
    cd /Users/sirosiro/project/c/simple-functions-scheduler/python
    ```
3.  **Run the sample script:**
    ```bash
    python3 sample.py
    ```

The `sample.py` script will initialize the scheduler, fork a few demonstration tasks with different priorities, and use the FRCC module to simulate time-based events. You will see output from the tasks and timer events printed to the console.

## Design Philosophy & Archaeology

This Python port was guided by the principles and architectural decisions documented in:

*   `DESIGN_PHILOSOPHY.md`: Outlines the overall approach to AI-assisted development and core project values.
*   `ARCHAEOLOGY_REPORT.md`: Details the reverse-engineered design of the original C codebase.
*   `ARCHITECTURE_MANIFEST.md`: Serves as the project's architectural constitution, specifying component designs based on the archaeological findings.
