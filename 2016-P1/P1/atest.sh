#!/bin/sh

./P1a $1 | tee "P1a.out"
./P1a_omp $1 | tee "P1a_omp.out"

rm "P1a.out" "P1a_omp.out" 