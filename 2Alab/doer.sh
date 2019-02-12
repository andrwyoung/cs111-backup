#!/bin/bash

threads=(1 2 4 8 12)
iterations=(1 10 100 1000)
options=("--sync=s" "--sync=m" "huh")
flags=("--yield=i" "--yield=d" "--yield=l" "--yield=id" "--yield=il" "--yield=dl" "--yield=idl" "huh")

# options=("--sync=c" "--sync=m" "--sync=s" "huh")
# flags=("--yield" "huh")
repeats=2

if [ $# -eq 0 ] || { [ "$1" != "add" ] && [ "$1" != "list" ]; }; then
	echo "which test? add or list?"
	echo "usage: $0 add/list [more options]"
	exit 1
fi
graph="plotter/lab2_$1.csv"

count=0
for i in ${threads[@]}; do
	for j in ${iterations[@]}; do
		for k in ${options[@]}; do
			for l in ${flags[@]}; do
				for c in `seq 1 $repeats`; do 
					./lab2_list --threads $i --iterations $j $k $l >> $graph || ((count++))
				done
			done
		done
	done
done
echo there were $count errors
cd plotter && ./lab2_$1.gp
