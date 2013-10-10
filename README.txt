Cameron Morris
UMS01115162

Design: For this assignment I created a C++ class to encapsulate the workings 
of the TCP socket for ease of use and in mind that it would be able to be used 
in further assignments. 


I started with the HTTP client which can perform both GET and PUT requests to 
an HTTP server. It does this by first taking the command line arguments and 
parsing them to determine the appropriate action. If it was a GET request the 
client will connect to the given host and port and send the HTTP request and 
then read the response and print it to the screen. If it was a PUT request it 
will open the file and make sure it exists and then send the PUT request to 
the server with the file data. Overall, I was successful with this without any
known issues. 


The HTTP server I designed also was able to handle both a GET requests and PUT
requests in a concurrent manner. When the server receives a question it will 
fork a child to handle the request and then continue to listen for more 
connections thus supporting multiple connections at any moment. If the client 
is requesting a GET the server will open the file it exists and then respond 
to it using the HTTP format. If a PUT request then it will read from the 
client until the finishes sending the file and then respond after saving the 
file to the disk.


Possible Improvements: The HTTP Client & Server could be improved to be able 
to handle conditional GETs to reduce the amount of transmissions when loading 
the web pages as I have noticed when testing them both. This could be done on 
the client side by adding the appropriate headers to the GET request and have 
a local cache of the objects downloaded. The Server would have to be able to 
interpret the header file and check the last modification on the file and send
 the appropriate response.
