#!/bin/bash 

id=604905807

if [ $# -ne 1 ]; then
	echo "expected 1 argument: which test?"
	exit 1
fi

make
make dist
mv lab?-$id.tar.gz tester/
if  cd tester; then
	chmod +x ./test$1.sh
	./test$1.sh $id
	cd ..
fi
