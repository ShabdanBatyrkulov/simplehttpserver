The server consists of:
 - 1 main thread
 - 1 listener thread that is responsible for accepting new connections
 - Possibly many threads that process HTTP messages and communicate with clients via socket.
   The number of workers is defined by a constant (5).

First of all, epoll is specific to Linux System . So this code is not portable on other OS.
We create a socket, bind it to the specified port (given by user), and start listening for incoming 
connections using listen. We then create an epoll instance for each worker using epoll_create1. And handle events.

