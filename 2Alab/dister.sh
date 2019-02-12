id=604905807

# I like having the graphs in the folder
cp ./graphs/* .

make && make dist || exit 1
mv lab2?-$id.tar.gz tester/
if cd tester; then
	chmod +x ./test.sh
	./test.sh $id
	cd ..
fi

# they better all be in graphs
rm *.png *.csv
