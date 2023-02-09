all: lock

lock: lock.cpp
	g++ lock.cpp -o lock -lpthread
	mv lock bin/.

clean:
	rm bin/*