# dns_server
Using the concept of sockets, I have made a dns server. 

Which first creates a socket, creates a connection between the client and the server. 
Reads request from the client, sends it to the upstream server and then waits to receive response from the upstream server which is send back to the client socket. 
