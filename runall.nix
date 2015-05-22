#!/bin/bash

make

PARAM_FILE="parameters/1-3-1-3.txt"

for ((seed=64; seed < 96; seed++))
do
  result_dir="results/current/seed-$seed"
  ./build/main.o $PARAM_FILE "$result_dir/LOD.csv" "$result_dir/genomes.csv" "$result_dir/" $seed 0 2>&1 | tee "$result_dir/output.log" &
done
