#!/bin/bash

cd results/current

for ((seed=0; seed < 28; seed++))
do
  ../../build/main.o ../../parameters/basic0.txt "seed-${seed}_LOD.csv" "seed-${seed}_GEN.txt" "seed-${seed}" ${seed} 0 2>&1 | tee "seed-"$seed"_output.log" &
done
