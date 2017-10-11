#!/bin/sh

#time(./ans_2opt_s ./dis_dir/test$1.dis ./in_dir/test$1.in ./output_s)
#./verify_2opt ./dis_dir/test$1.dis ./output_s

#time(./ans_2opt_p1 ./dis_dir/test$1.dis ./in_dir/test$1.in ./output_p1)
#./verify_2opt ./dis_dir/test$1.dis ./output_p1

#time(./ans_2opt_p2 ./dis_dir/test$1.dis ./in_dir/test$1.in ./output_p2)
#./verify_2opt ./dis_dir/test$1.dis ./output_p2

#time(./ans_2opt_s_timed ./dis_dir/test$1.dis ./in_dir/test$1.in ./output_s_timed)
#./verify_2opt ./dis_dir/test$1.dis ./output_s_timed

#time(./ans_2opt_sp4 ./dis_dir/test$1.dis ./in_dir/test$1.in ./output_sp4)
#./verify_2opt ./dis_dir/test$1.dis ./output_sp4

time(./ans_2opt_sp3 ../dis_dir/test$1.dis ../in_dir/test$1.in ./output_sp3)
./verify_2opt ../dis_dir/test$1.dis ./output_sp3

time(./ans_2opt_sp5 ../dis_dir/test$1.dis ../in_dir/test$1.in ./output_sp5)
./verify_2opt ../dis_dir/test$1.dis ./output_sp5

time(./ans_2opt_sp6 ./dis_dir/test$1.dis ./in_dir/test$1.in ./output_sp6)
#./verify_2opt ./dis_dir/test$1.dis ./output_sp6

time(./ans_2opt_sp7 ../dis_dir/test$1.dis ../in_dir/test$1.in ./output_sp7)
./verify_2opt ../dis_dir/test$1.dis ./output_sp7

