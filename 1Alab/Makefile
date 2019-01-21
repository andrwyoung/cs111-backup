# NAME: Andrew Yong
# EMAIL: yong.andrew11@gmail.com
# ID: 604905807

files = simpsh.c Makefile checker.sh README
id = 604905807

simpsh: simpsh.c

.PHONY: clean
clean:
	-@rm simpsh lab1-$(id).tar.gz

.PHONY: dist
dist:
	tar -czf lab1-$(id).tar.gz $(files)

.PHONY: check
check:
	chmod +x checker.sh
	./checker.sh


