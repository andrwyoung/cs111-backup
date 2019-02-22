files = lab3a.c
heads = ext_fs.h
flags = -Wall -Wextra

id = 604905807
tarname = lab3a-$(id).tar.gz
tars = Makefile README

exec = lab3a
default: $(exec)
lab3a: lab3a.c
	gcc $(flags) -o lab3a lab3a.c

.PHONY: clean
clean:
	-@rm $(exec) $(tarname)

.PHONY: dist
dist:
	tar -czf $(tarname) $(files) $(heads) $(tars)
	
