#!/bin/sh

./P1b input$1.txt output$1_o.txt | tee "P1b.out"
./P1b_omp input$1.txt output$1.txt | tee "P1b_omp.out"

diff output$1_o.txt output$1.txt
rm output$1_o.txt output$1.txt


