id=604905807

if [ $# -ne 1 ]; then
	echo which test?
	exit 1
fi

make && make dist || exit 1
mv lab3?-$id.tar.gz tester/
if cd tester; then
	chmod +x ./test$1.sh
	./test$1.sh $id
	cd ..
fi
