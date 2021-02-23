#!/usr/bin/env bash

for i in {1..47}
do
  printf "=====================================\n\n"
  echo "" > basic_theorem_prover_output.txt
  echo "" > two_variable_theorem_prover_output.txt
  cp "../../samples/input$i.txt" input.txt
  timeout 10s ../../../cmake-build-debug/project/src/clauseForm basic >/dev/null 2>&1
  exit_code=$?
  if [[ $exit_code -eq 0 ]]; then
    matches="$(tail -1 basic_theorem_prover_output.txt | grep -c -f "../../samples/answer$i.ans")"
    if (( matches > 0 )); then
      echo "Output Basic Theorem Prover $i : correct"
    else
      echo "Output Basic Theorem Prover $i : wrong"
    fi
  else
    echo "Output Basic Theorem Prover $i : timeout"
  fi

  timeout 10s ../../../cmake-build-debug/project/src/clauseForm two >/dev/null 2>&1
  exit_code=$?
  if [[ $exit_code -eq 0 ]]; then
    matches="$(tail -1 two_variable_theorem_prover_output.txt | grep -c -f "../../samples/answer$i.ans")"
    if (( matches > 0 )); then
      echo "Output Two-Variable Theorem Prover $i : correct"
    else
      echo "Output Two-Variable Theorem Prover $i : wrong"
    fi
  else
    echo "Output Two-Variable Theorem Prover $i : timeout"
  fi
  printf "=====================================\n\n"
done