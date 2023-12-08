// server_handler.h

#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

void handle_get_command(int client_sock, char* remote_path, char* local_path);
void handle_write_command(int client_sock, char* local_path, char* remote_path);
void handle_delete_command(int client_sock, char *remote_path);

#endif // SERVER_HANDLER_H
