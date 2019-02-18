#!/usr/bin/gnuplot
# general plot parameters
set terminal png
set datafile separator ","

set title "List-1b: total number of operations per second"
set xlabel "Threads"
set logscale x 2
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_1.png'

plot \
	"< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'mutex' with linespoints lc rgb 'purple', \
	"< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'spin' with linespoints lc rgb 'orange'



set title "List-2b: time vs threads"
set xlabel "Threads"
set logscale x 2
set ylabel "time (nanoseconds)"
set logscale y 10
set output 'lab2b_2.png'

plot \
	"< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'operations' with linespoints lc rgb 'purple', \
	"< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'time to wait' with linespoints lc rgb 'pink'



set title "List-3b: threads that run without failure (all with yields)"
set xlabel "Threads"
set logscale x 2
set ylabel "Iterations per thread"
set logscale y 10
set output 'lab2b_3.png'

plot \
	"< grep 'list-id-m,[1-8]*,[0-9]0,4,' lab2b_list.csv" using ($2):($3) \
	title 'with mutex' with points lc rgb 'purple', \
	"< grep 'list-id-s,[1-8]*,[0-9]0,4,' lab2b_list.csv" using ($2):($3) \
	title 'with spin' with points lc rgb 'orange', \
	"< grep 'list-id-none,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'no sync' with points lc rgb 'green'



set title "List-4b: throughput vs number of lists for mutex"
set xlabel "Number of lists"
set ylabel "Throughput"
set output 'lab2b_4.png'

plot \
	"< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'one thread' with linespoints lc rgb 'purple', \
	"< grep 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'two threads' with linespoints lc rgb 'red', \
	"< grep 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'four threads' with linespoints lc rgb 'blue', \
	"< grep 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'sixteen threads' with linespoints lc rgb 'black'


set title "List-5b: throughput vs number of lists for spin"
set xlabel "Number of threads"
set ylabel "Throughput"
set output 'lab2b_5.png'

plot \
	"< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'one list' with linespoints lc rgb 'purple', \
	"< grep 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'two lists' with linespoints lc rgb 'red', \
	"< grep 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'four lists' with linespoints lc rgb 'blue', \
	"< grep 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'sixteen lists' with linespoints lc rgb 'black'
