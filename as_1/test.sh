#!/bin/sh

#time(./ans_2opt_s ./dis_dir/test$1.dis ./in_dir/test$1.in ./output_s)
#./verify_2opt ./dis_dir/test$1.dis ./output_s
#time(./ans_2opt_p1 ./dis_dir/test$1.dis ./in_dir/test$1.in ./output_p1)
#./verify_2opt ./dis_dir/test$1.dis ./output_p1
#time(./ans_2opt_p2 ./dis_dir/test$1.dis ./in_dir/test$1.in ./output_p2)
#./verify_2opt ./dis_dir/test$1.dis ./output_p2

time(./ans_2opt_s_timed ./dis_dir/test$1.dis ./in_dir/test$1.in ./output_s_timed)
#./verify_2opt ./dis_dir/test$1.dis ./output_s_timed

time(./ans_2opt_sp ./dis_dir/test$1.dis ./in_dir/test$1.in ./output_sp)
#./verify_2opt ./dis_dir/test$1.dis ./output_sp





