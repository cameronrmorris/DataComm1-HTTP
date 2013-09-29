/*****************************************************************************/
/* Programmer: Cameron Morris                                                */
/*                                                                           */
/* Program Assignment: HTTP Client                                           */
/*                                                                           */
/* File: client.cpp                                                          */
/*                                                                           */
/* Description: This program is a basic HTTP client that can perform GET and */
/*              PUT requests to a HTTP server.                               */
/*                                                                           */
/* Last updated: September 28, 2013 1:00 PM                                  */
/*****************************************************************************/

#include "../shared/socket.hpp"
#include <iostream>
#include <string.h>
#include <stdio.h>

int main( int argc, char *argv[] ) {

  Socket client;
  char request[1024];
  char response[1024];

  // Handle command line
  if( argc < 5 ) {

    std::cout << "Usage: http_client <host> <port> GET/PUT <filename>" << std::endl;
    return -1;

  }

  // Connect to host
  if( client.Connect( argv[1], argv[2] ) == -1 ) {

    std::cerr << "Failed to connect to: " << argv[1] << ":" << argv[2] << std::endl;

    return -1;
  }

  // Perform get request
  if( strcmp( argv[3], "GET" ) == 0 ) {

    sprintf( request, "GET /%s HTTP/1.0\r\n\r\n", argv[4] );

    // Send HTTP GET request
    client.Send( (void *)request, sizeof( request ));
    
    // Read server's response and print
    while( client.Receive( (void*)response, sizeof(response) )) {  
      std::cout << response;
      memset( response, 0, sizeof(response));
    }
 
  } 
  client.Close();
  
  return 0;

}
