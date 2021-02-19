#!/usr/bin/env bash

i=0
while true; do
  printf "=====================================\n\n"
  python3.8 generator.py
  exit_code=$?
  if [[ $exit_code -ne 0 ]]; then
    continue
  fi
  cp output.txt input.txt
  ../../../cmake-build-debug/project/src/clauseForm none >/dev/null 2>&1
  exit_code=$?
  if [[ $exit_code -eq 0 ]]; then
    echo "$i: Clause form works!"
  else
    echo "$i: Clause form fails! have a look in input.txt"
    break
  fi
  ((i=i+1))
done