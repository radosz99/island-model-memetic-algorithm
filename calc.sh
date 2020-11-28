#!/bin/bash

rm averages.csv

echo "Test;Average;Best;Worst;Iterations;Generations average" >> averages.csv
for f in build/res*
do
	avg=`awk -F ";" '{ sum += $4 }; END { print sum/NR }' $f`
	avg_iter=`awk -F ";" '{ sum += $5 }; END { print sum/NR }' $f`
	best=`awk -F ";" '{ print $4 }' $f | sort -n | head -1`
	worst=`awk -F ";" '{ print $4 }' $f | sort -n | tail -1`
	amount=`awk -F ";" 'END { print NR}' $f`
	echo "$f;$avg;$best;$worst;$amount;$avg_iter" >> averages.csv
done

