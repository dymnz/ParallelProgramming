#!/bin/sh
a=$5
while [ "$a" -gt 0 ]
do
    echo $a
   ./thread_test.sh $1 $2 $3 $4
	sleep $6
a=`expr $a - 1`
done
