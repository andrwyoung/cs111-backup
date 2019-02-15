#!/bin/bash

threads=(1 2 4 8 12)
iterations=(1 10 100 1000)
options=("--sync=c" "--sync=m" "--sync=s" "huh")
flags=("--yield" "huh")

graph="lab2_add.csv"
> $graph

count=0
for i in ${threads[@]}; do
	for j in ${iterations[@]}; do
		for k in ${options[@]}; do
			for l in ${flags[@]}; do
				./lab2_add --threads $i --iterations $j $k $l >> $graph || ((count++))
			done
		done
	done
done

# test 5
for i in ${threads[@]}; do
	for k in ${options[@]}; do
		./lab2_add --threads $i --iterations 10000 $k $l >> $graph || ((count++))
	done
done


