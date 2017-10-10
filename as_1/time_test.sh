#!/bin/sh

echo "running ans_2opt_sp3" | tee -a "./results/time_test_sp3.txt"
time(./ans_2opt_sp3 ./dis_dir/test11.dis ./in_dir/test11.in ./output_sp3 16) | tee -a "./results/time_test_sp3.txt"
./verify_2opt ./dis_dir/test$a.dis ./output_sp3 | tee -a "./results/time_test_sp3.txt"

echo "running ans_2opt_sp5" | tee -a "./results/time_test_sp5.txt"
time(./ans_2opt_sp5 ./dis_dir/test11.dis ./in_dir/test11.in ./output_sp5 16) | tee -a "./results/time_test_sp5.txt"
./verify_2opt ./dis_dir/test$a.dis ./output_sp5 | tee -a "./results/time_test_sp5.txt"