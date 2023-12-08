// client_handler.h

#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 8196
#define PORT 2000

void send_file_data(int socket_desc, char* local_path, char* remote_path);
void receive_file_data(int socket_desc, char* local_path);
void send_version_command(int socket_desc, const char *remote_path);
void receive_version_info(int socket_desc, const char *remote_path);

#endif  // CLIENT_HANDLER_H
