#!/bin/sh

./p2 $1 $2 | tee "p2.out"
./p2_pth $1 $2 | tee "p2_omp.out"

diff "p2.out" "p2_omp.out" 
rm "p2.out" "p2_omp.out" 