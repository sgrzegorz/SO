#!/bin/bash

dir=cmake-build-debug
i=0
# 4  512 4096 8192
for size in 4  512 4096 8192
do

    if [ $size -eq 4 ]
    then
      number=7000
    fi
    if [ $size -eq 512 ]
    then
        number=600
    fi
    if [ $size -eq 4096 ]
    then
        number=1000
    fi
    if [ $size -eq 8192 ]
    then
        number=600
    fi
    echo "=TEST $i : number: $number block size: $size ============================================================"
    echo "-------------------------------------------------------------------> generate"
    ./$dir/ex1 g file.txt $number $size
    cp file.txt file1.txt

    echo "-------------------------------------------------------------------> sort"
    echo "--------------> using system functions"
    ./$dir/ex1 s file.txt $number $size sys

    echo "--------------> using library functions"
    ./$dir/ex1 s file1.txt $number $size lib

    rm file1.txt

    echo "-------------------------------------------------------------------> copy"
    echo "--------------> using system functions"
    ./$dir/ex1 c file.txt file1.txt $number $size sys

    rm file1.txt

    echo "--------------> using library functions"
    ./$dir/ex1 c file.txt file1.txt $number $size lib

    i=$[$i+1]
done