//
// Created by jonathan on 12/4/24.
//

#ifndef MAIN_PEARTOPEAR_H
#define MAIN_PEARTOPEAR_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <arpa/inet.h>
#include <curses.h>

int create_socket(void);
void bind_socket(int sock, struct sockaddr_in *my_addr);
void configure_peer_addr(struct sockaddr_in *peer_addr);
void receive_message(int sock, char *buffer, struct sockaddr_in *src_addr);
void send_message(int sock, const char *message, const struct sockaddr_in *peer_addr);

#endif    // MAIN_PEARTOPEAR_H
