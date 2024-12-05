#include "../include/PearToPear.h"

#define PORT "8080"
#define BUFSIZE 1024

int create_socket(struct addrinfo *hints)
{
    int sock = socket(hints->ai_family, hints->ai_socktype, hints->ai_protocol);
    if(sock < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    mvprintw(4, 1, "Socket created!");
    return sock;
}

void bind_socket(int sock, struct addrinfo *hints)
{
    struct sockaddr_storage my_addr;
    void                   *addr;
    socklen_t               addr_len = sizeof(my_addr);
    char                    addr_str[INET6_ADDRSTRLEN];
    memset(&my_addr, 0, addr_len);

    if(bind(sock, hints->ai_addr, hints->ai_addrlen) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if(getsockname(sock, (struct sockaddr *)&my_addr, &addr_len) == -1)
    {
        perror("getsockname failed");
        exit(EXIT_FAILURE);
    }

    addr = (hints->ai_family == AF_INET) ? (void *)&((struct sockaddr_in *)&my_addr)->sin_addr : (void *)&((struct sockaddr_in6 *)&my_addr)->sin6_addr;
    inet_ntop(hints->ai_family, addr, addr_str, sizeof(addr_str));
    mvprintw(4, 1, "Socket bound to address: %s, port: %s", addr_str, PORT);
}

void configure_peer_addr(struct addrinfo **peer_info, const char *peer_addr)
{
    struct addrinfo hints;
    int             status;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    status = getaddrinfo(peer_addr, PORT, &hints, peer_info);
    if(status != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    mvprintw(4, 1, "Peer address configured to: %s, port: %s", peer_addr, PORT);
}

void receive_message(int sock, char *buffer, struct sockaddr_storage *src_addr)
{
    ssize_t bytes_read;

    socklen_t src_addr_len = sizeof(*src_addr);

    memset(src_addr, 0, src_addr_len);

    mvprintw(4, 1, "Waiting to receive message...");
    bytes_read = recvfrom(sock, buffer, BUFSIZE, 0, (struct sockaddr *)src_addr, &src_addr_len);
    if(bytes_read < 0)
    {
        perror("Receive failed");
        return;
    }

    if(bytes_read > 0)
    {
        char  addr_str[INET6_ADDRSTRLEN];
        void *addr;
        buffer[bytes_read] = '\0';

        addr = (src_addr->ss_family == AF_INET) ? (void *)&((struct sockaddr_in *)src_addr)->sin_addr : (void *)&((struct sockaddr_in6 *)src_addr)->sin6_addr;
        inet_ntop(src_addr->ss_family, addr, addr_str, sizeof(addr_str));
        mvprintw(4, 1, "Message received from %s:%s: %s", addr_str, PORT, buffer);
    }
}

void send_message(int sock, const char *message, const struct addrinfo *peer_info)
{
    ssize_t sent_bytes;
    mvprintw(4, 1, "Sending message: %s", message);
    sent_bytes = sendto(sock, message, strlen(message), 0, peer_info->ai_addr, peer_info->ai_addrlen);
    if(sent_bytes < 0)
    {
        perror("Message sending failed");
    }
}
