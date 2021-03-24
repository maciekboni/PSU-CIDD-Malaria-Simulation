#!/bin/bash

t=1.6
p=1000

for i in `seq 1 1`; do
  for d in `seq 1 3`; do
    for k in `seq 0.95 0.005 0.995`; do
      for e in `seq 0.5 0.05 1.25`; do
        for n in `seq 5 5 20`; do
           ../../../build/bin/EfficacyEstimator -t $t 4.5 -k $k 0.99 -e $e 0.75 -n $n 20 -d $d $d -p $p >> result.csv
        done
      done
    done
  done
done
