# NAME: Andrew Yong
# EMAIL: yong.andrew11@gmail.com
# ID: 604905807
flags = -Wall -Wextra
files = lab0.c Makefile README backtrace.png breakpoint.png
CC = gcc -o

lab0: lab0.c
	$(CC) lab0 $(flags) -g lab0.c


	

.PHONY: clean 
clean:
	-@rm lab0 lab0-604905807.tar.gz

.PHONY: dist
dist:
	tar -czf lab0-604905807.tar.gz $(files) 



.PHONY: check
check: lab0 doIn doOut doInOut doSeg doCatSeg doCatDumSeg doBadopt

doIn:
	@echo "testing" > testin
	@./lab0 --input testin > testout
	@[ `cat testout` = `cat testout` ] \
		&& echo "OK: input success" || echo "FAIL: input" 
	@rm testin testout

doOut:
	@echo "testing" | ./lab0 --output testout
	@[ `cat testout` = "testing" ] \
		&& echo "OK: output success" || echo "FAIL: output"
	@rm testout

doInOut:
	@echo "testing" >  testin
	@./lab0 --output testout --input testin
	@[ `cat testout` = `cat testout` ] \
		&& echo "OK: input + output success" || echo "FAIL: input + output"
	@rm testout testin

doSeg:
	@./lab0 --segfault; \
	[ $$? -eq 139 ] && echo "OK: segfault success" || echo "FAIL: segfault"

doCatSeg: 
	@./lab0 --catch --segfault 2> /dev/null; \
	[ $$? -eq 4 ] && echo "OK: catch segfault success" || echo "FAIL: catch segfault"

doCatDumSeg:
	@./lab0 --catch --dump-core --segfault 2> /dev/null; \
	[ $$? -eq 139 ] && echo "OK: catch segfault success" || echo "FAIL: catch segfault"

doBadopt:
	@./lab0 --bogus 2> /dev/null; \
	[ $$? -eq 1 ] && echo "OK: bad argument success" || echo "FAIL: bad argument"
