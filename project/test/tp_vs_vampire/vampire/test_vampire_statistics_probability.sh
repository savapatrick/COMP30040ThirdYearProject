step=0
sat=0
AMIN=1
AMAX=50
CMIN=1
CMAX=50
PMIN=1
PMAX=100
cat /dev/null > "results_A_${AMIN}_${AMAX}_C_${CMIN}_${CMAX}_P_${PMIN}_${PMAX}_LMIN_1_LMAX_3.txt"
for steps in {1..100}; do
  for A in $(seq $AMIN $AMAX); do
    for C in 10 25 40 50; do
      for P in 10 35 60 100; do
        for LMAX in {3..3}; do
          echo "steps=$steps A=$A C=$C P=$P LMAX=$LMAX"
          python3.8 ../../test_generators/generator_scott_form.py -A "$A" -C "$C" -E -LMIN 1 -LMAX "$LMAX" -P "$P" -VP
          ./vampire_z3_Release_static_master_4764 <input_vampire_statistics.txt >output_vampire.txt
          exit_code=$?
          if [[ $exit_code -eq 0 ]]; then
            grep "status" output_vampire.txt
            vampire_satisfiable=$(grep -c "SZS status Satisfiable" output_vampire.txt)
            if [[ $vampire_satisfiable -gt 0 ]]; then
              echo "$A $P $C Satisfiable"   >> "results_A_${AMIN}_${AMAX}_C_${CMIN}_${CMAX}_P_${PMIN}_${PMAX}_LMIN_1_LMAX_${LMAX}.txt"
              ((sat = sat + 1))
            else
              echo "$A $P $C Unsatisfiable" >> "results_A_${AMIN}_${AMAX}_C_${CMIN}_${CMAX}_P_${PMIN}_${PMAX}_LMIN_1_LMAX_${LMAX}.txt"
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
