all: client tracker

client: client.cpp
		g++ client.cpp -std=c++11 -pthread -lcrypto	 -Wall -o client

tracker:tracker.cpp
		g++ tracker.cpp -std=c++1z -Wall -o tracker

clean: 
	-rm -f client tracker
