#!/bin/bash
for ((i = 1 ; i <= 64 ; i=i*2)); do
    ./sim -f 1.trc -s 512 -b $i -a 2 -x b -r RR
done
for ((i = 1 ; i <= 64 ; i=i*2)); do
    ./sim -f 2.trc -s 512 -b $i -a 2 -x b -r RR
done
for ((i = 1 ; i <= 64 ; i=i*2)); do
    ./sim -f 3.trc -s 512 -b $i -a 2 -x b -r RR
done
