/*****************************************************************************/
/* Programmer: Cameron Morris                                                */
/*                                                                           */
/* Program Assignment: HTTP Server                                           */
/*                                                                           */
/* File: server.cpp                                                          */
/*                                                                           */
/* Description: This program is a basic HTTP server that can accept GET and  */
/*              PUT requests from a HTTP client and respond.                 */
/*                                                                           */
/* Last updated: September 28, 2013 1:00 PM                                  */
/*****************************************************************************/

#include "../shared/socket.hpp"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

int main( int argc, char *argv[] ) {

  Socket server;
  Socket client;
  char buffer[1024];
  int bytes;
  struct sockaddr_storage remoteAddr;

  if( argc < 2 ) {

    std::cout << "Usage: http_server <port>" << std::endl;
    return -1;

  }

  // Bind socket to specified socket
  server.BindPort(argv[1]);

  // Set socket to listen mode
  server.Listen();

  // Begin server loop for requests
  while( 1 ) {

    client = server.Accept( &remoteAddr );

    switch( fork() ) {

      //Child
    case 0:
      server.Close();
      // Receive the client request
      std::cout << "Received client request" << std::endl;
      while( (bytes = client.Receive( (void *)buffer, sizeof(buffer))) == -1);
      std::cout << "bytes=" << bytes << " = Answering client: " << buffer  << std::endl;
      client.Send( (void *)"test", sizeof(4));
      client.Close();
      exit(0);
      break;
    default:
      client.Close();
      break;
    }
  }


  return 0;

}
