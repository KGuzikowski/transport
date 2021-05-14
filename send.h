#include <arpa/inet.h>

int send_data(
    int size,
    int curr_start,
    int default_len,
    int sockfd,
    struct sockaddr_in *address,
    size_t addr_size
);