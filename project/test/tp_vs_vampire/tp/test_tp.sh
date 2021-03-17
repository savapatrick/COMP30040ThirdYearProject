step=0
while true; do
  python3.8 ../../test_generators/generator_scott_form.py -A 10 -B 10 -C 2 -E -LMIN 3 -LMAX 3 -P 25 -TP
  cp input_tp.txt input.txt
  ../../../../cmake-build-debug/project/src/theorem_prover two >/dev/null 2>&1
  exit_code=$?
  if [[ $exit_code -eq 0 ]]; then
    echo "$step: success!"
  else
    echo "$step: failure!"
    break
  fi
  ((step = step + 1))
done
