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
  BOUND,
  CONNECTED,
  LISTENING,

};

class Socket {

public:

  // Creates socket on any port
  Socket();

  // Creates socket on specified port
  Socket( const char* port );
  
  // Bind the socket to a port
  int BindPort( const char* port );

  // Connect to host on port
  int Connect( char ip[16], const char* port );

  // Listens for connections & sets up socket if necessary
  int Listen( int backlog = 5 );

  // Accept any pending connection
  Socket Accept( struct sockaddr_storage *remoteAddress );
  
  // Send data over socket
  int Send( const void *data, int size );

  // Send data over specified socket (Server use)
  int Send( int socketFD, const void *data, int size);

  // Receive data over socket
  int Receive( void *data, int size );

  // Receieve data over specified socket (Server use)
  int Receive( int socketFD, void *data, int size);
  
  // Closes socket
  int Close();

  // Closes socket and cleans up
  ~Socket();

  // Getters/Setters
  int getState();
  void setState( int state );

  int getSocketFD();
  void setSocketFD( int socket );


private:
  
  // State of the socket
  int state;

  // Descriptor of the local socket
  int socketFD;
};
