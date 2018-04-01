#!/bin/bash

echo "------------------------------------------------------------------------------------------------------------"
echo "TEST 1A: create array with static memory allocation, add 1000 blocks"
echo
echo "-----> Test program with static library:" 

./build/pstatic c 1000 100 1 a 900 r 400 
echo
echo "-----> Test program with shared library:" 

./build/pshared c 1000 100 1 a 900 r 400 

echo
echo "-----> Test program with dynamically loaded library:"

./build/pdynamic c 1000 100 1 a 900 r 400

echo 

echo "------------------------------------------------------------------------------------------------------------"
echo "TEST 1B: create array with dynamic memory allocation, add 1000 blocks"
echo
echo "-----> Test program with static library:" 

./build/pstatic c 1000 100 0 a 900 r 400
echo
echo "-----> Test program with shared library:" 

./build/pshared c 1000 100 0 a 900 r 400

echo
echo "-----> Test program with dynamically loaded library:"

./build/pdynamic c 1000 100 0 a 900 r 400

echo 

echo "------------------------------------------------------------------------------------------------------------"
echo "TEST 2A: create array with static memory allocation, add few blocks and find ID of block for which sum of characters is closest to given value"
echo
echo "-----> Test program with static library:" 

./build/pstatic c 566 300 1 a 400 s 900 
echo
echo "-----> Test program with shared library:" 

./build/pshared c 566 300 1 a 400 s 900 

echo
echo "-----> Test program with dynamically loaded library:"

./build/pdynamic c 566 300 1 a 400 s 900 

echo 

echo "------------------------------------------------------------------------------------------------------------"
echo "TEST 2B: create array with dynamic memory allocation, find ID of block for which sum of characters is closest to given value"
echo
echo "-----> Test program with static library:" 

./build/pstatic c 566 300 0 a 400 s 900 
echo
echo "-----> Test program with shared library:" 

./build/pshared c 566 300 0 a 400 s 900 

echo
echo "-----> Test program with dynamically loaded library:"

./build/pdynamic c 566 300 0 a 400 s 900 

echo 

echo "------------------------------------------------------------------------------------------------------------"
echo "TEST 3A: create array with static memory allocation, adding and deleting blocks"
echo
echo "-----> Test program with static library:" 

./build/pstatic c 566 300 1 ar 300
echo
echo "-----> Test program with shared library:" 

./build/pshared c 566 300 1 ar 300

echo
echo "-----> Test program with dynamically loaded library:"

./build/pdynamic c 566 300 1 ar 300

echo 

echo "------------------------------------------------------------------------------------------------------------"
echo "TEST 3B: create array with dynamic memory allocation, adding and deleting blocks"
echo
echo "-----> Test program with static library:" 

./build/pstatic c 5660 300 0 ar 3000
echo
echo "-----> Test program with shared library:" 

./build/pshared c 566 300 0 ar 300

echo
echo "-----> Test program with dynamically loaded library:"

./build/pdynamic c 566 300 0 ar 300

echo 