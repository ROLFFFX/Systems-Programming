# Systems-Programming

1 - [Intro]  <br/>
2 - [Memory](#organization-of-memory)  <br/>
3 - [Address Space]  <br/>
  1. [OSTEP Ch 13: Address Space]
4 - [Virtualizing Memory]  <br/>  
  1. [OSTEP Ch 14: Memory API and Malloc Implementation]
5 - [Malloc]  <br/>  
  1. [OSTEP Ch 17: Free Implementation]
6 - [Malloc and Free]  <br/>  
  [OSTEP Ch 39, S 1-5: File I/O Basics] 
  [OSTEP Ch 39, S 7-16, 18]
7 - [File I/O]  <br/>  
8 - [Processes]  <br/>  
  [OSTEP Ch 4 S 1,2,6: Process Abstraction]
9 - [Threads]  <br/>  
  [OSTEP Ch 5m S 1-7: Creating and Executing Processes]

#Memory

## Organization of Memory
	Code/Text: program instructions
	Data: Global Static Vars
	Heap: Dynamic data storage
	Stack: Function invocation variables

## An array is a consecutive chunk of memory
char a[5];	in 2-D arrays, the size of the first dimension can be omitted but the size of the second and any subsequent dimensions must be specified. Need to know the size of rows
int x[][3]	valid, second dimension is provided. 
int x[2][]	invalid
int *x[]	valid. array of pointers to integers. 

# OSTEP Ch13 - Address Space
Multiprogramming: multiple processes were ready to run, OS would switch between them.
cons: timely response from currently executing tasks for concurrent users
Time sharing: run one process for a short while, giving it full access to all memory, then stop, save state to disk, load another process’s state, run it for a while, then switch.
cons: way too slow as memory grows. Restoring register level state (PC) is fast, saving content of memory to disk is slow.
solution: leave processes in memory while switching between them
Time sharing: protection problem. Multiple programs residing concurrently in memory makes a process able to read, write other processes' memory.

Address Space (Virtualizing Memory)
An abstraction of physical memory created by the OS. It is the running program’s view of memory in the system.
The address space of a process contains all of the memory state of the running program.
Code: instructions (static, at top)
Stack: keep track of where the program is in the function call chain as well as to allocate local variables and pass parameters and return values to and from routines. (grows upwards, from end of code)
Heap: dynamically-allocated, user-managed memory (grows downwards, from bottom).

The running program thinks it is loaded into memory at a particular address, and has potentially very large address space. OS translates virtual address to the actual address safely.


