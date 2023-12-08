// client_handler.c

#include "client_handler.h"

void send_file_data(int socket_desc, char* local_path, char* remote_path) {
    char client_message[BUFFER_SIZE];
    FILE* file = fopen(local_path, "rb");

    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    size_t bytes_read;
    while ((bytes_read = fread(client_message, 1, sizeof(client_message), file))>0){
        send(socket_desc, client_message, bytes_read, 0);
    }


    fclose(file);

        printf("WRITE Function: File data sent successfully\n");
}


void receive_file_data(int socket_desc, char* local_path) {
     FILE* file = fopen(local_path, "w");

    if (file == NULL) {
        perror("Error opening GET file");
        return;
    }

    char client_message[BUFFER_SIZE];
    ssize_t bytes_received;

    // Receive data from the server and write to the local file
    while ((bytes_received = recv(socket_desc, client_message, sizeof(client_message), 0)) > 0) {
        fwrite(client_message, 1, bytes_received, file);
    }

    fclose(file);

    printf("GET File received and saved locally as: %s\n", local_path);
}


void send_version_command(int socket_desc, const char *remote_path) {
    char command[BUFFER_SIZE];
    snprintf(command, sizeof(command), "LS %s", remote_path);
    send(socket_desc, command, strlen(command), 0);
}

void receive_version_info(int socket_desc, const char *remote_path) {
    char server_message[BUFFER_SIZE];
    ssize_t bytes_received = recv(socket_desc, server_message, sizeof(server_message), 0);

    if (bytes_received > 0) {
        printf("Versioning information for file '%s':\n%s\n", remote_path, server_message);
    } else {
        printf("Failed to receive versioning information.\n");
    }
}
