#!/bin/sh

a=10
while [ "$a" -le "$2" ]
do
    echo $a 
   ./test.sh $1 $a | tee -a "res$1.txt"
   echo "-----------------------"
a=`expr $a \* 2`
done