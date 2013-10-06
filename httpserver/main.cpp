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
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>


int main( int argc, char *argv[] ) {

  Socket server;
  Socket* client;
  char buffer[1024];
  struct sockaddr_storage remoteAddr;
  char ip[INET6_ADDRSTRLEN];
  
  if( argc < 2 ) {

    std::cout << "Usage: http_server <port>" << std::endl;
    return -1;

  }

  // Bind socket to specified socket
  if( server.BindPort(argv[1]) == -1 ) {

    std::cout << "Failed to bind on port: " << argv[1] << std::endl;

  }

  // Set socket to listen mode
  if( server.Listen() == -1 ) {

    std::cout << "Failed to listen." << std::endl;

  }

  std::cout << "ServerFD: " << server.getSocketFD() << std::endl;

  // Begin server loop for requests
  while( 1 ) {

    client = server.Accept( &remoteAddr );

    switch( fork() ) {

    //Child
    case 0:
      
      inet_ntop(remoteAddr.ss_family,
		get_in_addr((struct sockaddr *)&remoteAddr),
		ip, sizeof ip);
      std::cout << "got connection from " <<  ip << std::endl;
      
      // Client doesn't need listener
      server.Close();

      // Receive the client request
      client->Receive( (void *)buffer, sizeof(buffer));
      client->Send( (void *)buffer, sizeof(buffer));
      client->Close();
      exit(0);
      break;
    default:
      // Server doesn't need client
      client->Close();
      break;
    }
  }


  return 0;

}
