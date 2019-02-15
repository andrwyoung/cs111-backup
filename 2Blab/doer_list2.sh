#!/bin/bash

threads0=(1 2 4 8 12 16 24)
threads1=(1 2 4 8 12 16)
threads2=(1 2 4 8 12)

iterations0=(1 2 4 8 12 16)
iterations1=(10 20 40 80)
lists=(1 4 8 16)
syncs=("--sync=m" "--sync=s")


graph="lab2b_list.csv"
> $graph

#graph 2
echo "graphing 1/3 (like 30 sec)"
for i in ${threads0[@]}; do
	for k in ${syncs[@]}; do
		./lab2_list --threads $i --iterations 1000 $k >> $graph 2> /dev/null
	done
done


#graph 3a
echo "graphing 2/3 (like 30 sec)"
for i in ${threads1[@]}; do
	for j in ${iterations0[@]}; do
		./lab2_list --threads $i --iterations $j --lists 4 --yield id >> $graph 2> /dev/null
	done
done

#graph 3b
for i in ${threads1[@]}; do
	for j in ${iterations1[@]}; do
		for k in ${syncs[@]}; do
			./lab2_list --threads $i --iterations $j --lists 4 --yield id $k >> $graph 2> /dev/null
		done
	done
done

#graph 4 and 5
echo graphing 3/3
for i in ${threads2[@]}; do
	for j in ${lists[@]}; do
		for k in ${syncs[@]}; do
			./lab2_list --threads $i --iterations 1000 --lists $j $k >> $graph 2> /dev/null
		done
	done
done
			
