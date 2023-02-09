all: server client

server: server.cpp
	g++ server.cpp -o server
	mv server bin/.

client: client.cpp
	g++ client.cpp -o client
	mv client bin/.

clean:
	rm bin/*