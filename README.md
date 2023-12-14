# Systems Programming Projects
## Overview
This repository contains my attempts to the projects for the Systems Programming course at Emory University. The course focuses on the multiple interfaces and some basic mechanics of major operating system components and teaches efficient system services access through system calls or layered libraries. This course is beneficial for those interested in general software development, operating systems, embedded systems, or distributed systems.
## Objectives
- Understand how the OS uses system calls to abstract hardware resources.
- Differentiate between user and system-level services, including performance implications.
- Apply low-level system and library calls effectively.
- Understand and implement multi-threaded programs.
- Develop robust C programs in a Unix environment.
- Use debuggers for diagnosing program problems.
- Perform basic program performance measurement and analysis.

## Projects Overview
### Project 1: PPM Image Transformations
#### Objectives
- Develop, compile, run, and test C programs in a Linux environment.
- Navigate Linux command lines reliably.
#### Specifications
- Name: ppmcvt
- Description: A utility to manipulate PPM images with various transformations like grayscale, sepia, mirroring, etc.
- Examples:
  - `ppmcvt -o out.pbm in.ppm` - Convert PPM to PBM.
  - `ppmcvt -s -o out.ppm in.ppm` - Apply sepia transformation.
- Implementation: Utilizes ppmcvt.c and pbm_aux.c with the provided PBM library for image processing.

---

### Project 2: My Malloc Library
#### Objectives
- Manipulate C pointers to traverse a process’ address space.
- Use pointer arithmetic and type casting for memory management.

#### Specifications
- Name: my_malloc Library
- Description: A custom memory allocation library in C, handling memory allocation, deallocation, and optimization.
- Examples:
  - `my_malloc(size)` - Allocate size bytes of memory.
  - `my_free(ptr)` - Deallocate memory referenced by ptr.
- Implementation: Involves functions like `my_malloc()`, `my_free()`, `coalesce_free_list()`, and `free_list_begin()`, using `sbrk()` for extending the heap segment. Manages a free list for efficient memory allocation.

---

### Project 3: Tar File Manipulation
#### Objectives
- Read and understand directory entries and file attributes.
- Perform complex file input/output and file manipulation operations.

#### Specifications
- Name: mytar
- Description: Utility for creating, extracting, or printing details of tape archives (tar).
- Examples:
  - `mytar -c -f a.tar a` - Create an archive from a directory.
  - `mytar -x -f a.tar` - Extract files from an archive.
- Implementation: Handles file modes, modification times, directory structures, and hard links. Implements functionalities for creating and managing tar files, including file I/O and directory traversal.
