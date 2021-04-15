step=0
sat=0
failTp=0
failVampire=0
echo "" > tp_overall_errors.txt
echo "" > vampire_overall_errors.txt
while true; do
  for A in {1..6}; do
    for C in {1..5}; do
      for P in {1..250}; do
        for L in {1..5}; do
          python3.8 ../test_generators/generator_scott_form.py -A "$A" -C "$C" -E -LMIN 1 -LMAX "$L" -P "$P" -VP -TP
          cp input_tp.txt input.txt
          ../../../cmake-build-debug/project/src/theorem_prover two > possible_errors_tp.txt 2>&1
          exit_code=$?
          if [[ $exit_code -eq 0 ]]; then
            echo "[TP]: $step: success!"
          else
            ((failTp = failTp + 1))
            echo "[TP]: $step: failure!"
            cat possible_errors_tp.txt >> tp_overall_errors.txt
            continue
          fi
          tp_satisfiable=$(tail -1 two_variable_theorem_prover_output.txt | grep -c "proved by deriving the empty clause!")
          ./vampire/vampire_z3_Release_static_master_4764 <input_vampire.txt >output_vampire.txt
          exit_code=$?
          if [[ $exit_code -eq 0 ]]; then
            echo "[VP]: $step: success!"
          else
            ((failVampire = failVampire + 1))
            echo "[VP]: $step: failure!"
            cat possible_errors_vampire.txt >> vampire_overall_errors.txt
            continue
          fi
          vampire_satisfiable=$(grep -c "SZS status Satisfiable" output_vampire.txt)
          echo "tp_satisfiable is $tp_satisfiable and vampire_satisfiable is $vampire_satisfiable"
          if [[ ("$tp_satisfiable" -eq 0 && "$vampire_satisfiable" -gt 0) || ("$tp_satisfiable" -gt 0 && "$vampire_satisfiable" -eq 0) ]]; then
            echo "$step: Success! Both Vampire and TP are having the same output!"
          else
            echo "$step: Failure! Vampire and TP are having different verdicts!"
            exit 0
          fi
          grep "status" output_vampire.txt
          if [[ $vampire_satisfiable -gt 0 ]]; then
            echo "The formula is counter satisfiable!"
            ((sat = sat + 1))
          else
            echo "The formula is theorem!"
          fi
          echo "The number of counter satisfiable formulas so far is $sat"
          echo "FailTp=${failTp} and FailVampire=${failVampire}"
          ((step = step + 1))
        done
      done
    done
  done
done
