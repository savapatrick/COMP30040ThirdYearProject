step=0
while true; do
  python3.8 ../../test_generators/generator_scott_form.py -A 500 -B 6 -C 110 -E -LMIN 1 -LMAX 5 -P 1000000 -VP
  ./vampire_z3_Release_static_master_4764 <input_vampire.txt >output_vampire.txt
  exit_code=$?
  if [[ $exit_code -eq 0 ]]; then
    echo "$step: success!"
    echo "$(grep status output_vampire.txt)"
  else
    echo "$step: failure!"
  fi
  ((step = step + 1))
done
