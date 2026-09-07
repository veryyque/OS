#!/bin/bash
gcc -fPIC -c dynamiclib.c -o dynamiclib.o
gcc -dynamiclib -o libdynamic.dylib dynamiclib.o
gcc hello.c -o hello -ldl
./hello
otool -L hello
nm hello
