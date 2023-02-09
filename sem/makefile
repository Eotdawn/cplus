all: sem semr semw

sem: sem.cpp
	g++ sem.cpp -o sem -lpthread
	mv sem bin/.

semr: semr.cpp
	g++ semr.cpp -o semr -lpthread
	mv semr bin/.

semw: semw.cpp
	g++ semw.cpp -o semw -lpthread
	mv semw bin/.

clean:
	rm bin/*
