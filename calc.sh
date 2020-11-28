#!/bin/bash

rm averages.csv

echo "Test;Average;Iterations;Generations average" >> averages.csv
for f in build/res*
do
	avg=`awk -F ";" '{ sum += $4 }; END { print sum/NR }' $f`
	avg_iter=`awk -F ";" '{ sum += $5 }; END { print sum/NR }' $f`
	amount=`awk -F ";" 'END { print NR}' $f`
	echo "$f;$avg;$amount;$avg_iter" >> averages.csv
done
