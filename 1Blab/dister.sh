#!/bin/bash 

id=604905807

make
make dist
mv lab?-$id.tar.gz tester/
if  cd tester; then
	chmod +x ./test.sh
	./test.sh $id
	cd ..
fi
