#!/usr/bin/env bash

for i in {1..10}
do
	echo "test $i"
	./integration-test.sh > aux.txt
	echo "corect"
	grep correct aux.txt | wc -l
	echo "wrong"
	grep wrong aux.txt | wc -l
	echo "timeout"
	grep timeout aux.txt
done
