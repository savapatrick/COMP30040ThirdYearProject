step=0
while true
do
  python3.8 ../../test_generators/generator_scott_form.py -A 5 -B 3 -C 2 -E -LMIN 3 -LMAX 3 -P 4 -TP
  ../../../../cmake-build-debug/project/src/theorem_prover two < input_tp.txt > output_tp.txt
  exit_code=$?
  if [[ $exit_code -eq 0 ]]; then
    echo "$step: success!"
  else
    echo "$step: failure!"
  fi
  ((step=step+1))
done