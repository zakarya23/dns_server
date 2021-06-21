#ifndef BUFFERH 
#define BUFFERH
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <assert.h>

void handle_respone(int size, u_int8_t c, u_int8_t input[], FILE *fptr); 
void handle_request(int size, u_int8_t c, u_int8_t input[], FILE *fptr, char MY_TIME[]);
void read_buffer(u_int8_t buffer);
void read_buffer_array(u_int8_t buffer[]);

#endif
