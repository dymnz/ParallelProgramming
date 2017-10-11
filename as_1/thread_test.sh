#!/bin/sh

a=1
while [ "$a" -le 16 ]
do 
echo "testing: $a threads" | tee -a "./results/thread_test_sp3.txt" 
echo "running ans_2opt_sp3" | tee -a "./results/thread_test_sp3.txt"

time(./ans_2opt_sp3 ./dis_dir/test11.dis ./in_dir/test11.in ./output_sp3 $a) | tee -a "./results/thread_test_sp3.txt"
./verify_2opt ./dis_dir/test11.dis ./output_sp3 | tee -a "./results/thread_test_sp3.txt"

a=`expr $a \* 2`
done


a=122
while [ "$a" -le 16 ]
do 
echo "testing: $a threads" | tee -a "./results/thread_test_sp5.txt"
echo "running ans_2opt_sp5" | tee -a "./results/thread_test_sp5.txt"

time(./ans_2opt_sp5 ./dis_dir/test11.dis ./in_dir/test11.in ./output_sp5 $a) | tee -a "./results/thread_test_sp5.txt"
./verify_2opt ./dis_dir/test$a.dis ./output_sp5 | tee -a "./results/thread_test_sp5.txt"

a=`expr $a \* 2`
done
