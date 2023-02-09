all: poll epoll epoll_et

poll: poll.cpp
	g++ poll.cpp -o poll
	mv poll bin/.

epoll: epoll.cpp
	g++ epoll.cpp -o epoll
	mv epoll bin/.

epoll_et: epoll_et.cpp
	g++ epoll_et.cpp -o epoll_et
	mv epoll_et bin/.

clean:
	rm bin/*