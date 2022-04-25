all: test
test: test.o pool.o graph.o
	g++ pool.o graph.o test.o -lgmp -lgmpxx -o test
graph.o: graph.cpp
	g++ -c graph.cpp -o graph.o
pool.o: pool.cpp
	g++ -c pool.cpp -o pool.o
clean:
	rm -rf pool. *.o 
