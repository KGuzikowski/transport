#include <stdlib.h>

int file_write(const char* filename, uint8_t *buffer, int bytes);

int receive(
    const char* filename,
    char* ip_addr_str,
    int port,
    int default_len,
    int sockfd,
    int packets_no,
    int *last_packet,
    int *curr_start
);