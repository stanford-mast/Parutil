Description will go here.

Paratool is implemented using a combination of C and assembly.


# Requirements

To build and link with Paratool, the following are required.

- 64-bit x86-compatible processor with support for AVX2 instructions
  
  Paratool has been tested with Intel processors of the Haswell generation and newer.
  Non-Intel processors may also be compatible if they support the required instructions.
  Do not attempt to use it on older-generation processors, at the risk of encountering "Illegal Instruction" errors.
  
- Windows 10 Pro or Ubuntu 14.04
  
  64-bit versions of these operating systems are required.
  Other Linux distributions are likely compatible, but they have not been tested.

- [**Spindle**](https://github.com/stanford-mast/Spindle) and its dependencies
  
  Spindle provides the multi-threading support that Paratool uses internally.

- [**Silo**](https://github.com/stanford-mast/Silo) and its dependencies
  
  Silo provides NUMA awareness functionality that Paratool uses internally.


# Building

On all platforms, Paratool compiles to a static library.

The Windows build system is based on Visual Studio 2015 Community Edition. Compilation is known to work from the graphical interface, but command-line build is also likely possible.

To build on Linux, just type `make` from within the repository directory.


# Linking and Using

Projects that make use of Paratool should include the top-level paratool.h header file and nothing else.

Assuming a Linux-based C-language project that uses Paratool and consists of a single source file called "main.c", the following command would build and link with Paratool.

    gcc main.c -mno-vzeroupper -pthread -lparatool -lsilo -lspindle -ltopo -lhwloc -lnuma -lpciaccess -lxml2


# Getting Started

Documentation is available and can be built using Doxygen.
It covers both the external API in paratool.h and Paratool's internals, the latter being of interest only to those who wish to modify the implementation of Paratool.

On Linux, type `make docs` to compile the documentation. On Windows, run the Doxygen tool using the repository directory as the working directory (it may be necessary to create the output directory manually first).


# Copyright

Paratool is licensed under BSD 3-clause (see "LICENSE" in the top-level source code directory).

Copyright (c) 2016-2017 Stanford University, Department of Electrical Engineering.
Authored by Samuel Grossman.
