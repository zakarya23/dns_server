#ifndef HELPERH 
#define HELPERH
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <assert.h>
#define PORT_NUM "8053"
#define BUFF_SIZE 256
#define TRUE 1
#define FALSE 0
#include <arpa/inet.h>

void print_packets(u_int8_t buffer[], int size); 
void handle_respone(int size, u_int8_t c, u_int8_t input[], FILE *fptr); 
void handle_request(int size, u_int8_t c, u_int8_t input[], FILE *fptr);
void read_buffer(u_int8_t buffer);
void read_buffer_array(u_int8_t buffer[]);

#endif
