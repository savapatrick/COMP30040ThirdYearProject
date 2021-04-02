step=0
while true; do
  for A in {1..15}; do
    for B in {1..15}; do
      for C in {1..15}; do
        for P in {1..15}; do
          for LMAX in {1..15}; do
            python3.8 ../../test_generators/generator_scott_form.py -A "$A" -B "$B" -C "$C" -E -LMIN 1 -LMAX "$LMAX" -P "$P" -VP
            ./vampire_z3_Release_static_master_4764 <input_vampire.txt >output_vampire.txt
            exit_code=$?
            if [[ $exit_code -eq 0 ]]; then
              grep "status" output_vampire.txt
              vampire_satisfiable=$(grep -c "SZS status Theorem" output_vampire.txt)
              if [[ $vampire_satisfiable -gt 0 ]]; then
                echo "FOUND!!!"
                break
              fi
              echo "$step: success!"
            else
              echo "$step: failure!"
            fi
            ((step = step + 1))
          done
        done
      done
    done
  done
done
