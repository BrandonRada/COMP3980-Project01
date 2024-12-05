//
// Created by jonathan on 12/4/24.
//

#include "../include/PearToPear.h"

#define PORT 8080
#define BUFSIZE 1024
#define PEER_ADDR "192.168.0.149"
// #define PEER_ADDR "192.168.0.79"
#define FIVE 5
#define SIX 6
#define SEVEN 7
#define ATE 8

// #define NINE 9
// #define TEN 10

int create_socket(void)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        mvprintw(4, 1, "Socket creation failed");
        exit(EXIT_FAILURE);
    }
    mvprintw(4, 1, "Socket created!");
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
        mvprintw(FIVE, 1, "Bind failed");
        exit(EXIT_FAILURE);
    }

    // Get the assigned port number
    addr_len = sizeof(*my_addr);
    if(getsockname(sock, (struct sockaddr *)my_addr, &addr_len) == -1)
    {
        mvprintw(FIVE, 1, "getsockname failed");
        exit(EXIT_FAILURE);
    }

    mvprintw(FIVE, 1, "Socket bound to address: %s, port: %u", inet_ntoa(my_addr->sin_addr), ntohs(my_addr->sin_port));
}

void configure_peer_addr(struct sockaddr_in *peer_addr)
{
    memset(peer_addr, 0, sizeof(*peer_addr));
    peer_addr->sin_family = AF_INET;
    peer_addr->sin_port   = htons(PORT);
    if(inet_pton(AF_INET, PEER_ADDR, &peer_addr->sin_addr) <= 0)
    {
        mvprintw(SIX, 1, "Invalid address / Address not supported");
        exit(EXIT_FAILURE);
    }
    mvprintw(SIX, 1, "Peer address configured to: %s, port: %u", PEER_ADDR, ntohs(peer_addr->sin_port));
}

void receive_message(int sock, char *buffer, struct sockaddr_in *src_addr)
{
    socklen_t src_addr_len = sizeof(*src_addr);
    ssize_t   bytes_read;

    // Initialize src_addr to zero
    memset(src_addr, 0, sizeof(*src_addr));

    mvprintw(SEVEN, 1, "Waiting to receive message...");
    bytes_read = recvfrom(sock, buffer, BUFSIZE, 0, (struct sockaddr *)src_addr, &src_addr_len);
    if(bytes_read < 0)
    {
        mvprintw(SEVEN, 1, "Receive failed");
        return;
    }

    if(bytes_read > 0)
    {
        buffer[bytes_read] = '\0';

        mvprintw(SEVEN, 1, "Message received from %s:%u: %s", inet_ntoa(src_addr->sin_addr), ntohs(src_addr->sin_port), buffer);
    }
}

void send_message(int sock, const char *message, const struct sockaddr_in *peer_addr)
{
    ssize_t sent_bytes;
    mvprintw(ATE, 1, "Sending message: %s", message);
    sent_bytes = sendto(sock, message, strlen(message), 0, (const struct sockaddr *)peer_addr, sizeof(struct sockaddr_in));
    if(sent_bytes < 0)
    {
        mvprintw(ATE, 1, "Message sending failed");
    }
}
