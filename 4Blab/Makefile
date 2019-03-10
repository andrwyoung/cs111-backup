# NAME: Andrew Yong
# EMAIL: yong.andrew11@gmail.com
# ID: 604905807

files = lab4b.c
flags = -Wall -Wextra
libs =  -lmraa -lm -lpthread


id = 604905807
tarname = lab4b-$(id).tar.gz
tars = Makefile README checker.c

exec = lab4b
default: $(exec)
lab4b: lab4b.c
	gcc $(flags) -o lab4b lab4b.c $(libs)

.PHONY: check
check: checker
	./checker && echo sensor OK
checker: checker.c
	gcc -o checker checker.c -lmraa


.PHONY: clean
clean:
	-@rm $(exec) $(tarname) checker

.PHONY: dist
dist:
	tar -czf $(tarname) $(files) $(tars)
