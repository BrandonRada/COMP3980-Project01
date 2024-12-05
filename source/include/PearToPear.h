//
// Created by jonathan on 12/4/24.
//

#ifndef MAIN_PEARTOPEAR_H
#define MAIN_PEARTOPEAR_H

/*#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-macro-identifier"
    #pragma clang diagnostic ignored "-Wreserved-identifier"
    #pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop
#ifdef __clang__
    #pragma clang diagnostic pop
#endif*/
#include <arpa/inet.h>
#include <curses.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>

int  create_socket(void);
void bind_socket(int sock, struct sockaddr_in *my_addr);
void configure_peer_addr(struct sockaddr_in *peer_addr, const char *ip_address);
int  receive_message(int sock, char *buffer, struct sockaddr_in *src_addr, const char *ip_address);
void send_message(int sock, const char *message, const struct sockaddr_in *peer_addr);

#endif    // MAIN_PEARTOPEAR_H
