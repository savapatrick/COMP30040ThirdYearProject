step=0
while true; do
  for A in {1..10}; do
    for B in {1..10}; do
      for C in {1..10}; do
        for P in {10..100}; do
          for L in {1..10}; do
            python3.8 ../test_generators/generator_scott_form.py -A "$A" -B "$B" -C "$C" -E -LMIN 1 -LMAX "$L" -P "$P" -VP -TP
            cp input_tp.txt input.txt
            ../../../cmake-build-debug/project/src/theorem_prover two >/dev/null 2>&1
            exit_code=$?
            if [[ $exit_code -eq 0 ]]; then
              echo "[TP]: $step: success!"
            else
              echo "[TP]: $step: failure!"
              break
            fi
            tp_satisfiable=$(tail -1 two_variable_theorem_prover_output.txt | grep -c "proved by deriving the empty clause!")
            ./vampire/vampire_z3_Release_static_master_4764 <input_vampire.txt >output_vampire.txt
            exit_code=$?
            if [[ $exit_code -eq 0 ]]; then
              echo "[VP]: $step: success!"
            else
              echo "[VP]: $step: failure!"
              break
            fi
            vampire_satisfiable=$(grep -c "SZS status Theorem" output_vampire.txt)
            if [[ ("$tp_satisfiable" -gt 0 && "$vampire_satisfiable" -gt 0) || ("$tp_satisfiable" -eq 0 && "$vampire_satisfiable" -eq 0) ]]; then
              echo "$step: Success! Both Vampire and TP are having the same output!"
            else
              echo "$step: Failure! Vampire and TP are having different verdicts!"
              break
            fi
            echo "tp_satisfiable is $tp_satisfiable and vampire_satisfiable is $vampire_satisfiable"
            ((step = step + 1))
          done
        done
      done
    done
  done
done
