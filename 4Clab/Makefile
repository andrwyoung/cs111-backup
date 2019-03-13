
files = tcp.c tls.c
execs = lab4c_tcp lab4c_tls
flags = -Wall -Wextra
libs = -lmraa -lm -lpthread

id = 604905807
tarname = lab4c-$(id).tar.gz
tars = Makefile README

default: $(execs)
lab4c_tcp: tcp.c
	gcc $(flags) -o lab4c_tcp tcp.c $(libs)
lab4c_tls: tls.c
	gcc $(flags) -o lab4c_tls tls.c $(libs)

.PHONY: clean
clean:
	-@rm $(execs) $(tarname)

.PHONY: dist
dist:
	tar -czf $(tarname) $(files) $(tars)
