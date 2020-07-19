# About

This program performs multi-threaded matrix matrix multiplications using mpi.

# Usage

`mpirun -np 4 bin/mmm-mpi a-width a-height b-width b-height [logging]`

Example:

`mpirun -np 4 bin/mmm-mpi 7 10 10 7 true`

* a-width: width of matrix A
* a-height: height of matrix A
* b-width: width of matrix B
* b-height: height of matrix B
* [logging]: provide something as a fifth agument to enable logging




