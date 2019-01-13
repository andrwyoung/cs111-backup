# NAME: Andrew Yong
# EMAIL: yong.andrew11@gmail.com
# ID: 604905807
flags = -Wall -Wextra

lab0: lab0.c
	gcc -o lab0 $(flags) lab0.c

.PHONY: clean dist
clean:
	rm lab0

dist:
	tar -czf lab0-604905807.tar.gz lab0 Makefile README 
