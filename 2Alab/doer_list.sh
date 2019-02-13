#!/bin/bash

threads=(1 2 4 8 12)
threads1=(1 2 4 8 12 16 24)
iterations=(10 100 1000 10000 20000)
iterations1=(1 10 100 1000)
iterations2=(1 2 4 8 16 32)
options=("--sync=s" "--sync=m" "huh")
options1=("--sync=s" "--sync=m")
flags=("--yield=i" "--yield=d" "--yield=l" "--yield=id" "--yield=il" "--yield=dl" "--yield=idl" "huh")

graph="lab2_list.csv"

count=0

echo graphing 1/4
# graph 1 - big numbers
for j in ${iterations[@]}; do
	./lab2_list --threads 1 --iterations $j >> $graph 2> /dev/null|| ((count++))
done

echo graphing 2/4 
# graph 2 - bigish iterations
for i in ${threads[@]}; do
	for j in ${iterations1[@]}; do
		./lab2_list --threads $i --iterations $j >> $graph 2> /dev/null || ((count++))
	done
done

echo graphing 3/4
# graph 3 - with low amount of yields
for i in ${threads[@]}; do
	for j in ${iterations2[@]}; do
		for k in ${options[@]}; do
			for l in ${flags[@]}; do
				./lab2_list --threads $i --iterations $j $k $l >> $graph 2> /dev/null || ((count++))
			done
		done
	done
done

echo "graphing 4/4 (give it a min)"
# graph 4 - without yields
for i in ${threads1[@]}; do
	for k in ${options1[@]}; do
		./lab2_list --threads $i --iterations 1000 $k  >> $graph 2> /dev/null || ((count++))
	done
done

