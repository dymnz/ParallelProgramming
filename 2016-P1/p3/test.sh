#!/bin/sh

time(./P3_mod $1 $2)
time(./P3_ori $1 $2)
time(./P3_omp $1 $2)
