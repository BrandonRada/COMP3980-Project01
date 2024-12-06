#include "../include/PearToPear.h"

int create_socket(void)
{
    int flags;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        log_msg("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket to non-blocking mode
    flags = fcntl(sock, F_GETFL, 0);
    if(flags < 0 || fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        log_msg("Failed to set non-blocking mode");
        exit(EXIT_FAILURE);
    }

    log_msg("Socket created and set to non-blocking mode!");
    return sock;
}

void bind_socket(int sock, struct sockaddr_in *my_addr)
{
    socklen_t addr_len;
    memset(my_addr, 0, sizeof(*my_addr));
    my_addr->sin_family      = AF_INET;
    my_addr->sin_addr.s_addr = INADDR_ANY;
    my_addr->sin_port        = htons(PORT);

    if(bind(sock, (const struct sockaddr *)my_addr, sizeof(*my_addr)) < 0)
    {
        log_msg("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Get the assigned port number
    addr_len = sizeof(*my_addr);
    if(getsockname(sock, (struct sockaddr *)my_addr, &addr_len) == -1)
    {
        log_msg("getsockname failed");
        exit(EXIT_FAILURE);
    }

    log_msg("Socket bound to address: %s, port: %u", inet_ntoa(my_addr->sin_addr), ntohs(my_addr->sin_port));
}

void configure_peer_addr(struct sockaddr_in *peer_addr, const char *ip_address)
{
    memset(peer_addr, 0, sizeof(*peer_addr));
    peer_addr->sin_family = AF_INET;
    peer_addr->sin_port   = htons(PORT);
    if(inet_pton(AF_INET, ip_address, &peer_addr->sin_addr) <= 0)
    {
        log_msg("Invalid address / Address not supported");
        exit(EXIT_FAILURE);
    }
    log_msg("Peer address configured to: %s, port: %u", ip_address, ntohs(peer_addr->sin_port));
}

int read_socket(int sock, char *buffer, struct sockaddr_in *src_addr, const char *ip_address)
{
    socklen_t src_addr_len = sizeof(*src_addr);
    ssize_t   bytes_read;

    // Initialize src_addr to zero
    memset(src_addr, 0, sizeof(*src_addr));

    // Receive data
    bytes_read = recvfrom(sock, buffer, BUFSIZE, 0, (struct sockaddr *)src_addr, &src_addr_len);
    if(bytes_read > 0)
    {
        buffer[bytes_read] = '\0';

        if(strcmp(inet_ntoa(src_addr->sin_addr), ip_address) == 0)
        {
            log_msg("Message received from %s:%u: %s", inet_ntoa(src_addr->sin_addr), ntohs(src_addr->sin_port), buffer);
            return 0;
        }
    }
    return 1;
}

void write_socket(int sock, const char *message, const struct sockaddr_in *peer_addr)
{
    ssize_t sent_bytes;
    log_msg("Sending message: %s", message);
    sent_bytes = sendto(sock, message, strlen(message), 0, (const struct sockaddr *)peer_addr, sizeof(struct sockaddr_in));
    if(sent_bytes < 0)
    {
        log_msg("Message sending failed");
    }
}
