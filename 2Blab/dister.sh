id=604905807

make && make dist || exit 1
mv lab2?-$id.tar.gz tester/
if cd tester; then
	chmod +x ./test.sh
	./test.sh $id
	cd ..
fi
