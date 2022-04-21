all: test
test: test.o pool.o
	g++ pool.o test.o -lgmp -lgmpxx -o test
pool.o: pool.cpp
	g++ -c pool.cpp -o pool.o
clean:
	rm -rf pool. *.o                     
