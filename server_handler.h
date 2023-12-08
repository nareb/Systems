// server_handler.h

#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include <netinet/in.h>

struct ThreadArgs {
    int client_sock;
    struct sockaddr_in client_addr;
};


void handle_get_command(int client_sock, char* remote_path, char* local_path);
void handle_write_command(int client_sock, char* local_path, char* remote_path);
void handle_delete_command(int client_sock, char *remote_path);
void handle_client_requests(int client_sock);
void* handle_client(void* args);

#endif // SERVER_HANDLER_H
