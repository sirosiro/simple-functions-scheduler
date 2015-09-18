# Simple Functions Scheduler

## Description of basic feature
The purpose of this project is to provide a simplified scheduling functions without the use of any standard library of the C language.

## Requirement
The SFS library are not depend to any platform and any compiler but the sample programs require the MinGW environment.  

## Usage
 make  
 make gprof  
 make gcov  

## Description of sample programs
*sample00.c:* Two functions basic scheduling. used function of the SFS are *SFS_initialze*, *SFS_fork* and *SFS_dispatch*.  

*sample01.c:* Used a common function to two functions scheduling. additional used functions of the SFS are *SFS_work* and *SFS_change*.  

*sample02.c:* Two functions unregister a scheduling list by myself and the main function monitor it. used function of the SFS are SFS_initialize, SFS_fork, SFS_dispatch and *SFS_kill*.  

*sample03.c:* The master task to control state of the slave task. used function of SFS are SFS_initialize, SFS_fork, SFS_dispatch, SFS_work, SFS_chagne, SFS_kill and *SFS_otherWork*.  

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
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

