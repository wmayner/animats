#!/bin/bash

make

PARAM_FILE=parameters/basic0.txt

for ((seed=32; seed < 64; seed++))
do
  result_dir="results/current/seed-$seed"
  ./build/main.o $PARAM_FILE "$result_dir/LOD.csv" "$result_dir/genomes.csv" "$result_dir/" $seed 0 2>&1 | tee "$result_dir/output.log" &
done
