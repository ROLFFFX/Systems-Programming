# Systems-Programming

1 - [Intro]  <br/>
2 - [Memory]  <br/>
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
int *x[]		valid. array of pointers to integers. 
