#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include "helper1.h"

int main(int argc, char* argv[]) {
	// Open and close first time to over ride all prev stuff
	FILE *fptr = fopen("dns_svr.log", "w");
	fclose(fptr);
	// Server stuff
    int server_sockfd, client_fd, n, re, s;
	u_int8_t buffer[BUFF_SIZE];
	struct addrinfo hints, *res;
    // Create address we're going to listen on (with given port number)
	memset(&hints, 0, sizeof hints);
	struct sockaddr_storage client_addr;
	socklen_t client_addr_size;

    if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(EXIT_FAILURE);
	}

    hints.ai_family = AF_INET;       // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE;     // for bind, listen, accept
    hints.ai_protocol = IPPROTO_TCP; // TCP 

	// node (NULL means any interface), service (port), hints, res
	s = getaddrinfo(NULL, PORT_NUM, &hints, &res);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
	}

    // Create socket
	server_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (server_sockfd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	// Reuse port if possible
	re = TRUE;
	if (setsockopt(server_sockfd , SOL_SOCKET, SO_REUSEADDR, &re, sizeof(int)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	// Bind address to the socket
	if (bind(server_sockfd , res->ai_addr, res->ai_addrlen) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(res);

	// Listen on socket - means we're ready to accept connections,
	// incoming connection requests will be queued
	if (listen(server_sockfd , 10) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
    // Accept a connection - blocks until a connection is ready to be accepted
	// Get back a new file descriptor to communicate on

	while (TRUE) {
		
		client_addr_size = sizeof(client_addr);
		client_fd = accept(server_sockfd, (struct sockaddr*)&client_addr, &client_addr_size);
		if (client_fd < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}

// read all 3 sections 
// answer count is 0 then dont read answer 
// just answer if its AAAA then check if count = 0 then print '\n'

		/* We want to keep reading in case packet comes in the form 
		of packets */
		u_int8_t tmp[BUFF_SIZE]; 
		int buff_index = 0;
		int length_packet = 0; 
		while ((n = read(client_fd, tmp, BUFF_SIZE - 1)) > 0) {
			for (int j = 0; j < n; j++) {
				buffer[buff_index] = tmp[j];
				buff_index++; 
				
			}
			/* To make sure we recieced a packet which has something in it
			 So we can initialise the packet length and keep reading until 
			 we have read the whole packet */
			if (n >= 1) {
				length_packet = buffer[1];
			}
			if (buff_index >= length_packet) {break;}
		}


		// Null-terminate string
		buffer[n] = '\0';
		read_buffer_array(buffer);
		print_packets(buffer,n+1);

		/***************************** Upstream Socket *********************************/
		int upstream_socket, u_n, u_s;
		u_int8_t u_buffer[BUFF_SIZE];
		struct addrinfo u_hints, *u_res, *rp;
		// Create address we're going to listen on (with given port number)
		memset(&u_hints, 0, sizeof u_hints);


		u_hints.ai_family = AF_INET;       // IPv4
		u_hints.ai_socktype = SOCK_STREAM; // TCP

		// node (NULL means any interface)
		u_s = getaddrinfo(argv[1], argv[2], &u_hints, &u_res);
		if (u_s != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(u_s));
			exit(EXIT_FAILURE);
		}

		for (rp = u_res; rp != NULL; rp = rp->ai_next) {
			upstream_socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (upstream_socket == -1)
				continue;

			if (connect(upstream_socket, rp->ai_addr, rp->ai_addrlen) != -1)
				break; // success
		
			close(upstream_socket);
		}

		if (rp == NULL) {
			fprintf(stderr, "client: failed to connect\n");
			exit(EXIT_FAILURE);
		}

		// Writing packet we got in buffer to upstream socket
		u_n = write(upstream_socket, buffer, BUFF_SIZE);
		if (u_n < 0) {
			perror("write");
			exit(EXIT_FAILURE);
		}

		// Once we write, we wait for response from the upstream socket and store in u_buffer
		u_n = read(upstream_socket, u_buffer, BUFF_SIZE - 1); // n is number of charact  ers read
		// ONCE READ THEN WE WRITE BACK TO CLIENT SOCKET 
		if (u_n < 0) {
			perror("write");
			exit(EXIT_FAILURE);
		}

		u_buffer[u_n] = '\0'; 
		read_buffer_array(u_buffer);

		// Writing response packet we got from upstream back to client.
		int res_to_client = write(client_fd, u_buffer, u_n + 1);
		if (res_to_client < 0) {
			perror("write");
			exit(EXIT_FAILURE);
		}
	}
    return 0;
}

