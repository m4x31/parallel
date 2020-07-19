# About

This program performs multi-threaded matrix matrix multiplications using openmp.
The binary was build on macos (x86_64) and should(tm) run on x86 linux.


# Usage

`./bin/mmm-openmp a-width a-height b-width b-height [logging]`

Example:

`./bin/mmm-openmp 10 10 10 10 true`

* a-width: width of matrix A
* a-height: height of matrix A
* b-width: width of matrix B
* b-height: height of matrix B
* [logging]: provide something as a fifth agument to enable logging




