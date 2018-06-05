#!/bin/bash

dir=bin
i=0
# 4  512 4096 8192
rm results.txt
touch results.txt
for size in 1 2 4 8
do
    echo "-------------------------------------------------------------------" >> results.txt
    echo "=TEST $i : threads: $size" >> results.txt

    ./bin/zad $size ./dog.pgm ./edge_detection.txt ./newdog.pgm   >> results.txt

    i=$[$i+1]
done
