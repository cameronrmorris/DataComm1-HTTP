B/*****************************************************************************/
/* Programmer: Cameron Morris                                                */
/*                                                                           */
/* Program Assignment: HTTP Client/Server                                    */
/*                                                                           */
/* File: socket.hpp                                                          */
/0;115;0c*                                                                           */
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

}

// Creates listening socket
Socket::Socket( const char* port ) {

  BindPort( port );
 
  setState(LISTENING);

}

// Connects to specified host on port
int Socket::Connect( char ip[16], const char* port ) {

  int status;
  struct addrinfo hints, *result;
  
  if( getState() == LISTENING ) {
    
    std::cerr << "Cannot connect if listening!" << std::endl;
    return -1;
    
  }

  // Specify socket information of remote
  memset(&hints, 0, sizeof( hints ));
  hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // use TCP

  // Get socket information from OS
  getaddrinfo( ip, port, &hints, &result);
  
  // Open socket if not already

  if( getState() != BOUND ) {

    setSocketFD( socket( result->ai_family, result->ai_socktype, result->ai_protocol ) );

  }
  // Connect to host
  status = connect( getSocketFD(), result->ai_addr, result->ai_addrlen );
  
  freeaddrinfo( result );

  setState(CONNECTED);

  return status;


}

int Socket::BindPort( const char* port ) {

  int status;
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
  setSocketFD( socket( result->ai_family, result->ai_socktype, result->ai_protocol ));

  // Bind port on OS
  status = bind( getSocketFD(), result->ai_addr, result->ai_addrlen );

  freeaddrinfo( result );

  setState(BOUND);
  
  return status;
  

}

// Listens for a connection on the socket
int Socket::Listen( int backlog ) {

  // Set this socket to listen
  if( getState() == BOUND ) {

    setState(LISTENING);

  }

  if( getState() != LISTENING ) {

    std::cerr << "Cannot listen if socket not in listening mode" << std::endl;
    return -1;

  }

  return listen( getSocketFD(), backlog );

}

int Socket::Accept( struct sockaddr *remoteAddress ) {

  socklen_t addrsize = sizeof( *remoteAddress );

  return accept( getSocketFD(), remoteAddress, &addrsize);

}


// Sends data over the socket of specified size
int Socket::Send( const void *data, int size ) {

  int total_sent = 0;
  int bytes_left = size;
  int n;

  if( getState() != CONNECTED || getState() != LISTENING ) {

    std::cerr << "Socket not ready to send!" << std::endl;
    return -1;

  }

  // Keep sending until all data is transferred
  while ( total_sent < size ) {

    n = send( getSocketFD(), (char *)data+total_sent, bytes_left, 0 );
    
    // Something bad
    if( n == -1 ) {
      break;
    }

    total_sent += n;
    bytes_left -= n;

  }

  if( n == -1 )
    return -1;


  return total_sent;
  
}

int Socket::Receive( void *data, int size ) {

  if( getState() != CONNECTED || getState() != LISTENING ) {

    std::cerr << "Socket not ready to receive!" << std::endl;
    return -1;

  }
  return recv( getSocketFD(), data, size, 0 );

}

int Socket::Close() {
  
  setState(NOTREADY);
  
  return close(getSocketFD());

}

Socket::~Socket() {

  Close();

}
