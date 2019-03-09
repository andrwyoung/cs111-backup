# NAME: Andrew Yong
# EMAIL: yong.andrew11@gmail.com
# ID: 604905807

files = lab4b.c
flags = -Wall -Wextra -lmraa -lm

id = 604905807
tarname = lab3a-$(id).tar.gz
tars = Makefile README

exec = lab4b
default: $(exec)
lab4b: lab4b.c
	gcc -o lab4b lab4b.c $(flags) 

.PHONY: check
check:
	echo "do this please"

.PHONY: clean
clean:
	-@rm $(exec) $(tarname)

.PHONY: dist
dist:
	tar -czf $(tarname) $(files) $(tars)
