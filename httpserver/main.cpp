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
#include <fstream>
#include <string>
#include <sys/wait.h>
#include <signal.h>

void sig_handler(int n) {

  while( waitpid(-1, NULL, WNOHANG) > 0);

}


int main( int argc, char *argv[] ) {

  Socket server;
  Socket* client;
  char buffer[1024];
  struct sockaddr_storage remoteAddr;
  char ip[INET6_ADDRSTRLEN];
  std::fstream file;
  std::string path, buffertemp, tmpstr;
  char response[1024];
  int length, bytes, pos;
  struct sigaction sa;

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

  // Setup signal handler
  sa.sa_handler = sig_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if( sigaction(SIGCHLD, &sa, NULL ) == -1 ) {

    perror("sigaction");
    exit(-1);

  }

  
  std::cout << "Listening on : " << argv[1] << std::endl;

  // Begin server loop for requests
  while( 1 ) {

    client = server.Accept( &remoteAddr );

    switch( fork() ) {

    //Child
    case 0:

      // Print out remote address
      inet_ntop(remoteAddr.ss_family,
		get_in_addr((struct sockaddr *)&remoteAddr),
		ip, sizeof ip);
      std::cout << "got connection from " <<  ip << std::endl;
      
      // Client doesn't need listener
      server.Close();

      // Receive the client request
      bytes = client->Receive( (void *)buffer, sizeof(buffer));

      // Make a copy of the buffer
      buffertemp = buffer;

      // Get the type of the request
      pos = buffertemp.find(" ");
      tmpstr = buffertemp.substr(0,pos);
      buffertemp = buffertemp.substr(pos+1,std::string::npos);

      // Handle GET request
      if( tmpstr == "GET" ) { 
	
	// Get the name of the file requested
	pos = buffertemp.find(" ");
	tmpstr = buffertemp.substr(0,pos);
	path += ".";
	path += tmpstr;
	
	// Open file
	file.open(path.c_str(),std::ios::in);
	
	// File exists, send the response
	if( file.is_open() && file.good() ) {

	  strcpy(response,"HTTP/1.0 200 OK\r\n\r\n");
	  std::cout << "200 OK " << path << std::endl;
	  // Send status code
	  client->Send( (void *)response, strlen(response));

	  memset( buffer, 0, sizeof(buffer));

	  // Get length of file
	  file.seekg(0, file.end);
	  length = file.tellg();
	  file.seekg(0, file.beg);

	  // Send file
	  while( length > 0 ) {

	    // Size of buffer
	    if( length > (int) sizeof(buffer) ) {
	      file.read(buffer, sizeof(buffer));
	      client->Send( (void *)buffer, sizeof(buffer));
	      length -= sizeof(buffer);
	    }
	    // Less than or equal 1k
	    else {
	      file.read(buffer,length);
	      client->Send( (void *)buffer, length);
	      break;
	    }   
	    memset( buffer, 0, sizeof(buffer));

	    // Finished with file
	    if( file.eof() ) 
	      break;
	  }
	}
	// File doesn't exist
	else {
	  
	  std::cout << "404 Not Found " << path << std::endl;

	  strcpy(response,"HTTP/1.0 404 Not Found\r\n\r\n");
	  client->Send( (void *)response, strlen(response));
	  memset( buffer, 0, sizeof(buffer));
	  strcpy(buffer,"<html><head><title>404 Not Found</title></head><body>404 Not Found</body></html>");
	  client->Send( (void*)buffer, strlen(buffer));

	}

      }
      // Handle PUT request
      else if ( tmpstr == "PUT" ) {

	// Read filename 
        pos = buffertemp.find(" ");
        tmpstr = buffertemp.substr(0,pos);
        path += ".";
        path += tmpstr;

	// Sanitize path name
	path = path.substr(0, path.find("\r\n\r\n"));

	std::cout << "Creating file " << path << std::endl;
	
	// Open the file
	file.open( path.c_str(), std::ios::out );

	// Extract file data from initial receive request

	pos = buffertemp.find("\r\n\r\n")+4;

	// Write initial data to file
	file.write(&buffer[pos+4], bytes-(pos+4)); 

	memset( buffer, 0, sizeof(buffer));
	
	// Receive any remaining data
	while( (bytes = client->Receive( (void *)buffer, sizeof(buffer))) ) {

	  file.write(buffer,bytes);
	  memset( buffer, 0, sizeof(buffer));

	}

	// Close & write file
	file.close();

	// Respond to client
	strcpy(response,"HTTP/1.0 200 Ok File Created\r\n\r\n");
	client->Send( (void*)response, strlen(response));
	std::cout << "200 Ok File Created" << std::endl;

      }
      // Invalid request
      else {

	  std::cout << "400 Bad Request " << path << std::endl;
	  strcpy(response,"HTTP/1.0 400 Bad Request\r\n\r\n");
	  client->Send( (void *)response, strlen(response));
	  memset( buffer, 0, sizeof(buffer));
	  strcpy(buffer,"<html><head><title>400 Bad Request</title></head><body>400 Bad Request</body></html>");
	  client->Send( (void*)buffer, strlen(buffer));
      }
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
