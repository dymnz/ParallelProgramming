#!/bin/sh


echo "testing: $2 threads" | tee -a "./results/thread_test_sp$1_$2.txt"
echo "running ans_2opt_sp$1" | tee -a "./results/thread_test_sp$1_$2.txt"

time(./ans_2opt_sp$1 ./dis_dir/test$3.dis ./in_dir/test$3.in ./output_sp$1_$2 $2) | tee -a "./results/thread_test_sp$1_$2.txt"
./verify_2opt ./dis_dir/test$3.dis ./output_sp$1_$2 | tee -a "./results/thread_test_sp$1_$2.txt"

