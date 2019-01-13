# NAME: Andrew Yong
# EMAIL: yong.andrew11@gmail.com
# ID: 604905807
flags = -Wall -Wextra
files = lab0.c Makefile README
CC = gcc -o

lab0: lab0.c
	$(CC) lab0 $(flags) lab0.c


.PHONY: check
check:
	echo nope
	
.PHONY: clean 
clean:
	rm $(files) lab0

.PHONY: debug
debug:
	$(CC) lab0 $(flags) -g lab0.c

.PHONY: dist
dist:
	tar -czf lab0-604905807.tar.gz $(files) 
