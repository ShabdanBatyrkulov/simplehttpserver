server: server.o httpserver.o httpmessage.o
	g++ server.o httpserver.o httpmessage.o -o server -pthread

server.o: server.cc
	g++ server.cc -c

httpserver.o: httpserver.cc
	g++ httpserver.cc -c

httpmessage.o: httpmessage.cc
	g++ httpmessage.cc -c

clean:
	rm -f server *.o