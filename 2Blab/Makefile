#NAME: Andrew Yong
#EMAIL: yong.andrew11@gmail.com
#ID: 604905807

files = lab2_add.c SortedList.c lab2_list.c
heads = SortedList.h
pics1 = lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png
pics2 = lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png
csvs = lab2_add.csv lab2_list.csv lab2b_list.csv

graphers = lab2_add.gp lab2_list.gp 2b_grapher.gp
doers = doer_add.sh doer_list.sh

execs = lab2_add lab2_list
tars = Makefile README profile.out

flags = -pthread -Wall -Wextra
profile = -g -lprofiler
id = 604905807
tarname = lab2b-$(id).tar.gz

build: lab2_add lab2_list 

lab2_add: lab2_add.c
	gcc $(flags) $(profile) -o lab2_add lab2_add.c
lab2_list: lab2_list.c SortedList.c
	gcc $(flags) $(profile) -o lab2_list lab2_list.c SortedList.c

.PHONY: tests
tests: $(execs)
	./doer_add.sh 
	./doer_list.sh

.PHONY: graphs
graphs: tests $(graphers) $(csvs) 
	./lab2_add.gp
	./lab2_list.gp

.PHONY: clean
clean:
	-@rm $(execs) $(tarname) profiler.prof

.PHONY: profile
profile:
	export CPUPROFILE=profiler.prof; \
	./lab2_list --threads 10 --iterations 1000 --sync s; \
	pprof --list spin_pthreader lab2_list profiler.prof > profile.txt

.PHONY: dist
dist: graphs
	tar -czf $(tarname) $(files) $(tars) $(heads) $(pics1) $(pics2) $(csvs) $(doers) $(graphers)
