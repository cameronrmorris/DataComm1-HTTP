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
#include <string>
#include <stdio.h>
#include <fstream>

int main( int argc, char *argv[] ) {

  Socket client;
  char request[1024];
  char response[1024];
  std::ifstream file;
  int length;
  std::string path;
  
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
    if( client.Send( (void *)request, strlen( request )) == -1) {

      std::cout << "Failed to send GET" << std::endl;
      client.Close();
      return -1;

    }

    // Read server's response and print
    while( client.Receive( (void*)response, sizeof(response) )) {  
      std::cout << response;
      memset( response, 0, sizeof(response));
    }
  }
  // Perform PUT request
  else if( strcmp( argv[3], "PUT" ) == 0 ) {

    sprintf( request, "PUT /%s\r\n\r\n", argv[4] );

    // Send HTTP PUT request
    if( client.Send( (void *)request, strlen( request )) == -1) {

      std::cout << "Failed to send PUT" << std::endl;
      client.Close();
      return -1;
    }

    // Open file
    file.open( path.c_str(), std::ios::in );

    // File exists
    if( file.is_open() && file.good() ) {

      // Get length of file
      file.seekg(0, file.end);
      length = file.tellg();
      file.seekg(0, file.beg);

      // Send file
      while( length > 0 ) {

	// Size of buffer
	if( length > (int)sizeof(request) ) {
	  file.read( request,sizeof(request));
	  client.Send( (void *)request, sizeof(request));
	  length -= sizeof(request);
	}
	// Less than or equal to buffer
	else {
	  file.read(request, length);
	  client.Send( (void *)request, length);
	  break;
	}
	memset( request, 0, sizeof(request));
	if( file.eof())
	  break;
      }
    }
    // Read server's response and print 
    while( client.Receive( (void*)response, sizeof(response) )) {
      std::cout << response;
      memset( response, 0, sizeof(response));
    }

  }
  // Bad option
  else {
    std::cout << "Invalid option: " << argv[3] << std::endl;
  }
  client.Close();
  
  return 0;

}
