step=0
sat=0
AMAX=50
CMAX=50
while true; do
  for A in $(seq 1 $AMAX); do
    for C in $(seq 1 $CMAX); do
      for P in {7..7}; do
        for LMAX in {3..3}; do
          echo "A=$A B=$B C=$C P=$P LMAX=$LMAX"
          python3.8 ../../test_generators/generator_scott_form.py -A "$A" -C "$C" -E -LMIN 1 -LMAX "$LMAX" -P "$P" -VP
          ./vampire_z3_Release_static_master_4764 <input_vampire_statistics.txt >output_vampire.txt
          exit_code=$?
          if [[ $exit_code -eq 0 ]]; then
            grep "status" output_vampire.txt
            vampire_satisfiable=$(grep -c "SZS status Satisfiable" output_vampire.txt)
            if [[ $vampire_satisfiable -gt 0 ]]; then
              echo "$A $C Satisfiable" >>"results_A_1_${AMAX}_C_1_${CMAX}_P_${P}_LMIN_1_LMAX_${LMAX}.txt"
              ((sat = sat + 1))
            else
              echo "$A $C Unsatisfiable" >>"results_A_1_${AMAX}_C_1_${CMAX}_P_${P}_LMIN_1_LMAX_${LMAX}.txt"
            fi
            echo "$step: success!"
          else
            echo "$step: failure!"
            exit 0
          fi
          echo "number of satisfiable formulas so far is $sat"
          ((step = step + 1))
        done
      done
    done
  done
done
