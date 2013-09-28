/*****************************************************************************/
/* Programmer: Cameron Morris                                                */
/*                                                                           */
/* Program Assignment: HTTP Client/Server                                    */
/*                                                                           */
/* File: socket.hpp                                                          */
/*                                                                           */
/* Description: Contains the class for a tcp socket using unix api           */
/*                                                                           */
/*                                                                           */
/* Last updated: September 28, 2013 1:00 PM                                  */
/*****************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>

enum SocketStates {

  NOTREADY,
  CONNECTED,
  LISTENING,

};

class Socket {

public:

  // Creates socket on any port
  Socket();

  // Creates socket on specified port
  Socket(unsigned short port);
  
  // Connect to host on port
  int Connect( char ip[16], unsigned short port );

  // Listens for connections
  int Listen( int backlog = 5);

  // Accept any pending connection
  int Accept( struct sockaddr *remoteAddress );
  
  // Send data over socket

  int Send( const void *data, int size );

  // Receive data over socket

  int Receive( void *data, int size );

  
  // Closes socket and cleans up
  ~Socket();

private:

  // State of the socket
  int state;

  // Descriptor of the socket
  int socketFD;
  
};
