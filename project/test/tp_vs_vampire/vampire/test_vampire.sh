step=0
sat=0
while true; do
  for A in {1..7}; do
    for C in {1..7}; do
      for P in {1..7}; do
        for LMAX in {1..7}; do
          echo "A=$A B=$B C=$C P=$P LMAX=$LMAX"
          python3.8 ../../test_generators/generator_scott_form.py -A "$A" -C "$C" -E -LMIN 1 -LMAX "$LMAX" -P "$P" -VP
          ./vampire_z3_Release_static_master_4764 <input_vampire_statistics.txt >output_vampire.txt
          exit_code=$?
          if [[ $exit_code -eq 0 ]]; then
            grep "status" output_vampire.txt
            vampire_satisfiable=$(grep -c "SZS status Satisfiable" output_vampire.txt)
            if [[ $vampire_satisfiable -gt 0 ]]; then
              echo "FOUND!!!"
              ((sat = sat + 1))
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
