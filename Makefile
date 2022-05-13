CC=g++
CFLAGS=-Ijson-develop/include -I.

client: client.cpp requests.cpp helpers.cpp buffer.cpp
	$(CC) $(CFLAGS) -o client client.cpp requests.cpp helpers.cpp buffer.cpp -Wall

run: client
	./client

clean:
	rm -f *.o client
