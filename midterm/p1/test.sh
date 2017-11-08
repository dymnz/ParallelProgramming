#!/bin/sh

echo "original:"
./p1 $1 $2

echo "omp:"
./p1_omp $1 $2

#echo "omp2:"
#./p1_omp2 $1 $2