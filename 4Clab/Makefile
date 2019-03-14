#NAME: Andrew Yong
#EMAIL: yong.andrew11@gmail.com
#ID: 604905807

files = tcp.c tls.c converter.c
execs = lab4c_tcp lab4c_tls
flags = -Wall -Wextra
libs = -lmraa -lm -lpthread -lssl -lcrypto

id = 604905807
tarname = lab4c-$(id).tar.gz
tars = Makefile README converter.h

default: $(execs)
lab4c_tcp: tcp.c
	gcc $(flags) -o lab4c_tcp tcp.c $(libs)
lab4c_tls: tls.c converter.c
	gcc $(flags) -o lab4c_tls tls.c converter.c $(libs) 

.PHONY: clean
clean:
	-@rm $(execs) $(tarname)

.PHONY: dist
dist:
	tar -czf $(tarname) $(files) $(tars)
