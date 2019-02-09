#NAME: Andrew Yong
#EMAIL: yong.andrew11@gmail.com
#ID: 604905807

files = lab2_add.c utils.c
heads = utils.h
tars = Makefile

flags = -pthread -Wall -Wextra

id = 604905807
name = lab2_add
tarname = lab2a-$(id).tar.gz

build: $(files) $(heads)
	gcc $(flags) -o $(name) $(files)

.PHONY: clean
clean:
	-@rm $(name) $(tarname)

.PHONY: dist
dist:
	tar -czf $(tarname) $(files) $(tars) $(heads)
