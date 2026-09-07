#!/bin/bash
gcc -c staticlib.c -o staticlib.o
ar rcs libstatic.a staticlib.o
gcc hello_static.c -L. -lstatic -o hello_static
nm hello_static
otool -L hello_static
./hello_static
gcc -fPIC -c dynamiclib.c -o dynamiclib.o
gcc -dynamiclib -o libdynamic.dylib dynamiclib.o
gcc hello_dynamic.c -L. -ldynamic -o hello_dynamic
nm hello_dynamic
otool -L hello_dynamic
./hello_dynamic
