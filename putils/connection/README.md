# connection
A simple and re-usable C++ network abstraction

## Listeners
The "Listener" family class is an abstraction to server sockets.

### ITCPListener
Basic interface for the Listener classes. It inherits from `putils::Observable` and will notify its observers whenever a new client connects. Observers can then get to the new client by using `getLastClient()`.
Clients themselves inherit from `putils::Observable` and will notify their observers whenever a new message arrives. Observers should not explicitly capture references to `Client` objects, as these may be stored in a vector and references may therefore be invalidated.
Instead, observers should capture the `fd` associated with a client and use it to `get()` the client when notified.

See below for an example usage in a basic server.

ITCPListener allows for two main operations:

#### `select(timeval &&timeout = {-1, -1})`
Wait for new connections and/or new messages from existing clients. Observers will be notified if one of these happens.
If no timeout is specified, the function will block until something happens.

#### `send(int fd, const std::string &msg)`
Send `msg` to the client associated with `fd`.

### ATCPListener
"Typical" implementation of the core part of of ITCPListener. This allows future implementations to only specify how to perform very low level operations. The only pure functions left to implement are:

##### `doRemove(int fd)`
Perform whatever should happen when removing a client (such as freeing the SSL* pointer associated with it in the case of OpenSSL).

##### `doAccept(int fd)`
Perform whatever should happen when accepting a new client (such as SSL_accept in the case of OpenSSL)

##### `doRead(int fd, char *dest, size_t length)`
Perform a read operation, reading up to `length` bytes into `dest`

##### `doWrite(int fd, const char *data, size_t length)`
Perform a write operation, writing `length` bytes from `data`

### TCPListener
A final implementation of a listening socket.
`TCPListener` is constructed the following way:
```
TCPListener(short port, const std::string &host = "127.0.0.1");
```

### OpenSSLTCPListener
A final implementation of a listening socket over an OpenSSL secure connection.
`OpenSSLTCPListener` is constructed the following way:
```
OpenSSLTCPListener(short port, const std::string &host = "127.0.0.1",
                   const std::string &certificate, const std::string &key);
```
If no `certificate` or `key` is provided, these will be automatically searched for in a "certificates" directory at run-time.

### Example server code
Here's the code for a simple server that listens on port 4242, prints messages from clients on stdout and sends "Hello there!" as a reply.
You can test this server with "nc 127.0.0.1 4242": anything you type should be echoed server-side, and nc should print "Hello there!" back.
```
Th#include <iostream>
#include "TCPListener.hpp"

int main()
{
  // Listen on port 4242
  putils::TCPListener server(4242);

  // Observe server for new clients
  server.addObserver([&server]()
      {
      auto &client = server.getLastClient();
      // Capture client's fd as the client reference may be invalidated
      int fd = client.fd;

      // Observe client for new messages
      client.addObserver([&server, fd]()
	  {
	  // Get message from client
	  std::cout << server.getClient(fd).getBuff() << std::endl;
	  // Send message to client
	  server.send(fd, "Hello there!");
	  });
      });

  while (true)
    server.select();
}
```

## Connections
The "Connection" family class is an abstraction to client sockets.

### ITCPConnection
Basic interface for the Connection classes.

ITCPConnection allows for two operations:

##### `send(const std::string &msg)`, `send(const char *data, size_t length)`
Send `msg` to the server, or `length` bytes from `data`.

##### `std::string receive()`, `void receive(char *dest, size_t length)`
Read a string from the server, or up to `length` bytes into `dest`.

### ATCPConnection
"Typical" implementation of the core part of of ITCPConnection. This allows future implementations to only specify how to perform very low level operations. The only pure functions left to implement are:

##### `doRead(int fd, char *dest, size_t length)`
Perform a read operation, reading up to `length` bytes into `dest`

##### `doWrite(int fd, const char *data, size_t length)`
Perform a write operation, writing `length` bytes from `data`

### TCPConnection
A final implementation of a client socket.
`TCPConnection` is constructed the following way:
```
TCPConnection(const std::string &host, short port);
```

### OpenSSLTCPConnection
A final implementation of a listening socket over an OpenSSL secure connection.
`OpenSSLTCPConnection` is constructed the following way:
```
OpenSSLTCPConnection(const std::string &host, short port,
                     const std::string &certificate, const std::string &key);
```
If no `certificate` or `key` is provided, these will be automatically searched for in a "certificates" directory at run-time.

### Example client code
Here's the code for a simple client that connects to localhost on port 4242, sends messages entered on stdin and prints replies from the server.
You can test this client with "nc -l -p 4242": anything you type in the client should be echoed by nc, and vice versa.
```
#include <iostream>
#include "TCPConnection.hpp"

int main()
{
  // Connect to localhost on port 4242
  putils::TCPConnection client("127.0.0.1", 4242);

  while (true)
  {
    std::string buff;
    std::getline(std::cin, buff);
    // Send input to server
    client.send(buff);
    // Print reply from server
    std::cout << client.receive() << std::endl;
  }
}
```
