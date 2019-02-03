#!/bin/bash
# NAME: Andrew Yong
# EMAIL: yong.andrew11@gmail.com
# ID: 604905807

echo "hey
there
interesting" > input.txt 
touch output.txt error.txt

# test 1: simple
./simpsh
if [ "$?" -eq 0 ]; then
	echo "OK check 1"
else
	echo "FAIL check 1"
fi

# test 2: can it open fds successfully
./simpsh --rdonly input.txt --wronly output.txt --wronly error.txt
if [ "$?" -eq 0 ] && [ ! -s output.txt ] && [ ! -s error.txt ]; then
	echo "OK check 2"
else
	echo "FAIL check 2"
fi

# test 3: do a simple command
./simpsh --rdonly input.txt --wronly output.txt --wronly error.txt --command 0 1 2 cat --wait
if [ "$?" -eq 0 ] && cmp -s output.txt input.txt; then
	echo "OK check 3"
else
	echo "FAIL check 3"
fi

# test 4: bad command
./simpsh --rdonly input.txt --wronly output.txt --wronly error.txt --command 0 1 2 cattftf --wait
if [ -s error.txt ]; then
	echo "OK check 4"
else
	echo "FAIL check 4"
fi

#rm input.txt output.txt error.txt

