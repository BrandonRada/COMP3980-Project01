//
// Created by jonathan on 12/4/24.
//

#include "../include/PearToPear.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>

#define PORT 8080
#define BUFSIZE 1024
#define PEER_ADDR "127.0.0.1"    // Address of the peer

static void process_message(char *buffer, char *response)
{
    char       *saveptr;
    const char *conversion_type = strtok_r(buffer, ":", &saveptr);
    char       *client_string   = strtok_r(NULL, ":", &saveptr);

    if(conversion_type && client_string)
    {
        printf("Processing Message: %s:%s\n", conversion_type, client_string);
        for(int i = 0; client_string[i]; i++)
        {
            if(strcmp(conversion_type, "upper") == 0)
            {
                //client_string[i] = upper_filter(client_string[i]);
            }
            else if(strcmp(conversion_type, "lower") == 0)
            {
                //client_string[i] = lower_filter(client_string[i]);
            }
            else
            {
                //client_string[i] = null_filter(client_string[i]);
            }
        }
        snprintf(response, BUFSIZE, "%s", client_string);
    }
    else
    {
        snprintf(response, BUFSIZE, "Invalid format.");
    }
    printf("Process message complete: %s\n", response);
}

int create_socket(void)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created!\n");
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
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Get the assigned port number
    addr_len = sizeof(*my_addr);
    if(getsockname(sock, (struct sockaddr *)my_addr, &addr_len) == -1)
    {
        perror("getsockname failed");
        exit(EXIT_FAILURE);
    }

    printf("Socket bound to address: %s, port: %u\n", inet_ntoa(my_addr->sin_addr), ntohs(my_addr->sin_port));
}

void configure_peer_addr(struct sockaddr_in *peer_addr)
{
    memset(peer_addr, 0, sizeof(*peer_addr));
    peer_addr->sin_family = AF_INET;
    peer_addr->sin_port   = htons(PORT);
    if(inet_pton(AF_INET, PEER_ADDR, &peer_addr->sin_addr) <= 0)
    {
        perror("Invalid address / Address not supported");
        exit(EXIT_FAILURE);
    }
    printf("Peer address configured to: %s, port: %u\n", PEER_ADDR, ntohs(peer_addr->sin_port));
}

void read_socket(int sock, char *buffer, const struct sockaddr_in *my_addr)
{
    struct sockaddr_in src_addr;
    socklen_t          src_addr_len = sizeof(src_addr);
    ssize_t            bytes_read;

    // Initialize src_addr to zero
    memset(&src_addr, 0, sizeof(src_addr));

    bytes_read = recvfrom(sock, buffer, BUFSIZE, 0, (struct sockaddr *)&src_addr, &src_addr_len);
    if(bytes_read > 0)
    {
        buffer[bytes_read] = '\0';

        printf("my_addr: %s\n", inet_ntoa(my_addr->sin_addr));
        printf("my_port: %u\n", ntohs(my_addr->sin_port));
        printf("src_addr: %s\n", inet_ntoa(src_addr.sin_addr));
        printf("src_port: %u\n", ntohs(src_addr.sin_port));
        // Ignore the message if it is from the peer's own address
        if(src_addr.sin_addr.s_addr == my_addr->sin_addr.s_addr && src_addr.sin_port == my_addr->sin_port)
        {
            printf("Ignored message from self\n");
            return;
        }
        printf("Message received: %s\n", buffer);
    }
}

void write_socket(int sock, const char *response, const struct sockaddr_in *peer_addr)
{
    ssize_t sent_bytes;
    printf("Sending message: %s\n", response);
    sent_bytes = sendto(sock, response, strlen(response), 0, (const struct sockaddr *)peer_addr, sizeof(struct sockaddr_in));
    if(sent_bytes < 0)
    {
        perror("Message sending failed");
    }
}

int main(void)
{
    int                sock;
    struct sockaddr_in peer_addr;
    struct sockaddr_in my_addr;
    char               buffer[BUFSIZE];
    char               response[BUFSIZE];

    sock = create_socket();
    bind_socket(sock, &my_addr);
    configure_peer_addr(&peer_addr);

    printf("Peer is running...\n");

    while(1)
    {
        snprintf(response, sizeof(response), "%s:%s", "upper", "Hello, Peer");
        write_socket(sock, response, &peer_addr);

        read_socket(sock, buffer, &my_addr);
        if(strlen(buffer) > 0)
        {
            process_message(buffer, response);
        }
        sleep(1);
        printf("\n");
    }
}
