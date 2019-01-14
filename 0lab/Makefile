# NAME: Andrew Yong
# EMAIL: yong.andrew11@gmail.com
# ID: 604905807
flags = -Wall -Wextra
files = lab0.c Makefile README backtrace.png breakpoint.png
CC = gcc -o

lab0: lab0.c
	$(CC) lab0 $(flags) -g lab0.c


.PHONY: check
check: lab0
	echo "This is just a test" >  test-input
	./lab0 --output test-output --input test-input --catch --dump-core
	rm test-output test-input
	$(info --catch --dump-core --output and --input work)
	

.PHONY: clean 
clean:
	-rm lab0 lab0-604905807.tar.gz

.PHONY: dist
dist:
	tar -czf lab0-604905807.tar.gz $(files) 
