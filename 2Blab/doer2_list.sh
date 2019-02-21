#!/bin/bash

threads=(1 2 4 8 12 16 24)

graph="lab2_list.csv"
> $graph

for i in ${threads[@]}; do
	./lab2_list --threads $i --iterations 1000 --sync m >> $graph 2> /dev/null 
done

