#!/bin/bash

#g++ -Wall -O3 hsq.cpp -o hsq
#gcc -std=c99 -Wall -O3 subleq.c -o subleq

./hsq -h quick_zpu_hsq.c -b -q
./subleq quick_zpu_hsq.sq
