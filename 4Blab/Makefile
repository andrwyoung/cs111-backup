
files = lab4b.c
flags = -Wall -Wextra 

id = 604905807
tarname = lab3a-$(id).tar.gz
tars = Makefile README

exec = lab4b
default: $(exec)
lab4b: lab4b.c
	gcc $(flags) -o lab4b lab4b.c

.PHONY: check
check:
	echo "do this please"

.PHONY: clean
clean:
	-@rm $(exec) $(tarname)

.PHONY: dist
dist:
	tar -czf $(tarname) $(files) $(tars)
