#!/bin/sh


echo "testing: $2 threads" | tee -a "./results/thread_test_sp$1_$2.txt"
echo "running ans_2opt_sp$1" | tee -a "./results/thread_test_sp$1_$2.txt"

time(./ans_2opt_sp$1 ../as_1/dis_dir/test11.dis ../as_1/in_dir/test11.in ./output_sp$1_$2 $2) | tee -a "./results/thread_test_sp$1_$2.txt"
./verify_2opt ../as_1/dis_dir/test11.dis ./output_sp$1_$2 | tee -a "./results/thread_test_sp$1_$2.txt"

