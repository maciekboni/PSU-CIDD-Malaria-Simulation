#!/bin/bash

p=10000

for e in `seq 0.5 0.01 1.4`; do
     ./EfficacyEstimator -t 0.0 4.5 -k 0.999 0.99 -e 1.2 $e -n 25 20 -d 3 3 -p $p >> result.csv
done