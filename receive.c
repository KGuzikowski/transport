#include <stdio.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "receive.h"

#define TIMEOUT 30000

int file_write(const char* filename, uint8_t *buffer, int bytes) { 
    FILE *file = fopen(filename, "a+");
    if (file == NULL) return EXIT_FAILURE;

    size_t written_no = fwrite(buffer, sizeof(uint8_t), bytes, file);

    if (written_no < sizeof(buffer) || fclose(file) == EOF)
        return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}

int receive(
    const char* filename,
    char* ip_addr_str,
    int port,
    int default_len,
    int sockfd,
    int packets_no,
    int *last_packet,
    int *curr_start
) {
    // Code below is very similar to that from the traceroute project.

    // file descriptors set that select will monitor
	fd_set descriptors;
	// zero the mask (file descriptor)
	FD_ZERO(&descriptors);
	// Sets the bit for the file descriptor 'sockfd'
	// in the file descriptor set 'descriptors/.
	FD_SET(sockfd, &descriptors);
    
    struct timeval tv = {0, TIMEOUT};
    char ip_str[20];

    while (*last_packet < packets_no && (tv.tv_sec > 0 || tv.tv_usec > 0)) {
		int ready_fd = select(sockfd + 1, &descriptors, NULL, NULL, &tv);

        if (ready_fd < 0) return EXIT_FAILURE;
		else if (ready_fd == 0) break;

        struct sockaddr_in sender;
		socklen_t sender_len = sizeof(sender);
		uint8_t buffer[IP_MAXPACKET + 1];

        ssize_t packet_len = recvfrom(
			sockfd,
			buffer,
			IP_MAXPACKET,
			MSG_DONTWAIT,
			(struct sockaddr*) &sender,
			&sender_len
		);
		
		if (
            packet_len < 0 ||
            inet_ntop(AF_INET, &(sender.sin_addr), ip_str, sizeof(ip_str)) == NULL
        ) return EXIT_FAILURE;

        // Ignore the packet if it's not for me.
        if (
            strcmp(ip_addr_str, ip_str) != 0 ||
            ntohs(sender.sin_port) != port
        ) continue;

        uint8_t content[default_len + 1];
        
        int data_length, data_start;
        char data[] = "DATA", data_str[5];

        sscanf(
            (char *)buffer,
            "%s %d %d\n",
            data_str,
            &data_start,
            &data_length
        );

        if (
            strcmp(data_str, data) != 0 ||
            data_length > default_len ||
            data_start != *curr_start
        ) return EXIT_FAILURE;

        int i = 0;
        while (buffer[i] != '\n')
            i++;
        i++;

        memmove(content, buffer + i, packet_len);

        *curr_start += default_len;
        *last_packet += 1;

        if (file_write(filename, content, packet_len - i) < 0)
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}