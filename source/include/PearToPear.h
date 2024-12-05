#ifndef MAIN_PEARTOPEAR_H
#define MAIN_PEARTOPEAR_H

#include <SDL2/SDL.h>
#include <arpa/inet.h>
#include <curses.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>

int  create_socket(struct addrinfo *hints);
void bind_socket(int sock, struct addrinfo *hints);
void configure_peer_addr(struct addrinfo **peer_info, const char *peer_addr);
void receive_message(int sock, char *buffer, struct sockaddr_storage *src_addr);
void send_message(int sock, const char *message, const struct addrinfo *peer_info);

#endif    // MAIN_PEARTOPEAR_H
