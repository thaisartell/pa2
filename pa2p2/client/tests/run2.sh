#!/bin/bash

qsize=5
nthreads=10
nclients=2
port=8080
testpath=../testcases/t2/client
outpath=output/client
pgm=./client

make clean
make


for (( i = 1; i <= ${nclients}; i++ ))
    do
        ${pgm} ${testpath}${i}.txt ${qsize} ${nthreads} ${i} ${port} > ${outpath}${i}.txt &
    done