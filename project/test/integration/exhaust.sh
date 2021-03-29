#!/usr/bin/env bash

for i in {1..3000}; do
  echo "test $i"
 ./integration-test.sh &>"aux$i.txt"
# echo "corect: "
# grep correct "aux$i.txt" | wc -l
# echo "wrong"
  grep wrong "aux$i.txt"
# echo "timeout"
# grep timeout "aux$i.txt"
done
