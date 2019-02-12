#NAME: Andrew Yong
#EMAIL: yong.andrew11@gmail.com
#ID: 604905807

files = lab2_add.c SortedList.c lab2_list.c
heads = SortedList.h
pics1 = lab2_add.csv lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png

execs = lab2_add lab2_list
tars = Makefile README

flags = -pthread -Wall -Wextra

id = 604905807
tarname = lab2a-$(id).tar.gz

build: lab2_add lab2_list 

lab2_add: lab2_add.c
	gcc $(flags) -o lab2_add lab2_add.c
lab2_list: lab2_list.c SortedList.c
	gcc $(flags) -o lab2_list lab2_list.c SortedList.c


.PHONY: clean
clean:
	-@rm $(execs) $(tarname)

.PHONY: dist
dist:
	tar -czf $(tarname) $(files) $(tars) $(heads) $(pics1)
