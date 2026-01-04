# Simple Functions Scheduler

## Description of basic feature
The purpose of this project is to provide a simplified scheduling functions without the use of any standard library of the C language.

## Requirement
The SFS library does not depend on any platform. The sample programs require a `gcc` compiler environment, such as MinGW on Windows.

On macOS, using the profiling feature (`make gprof`) requires the full **Xcode** application to be installed from the App Store, as it depends on the `xctrace` utility.

## Usage
The `Makefile` will use `gcc` as the default compiler. If you need to use a specific version or a different C compiler, you can specify it via the `CC` variable.

*   **`make`**  
    Builds all sample programs.
    ```sh
    # Use the default gcc compiler
    make
    
    # Use a specific version of gcc
    make CC=gcc-15
    ```

*   **`make gcov`**  
    Generates code coverage reports (`.gcov` files) for all source files.
    ```sh
    make gcov
    
    # Ensure gcov version matches the compiler
    make gcov CC=gcc-15
    ```

*   **`make gprof`**  
    Generates profiling output.
    *   On Linux, this creates `.prof` files using `gprof`.
    *   On macOS, this creates a `sample00.trace` file using `xctrace`, which can be opened with the Instruments application.
    ```sh
    make gprof
    
    make gprof CC=gcc-15
    ```

*   **`make clean`**  
    Removes all built executables, object files, and report files (`.g*`, `.out`, `.prof`, `.trace`).
    ```sh
    make clean
    ```

## Description of sample programs
**sample00.c:** Two functions basic scheduling. used function of the SFS are **SFS_initialze**, **SFS_fork** and **SFS_dispatch**.  

**sample01.c:** Used a common function to two functions scheduling. additional used functions of the SFS are **SFS_work** and **SFS_change**.  

**sample02.c:** Two functions unregister a scheduling list by myself and the main function monitors it. used function of the SFS are SFS_initialize, SFS_fork, SFS_dispatch and **SFS_kill**.  

**sample03.c:** The master task to control state of the slave task. used function of SFS are SFS_initialize, SFS_fork, SFS_dispatch, SFS_work, SFS_chagne, SFS_kill and **SFS_otherWork**.  

**sample_frcc01.c** Two functions are get clock from a minimum free run counter and control execution time myself. used function of the FRCC are the **GetFreeRunCounterMini**.  


## In Future
FIFO control function.  
Event control function.  
Ring buffer control function.  

## Licence

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

