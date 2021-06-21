#include "helper1.h"

void read_buffer_array(u_int8_t buffer[]) {
     // File we are writing to
    FILE *fptr; 
    fptr = fopen("dns_svr.log", "w");

    // For getting the time and printing it to file. 
    time_t current_time; 
    struct tm *tmp; 
    char MY_TIME[50]; 
    time(&current_time); 
    tmp = localtime(&current_time); 
    strftime(MY_TIME, sizeof(MY_TIME), "%FT%T%z", tmp); 
    printf("%s ", MY_TIME);
    fprintf(fptr, "%s ", MY_TIME);

    // For reading in length of the file
    // u_int8_t length[2];
    // size_t nbytes = sizeof(length);

    // Read first 2 lines for length
    // ssize_t total_bytes = read(buffer, length, nbytes);
    u_int8_t length = buffer[1]; 


    // if (total_bytes < 0) {
    //     printf("ERROR\n"); 
    //     exit(EXIT_FAILURE);
    // }
    
    // Store the size in a variable
    int size = (int) length; 

    // Mallocing space to read the rest of the file. 
    u_int8_t input[size];
    // size_t num_bytes = sizeof(input);
    // Reading the rest and storing in input
    // ssize_t total = read(buffer, input, num_bytes);
    for (int i=0; i < size; i++) {
        input[i] = buffer[i + 2];
    }

    // if (total < 0) {
    //     printf("ERROR\n"); 
    //     exit(EXIT_FAILURE);
    // }

    // Count is the first part where we start reading the ASCII
    u_int8_t count = input[12]; 
    int qr = input[2] >> 7; 

    int response = 0; 
    if (qr == 1) {
        response = 1; 
    }

    if (response) {
        handle_respone(size, count, input, fptr);
    }
    else {
        handle_request(size, count, input, fptr, MY_TIME);
    }
    fclose(fptr);
}

void read_buffer(u_int8_t buffer) {
     // File we are writing to
    FILE *fptr; 
    fptr = fopen("dns_svr.log", "w");

    // For getting the time and printing it to file. 
    time_t current_time; 
    struct tm *tmp; 
    char MY_TIME[50]; 
    time(&current_time); 
    tmp = localtime(&current_time); 
    strftime(MY_TIME, sizeof(MY_TIME), "%FT%T%z", tmp); 
    printf("%s ", MY_TIME);
    fprintf(fptr, "%s ", MY_TIME);

    // For reading in length of the file
    u_int8_t length[2];
    size_t nbytes = sizeof(length);

    // Read first 2 lines for length
    ssize_t total_bytes = read(buffer, length, nbytes);

    if (total_bytes < 0) {
        printf("ERROR\n"); 
        exit(EXIT_FAILURE);
    }
    
    // Store the size in a variable
    int size = (int) length[1]; 

    // Mallocing space to read the rest of the file. 
    u_int8_t input[size];
    size_t num_bytes = sizeof(input);
    // Reading the rest and storing in input
    ssize_t total = read(buffer, input, num_bytes);

    if (total < 0) {
        printf("ERROR\n"); 
        exit(EXIT_FAILURE);
    }

    // Count is the first part where we start reading the ASCII
    u_int8_t count = input[12]; 
    int qr = input[2] >> 7; 

    int response = 0; 
    if (qr == 1) {
        response = 1; 
    }

    if (response) {
        handle_respone(size, count, input, fptr);
    }
    else {
        handle_request(size, count, input, fptr, MY_TIME);
    }
    fclose(fptr);
}


void handle_request(int size, u_int8_t c, u_int8_t input[], FILE *fptr, char MY_TIME[]) {

    int count = c; 
    int null_point = 0; 
    int query = 1; 
    fprintf(fptr, "%s ", "requested");
        printf("requested ");
        for (int i = 0 ; i < size; i++) {

            if (i > 12 && query) {
                if (count == 0) {
                    count = input[i]; 
                    if (count == '\0') {
                        // printf(" is at ");
                        // fprintf(fptr, "%s", " is at ");
                        query = 0;
                        null_point = i; 
            
                    }
                    else {
                        printf(".");
                        fprintf(fptr, "%c", '.');
                    }
    
                }
                else if (input[i] == '\t') {
                    printf(".");
                } 
                else{
                    printf("%c", input[i]);
                    fprintf(fptr, "%c", input[i]);
                    count--; 
                }
            }

            if (!query && (i == (null_point + 2))) {
                // printf(" a %d", input[i]);
                if (input[i] != 28) {
                  
                    // printf("");
                    // fptr = fopen("dns_svr.log", "w");
                    fprintf(fptr, "\n");
                    fprintf(fptr, "%s ", MY_TIME);
                    fprintf(fptr, "%s", "unimplemented request");
                    break;
                }
            }
        }
        printf("\n");
}

void handle_respone(int size, u_int8_t c, u_int8_t input[], FILE *fptr) {
    // Variables to help us with identifying stuff in the ip.
    int count = c; 
    int null_point = 0;  //
    int query = 1; //
    int num = 0; //
    int zero = 0; //
    int colon = 0; //
    int ip = 0; //
    for (int i = 0 ; i < size; i++) {

            if (i > 12 && query) {
                
                if (count == 0) {
                    count = input[i]; 
                    if (count == '\0') {
                        printf(" is at ");
                        fprintf(fptr, "%s", " is at ");
                        query = 0;
                        null_point = i; 
            
                    }
                    else {
                        printf(".");
                        fprintf(fptr, "%c", '.');
                    }
    
                }
                else if (input[i] == '\t') {
                    printf(".");
                }
                else{
                    printf("%c", input[i]);
                    fprintf(fptr, "%c", input[i]);
                    count--; 
                }
            }

            if (!query && i == (null_point + 16)) {
                ip = input[i]; 

            }

            if (!query && (i > (null_point + 4)) && (i < (null_point + 6))) {
                int j = i + 1; 
                if ((input[i] != 192) && (input[j] != 12)) {
                    fclose(fptr);
                    fptr = fopen("dns_svr.log", "w");
                    fclose(fptr);
                    break;
                }
            }

            if (!query && i >= (null_point + 17) && ip > 0) {
                num++;
                if (num % 2 == 0) {
                    if (input[i] == 0) {
                        zero++;
                    }

                    if (zero == 2) {
                        colon++; 
                        ip--; 
                        if (colon < 2) {
                            printf(":");
                            fprintf(fptr, "%c", ':');
                        }
                        zero = 0; 
                        continue;
                    }
                    if (input[i] < 10 && zero == 0 ) {
                        ip--; 
                        if (ip > 0) {
                            printf("0%x:", input[i]);
                            fprintf(fptr, "0%x:", input[i]);
                        }
                        else {
                            printf("0%x", input[i]);
                            fprintf(fptr, "0%x", input[i]);
                        }
                    }
                    else if (zero != 2) {
                        ip--; 
                        if (ip > 0) {
                            if (input[i] < 10) {
                                printf("0%x:", input[i]);
                                fprintf(fptr, "0%x:", input[i]);
                            }
                            else {
                                printf("%x:", input[i]);
                                fprintf(fptr, "%x:", input[i]);
                            }
                        }
                        else {
                            printf("%x", input[i]);
                            fprintf(fptr, "%x", input[i]);
                        }
                    }
                
                    zero = 0; 
                    continue;
                }
                if (input[i] == 0) {
                    ip--; 
                    zero++;
                }
            
                else {
                    ip--; 
                    printf("%x", input[i]);
                    fprintf(fptr, "%x", input[i]);
                }
            }
        }
    
        fprintf(fptr, "%s", "\n");
        printf("\n");
}
