#!/usr/bin/gnuplot
# general plot parameters
set terminal png
set datafile separator ","

set title "List-1b: total number of operations per second"
set xlabel "Threads"
set ylabel "Throughput"
set output 'lab2b_1.png'

plot \
	"< grep 'list-none-m,[0-9]*,1000,' lab2_list.csv" using ($2):($7)/(1000000000) \
	title 'mutex' with linespoints lc rgb 'cyan', \
	"< grep 'list-none-s,[0-9]*,1000,' lab2_list.csv" using ($2):($7)/(1000000000) \
	title 'spin' with linespoints lc rgb 'orange'
