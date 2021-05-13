// Karol Guzikowski - 308864

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include "receive.h"
#include "send.h"

int main(int argc, char **argv) {
    int last_packet_nr = 0;
    int curr_start = 0;
    char *ip_addr_str = argv[1];
    const char *filename = argv[3];
    int port = atoi(argv[2]);
    int size = atoi(argv[4]);
    struct in_addr ip_addr;
    int sockfd;
    struct sockaddr_in address;
    int default_len = 1000;
    const int packets_no = (size % default_len == 0 ? 0 : 1) + (size / default_len);
    size_t addr_size = sizeof(address);


    if (
        (argc != 5) ||
        (size < 0) ||
        (inet_pton(AF_INET, ip_addr_str, &ip_addr) != 1) ||
        (port <= 0 || port > 65535)
    ) {
        fprintf(stderr, "Wrong arguments! Please provide correct ones.\n");
        return EXIT_FAILURE;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sockfd < 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // Just like we were shown at the lecture
    bzero(&address, addr_size);

    address.sin_addr = ip_addr;
    address.sin_family = AF_INET;
    // https://jameshfisher.com/2016/12/21/htons/
    address.sin_port = htons(port);


    if (
        connect(
            sockfd,
            (struct sockaddr*) &address,
            addr_size
        )
    ) {
        fprintf(stderr, "%s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    while(last_packet_nr < packets_no)
        if (
            receive(
                filename,
                ip_addr_str,
                port,
                default_len,
                sockfd, packets_no,
                &last_packet_nr,
                &curr_start
            ) < 0 ||
            send_data(
                size,
                curr_start,
                &default_len,
                sockfd,
                &address,
                addr_size
            ) < 0
        ) {
            fprintf(stderr, "%s\n", strerror(errno));
            return EXIT_FAILURE;
        }

    close(sockfd);
    return EXIT_SUCCESS;
}