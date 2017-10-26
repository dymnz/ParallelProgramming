#!/bin/sh


echo "testing: $2 threads" | tee -a "./results/$1_$2.txt"
echo "running ans_2opt_omp$1" | tee -a "./results/$1_$2.txt"

time(./ans_2opt_omp$1 ../as_1/dis_dir/test$3.dis ../as_1/in_dir/test$3.in ./output_omp$1_$2 $2) | tee -a "./results/$1_$2.txt"
./verify_2opt ../as_1/dis_dir/test$3.dis ./output_omp$1_$2 | tee -a "./results/$1_$2.txt"

rm ./output_omp$1_$2

