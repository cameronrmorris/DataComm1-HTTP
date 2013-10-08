/*****************************************************************************/
/* Programmer: Cameron Morris                                                */
/*                                                                           */
/* Program Assignment: HTTP Client/Server                                    */
/*                                                                           */
/* File: socket.hpp                                                          */
/*                                                                           */
/* Description: Contains the implementation of the socket class              */
/*                                                                           */
/*                                                                           */
/* Last updated: September 28, 2013 1:00 PM                                  */
/*****************************************************************************/

#include "socket.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>

// Getters/setters
void Socket::setState( int state ) {

  this->state = state;

}

int Socket::getState() {

  return this->state;

}

void Socket::setSocketFD( int socketFD ) {

  this->socketFD = socketFD;

}

int Socket::getSocketFD() {

  return this->socketFD;

}

// Creates non-listening socket
Socket::Socket() {

  setState(NOTREADY);
  setSocketFD(-1);
}

// Creates listening socket
Socket::Socket( const char* port ) {

  BindPort( port );
 
  setState(LISTENING);
  setSocketFD(-1);

}

// Connects to specified host on port
int Socket::Connect( char ip[16], const char* port ) {

  int status;
  int newFD;
  struct addrinfo hints, *result, *p;
  
  if( getState() == LISTENING ) {
    
    std::cerr << "Cannot connect if listening!" << std::endl;
    return -1;
    
  }

  // Specify socket information of remote
  memset(&hints, 0, sizeof( hints ));
  hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // use TCP

  // Get socket information from OS
  if( (status = getaddrinfo( ip, port, &hints, &result )) != 0 ) {

    std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
    exit(1);

  }
  
  // Try to connect to all results
  for( p = result; p != NULL; p = p->ai_next ) {

    // Open socket if not already
    if( getState() != BOUND ) {

      newFD = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
      
      if( newFD == -1 ) {
	std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
	exit(-1);
      }
      
      setSocketFD(newFD);
    
    }
    // Connect to host
    status = connect( getSocketFD(), p->ai_addr, p->ai_addrlen );  
    if( status == -1 ) {

      // Fail on last result
      if( p->ai_next == NULL ) {
	std::cerr << "Connected failed: " << strerror(errno) << std::endl;
	exit(-1);
      }
      Close();

     	
    }

  }
  freeaddrinfo( result );

  setState(CONNECTED);

  return status;


}

// Binds the port of the socket
int Socket::BindPort( const char* port ) {

  int status;
  int newFD;
  struct addrinfo hints, *result;

  if( getState() == LISTENING ) {

    std::cerr << "Already listening on a port" << std::endl;
    return -1;

  }

  if( getState() == CONNECTED ) {

    std::cerr << "Already connected. Can't bind port" << std::endl;
    
  }

  // Specify socket information
  memset( &hints, 0, sizeof( hints ) );
  hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // use TCP
  hints.ai_flags = AI_PASSIVE;     // fill in IP

  // Get socket information from OS
  if( (status = getaddrinfo( NULL, port, &hints, &result )) != 0 ) {

    std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
    exit(1);

  }

  // Open socket
  newFD = socket( result->ai_family, result->ai_socktype, result->ai_protocol );

  if( newFD == -1 ) {
    std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
    exit(-1); 
  }

  setSocketFD(newFD); 

  // Bind port on OS
  status = bind( getSocketFD(), result->ai_addr, result->ai_addrlen );
  if( status == -1 ) {
    std::cerr << "Failed to bind socket: " <<  strerror(errno) << std::endl;
    exit(-1);
  }
  freeaddrinfo( result );

  setState(BOUND);
  
  return status;
  

}

// Listens for a connection on the socket
int Socket::Listen( int backlog ) {

  int status;

  // Set this socket to listen
  if( getState() == BOUND ) {

    setState(LISTENING);

  }

  if( getState() != LISTENING ) {

    std::cerr << "Cannot listen if socket not in listening mode" << std::endl;
    return -1;

  }

  status = listen( getSocketFD(), backlog );

  if( status == -1 ) {
    std::cerr << "Failed to listen: " << strerror(errno) << std::endl;
    exit(-1);
  }

  return status;

}

// Accepts any pending connections and returns the FD
Socket *Socket::Accept( struct sockaddr_storage *remoteAddress ) {

  Socket* newSocket = new Socket();
  socklen_t addrsize = sizeof( *remoteAddress );
  int newFD;


  if( getState() != LISTENING ) {
    
    std::cerr << "Cannot accept if not listening" << std::endl;
    return NULL;
  }
  
  newFD = accept( getSocketFD(), (struct sockaddr *)remoteAddress, &addrsize);
  if( newFD == -1 ) {
    std::cerr << "Failed to accept: " << strerror(errno) << std::endl;
    exit(-1);
  }
  
  newSocket->setSocketFD(newFD);
  newSocket->setState(CONNECTED);

  return newSocket;

}


// Sends data over the socket of specified size
int Socket::Send( const void *data, int size ) {

  int total_sent = 0;
  int bytes_left = size;
  int n;

  if( getState() != CONNECTED ) {

    std::cerr << "Socket not ready to send!" << std::endl;
    return -1;

  }

  // Keep sending until all data is transferred
  while ( total_sent < size ) {

    n = send( getSocketFD(), (char *)data+total_sent, bytes_left, 0 );
    
    // Something bad
    if( n == -1 ) {
      std::cerr << "Failed to send : " <<  strerror(errno) << std::endl;
      break;
    }

    total_sent += n;
    bytes_left -= n;

  }

  if( n == -1 )
    return -1;


  return total_sent;
  
}

// Receives data over the socket in chunks of specified size
int Socket::Receive( void *data, int size ) {
  
  int bytes;

  if( getState() != CONNECTED ) {

    std::cerr << "Socket not ready to receive!" << std::endl;
    return -1;

  }
  bytes = recv( getSocketFD(), data, size, 0 );
  
  if( bytes == -1 ) {
    std::cerr << "Failed to receieve: " << strerror(errno) << std::endl;
    exit(-1);
  }

  return bytes;
}

int Socket::Close() {

  int status;

  // Already closed in the kernel
  if( getSocketFD() == -1 )
    return 0;
  
  status = close(getSocketFD());

  if( status == -1 ) {

    std::cerr << "Failed to close socket: " << strerror(errno) << std::endl;
    exit(-1);

  }

  setState(NOTREADY);
  setSocketFD(-1);

  return status;
}


// Closes the socket and destroys
Socket::~Socket() {

  Close();

}

// Interprets if an IP is v4 or v6
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
