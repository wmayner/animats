#!/bin/bash

cd results/current

for ((seed=0; seed < 28; seed++))
do
  ../../build/main.o ../../parameters/basic0.txt "seed_${seed}_LOD.txt" "seed_${seed}_GEN.txt" "seed_${seed}" ${seed} 0 2>&1 | tee "seed_"$seed"_output.log" &
done
