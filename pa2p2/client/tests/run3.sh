#!/bin/bash

qsize=10
nthreads=20
nclients=10
port=8080
testpath=../testcases/t3/client
outpath=output/client
pgm=./client

make clean
make


for (( i = 1; i <= ${nclients}; i++ ))
    do
        ${pgm} ${testpath}${i}.txt ${qsize} ${nthreads} ${i} ${port} > ${outpath}${i}.txt &
    done