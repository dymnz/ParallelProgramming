#!/bin/sh

# Start from $1 thread to $2thread of ans_2opt_sp$3
a=$1
while [ "$a" -le $2 ]
do 

screen -dmS test$a ./test.sh $3 $a $4

a=`expr $a \* 2`
done
