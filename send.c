#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "send.h"


int send_data(
    int size,
    int curr_start,
    int default_len,
    int sockfd,
    struct sockaddr_in *address,
    size_t addr_size
) {
    char msg[20];
    int len = default_len;
    if (len + curr_start > size)
        len = size - curr_start;

    sprintf(msg, "GET %d %d\n", curr_start, len);
    int packet_len = strlen(msg);
    
    if (
        sendto(
            sockfd,
            msg,
            packet_len,
            0,
            (struct sockaddr*) address,
            addr_size
        ) < 0
    ) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}