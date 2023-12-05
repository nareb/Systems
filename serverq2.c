/*
 * server.c -- TCP Socket Server
 * 
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 * 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>


#define BUFFER_SIZE 8196
#define ROOT_FOLDER "/home/mbinii_sha/Practicum"
#define PORT 2000


//void handle_write_command(int client_sock, const char *local_path, const char *remote_path) {
void handle_write_command(int client_sock, const char *client_message,  const char *local_path, const char *remote_path) {


    // Create a non-const copy of client_message
    char message_copy[BUFFER_SIZE];
    strcpy(message_copy, client_message);


    // Extract local and remote paths from the client message
    char *local_path_token = strtok(message_copy + 6, " ");
    char *remote_path_token = strtok(NULL, " ");


    // Ensure that the paths are not NULL before using them
    if (local_path_token == NULL || remote_path_token == NULL) {
        fprintf(stderr, "Invalid WRITE command format\n");
        return;
    }


    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', sizeof(buffer));


    // Construct the full path to the file on the server
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_FOLDER, remote_path_token);
    //snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_FOLDER, local_path);

    //FILE *file = fopen(remote_path, "w");
    FILE *file = fopen(full_path, "a");
    if (file == NULL) {
        perror("Error opening file on server");
        return;
    }

    ssize_t bytes_received;
    while ((bytes_received = recv(client_sock, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
        memset(buffer, '\0', sizeof(buffer));
    }

    fclose(file);
    //printf("File received and saved on the server: %s\n", remote_path);
    printf("File received and saved on the server: %s\n", full_path);
}


// Function to handle the GET command
void handle_get_command(int client_sock, const char *remote_path, const char *local_path) {
    // Construct the full path to the file on the server
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_FOLDER, remote_path);


    FILE *file = fopen(full_path, "r");
    if (file == NULL) {
        perror("Error opening file on server");
        return;
    }



    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;


    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(client_sock, buffer, bytes_read, 0) < 0) {
            perror("Error sending file data");
            fclose(file);
            return;
        }
    }


    fclose(file);
    printf("File sent to the client: %s\n", local_path);
}


int main(void)
{
    int socket_desc, client_sock;
    socklen_t client_size;
    struct sockaddr_in server_addr, client_addr;
    char server_message[BUFFER_SIZE], client_message[BUFFER_SIZE];

    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
     }


     printf("Socket created successfully\n");


     // Set port and IP:
     server_addr.sin_family = AF_INET;
     server_addr.sin_port = htons(PORT);
     //server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
     server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces


     // Bind to the set port and IP:
     if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("Couldn't bind to the port\n");
        return -1;
     }


      printf("Done with binding\n");


      // Listen for clients:
      if(listen(socket_desc, 1) < 0){
        printf("Error while listening\n");
        return -1;
      }


      printf("\nListening for incoming connections.....\n");



      while (1) {
        // Accept an incoming connection:
        client_size = sizeof(client_addr);
        client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);

        if (client_sock < 0) {
            perror("Can't accept");
            return -1;
        }


        printf("Client connected at IP: %s and port: %i\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));


        // Receive client's message:
        if (recv(client_sock, client_message, sizeof(client_message), 0) < 0) {
            perror("Couldn't receive");
            close(client_sock);
            //printf("Msg from client: %s\n", client_message);
            continue; // Continue to the next iteration to accept other connections
        }

         printf("Msg from client: %s\n", client_message);

        // Respond to client:
        strcpy(server_message, "Hello! I am messenger from your server Q2server.txt bringing you message from server.");

        if (send(client_sock, server_message, strlen(server_message), 0) < 0){
                printf("Can't send\n");
                return -1;
        }


        // Handle different commands here
        if (strncmp(client_message, "GET", 3) == 0) {
            char *remote_path = strtok(client_message + 4, " ");
            char *local_path = strtok(NULL, " ");
            handle_get_command(client_sock, remote_path, local_path);
        } else if (strncmp(client_message, "WRITE", 5) == 0) {
                char *local_path = strtok(client_message + 6, " ");
                char *remote_path = strtok(NULL, " ");
                handle_write_command(client_sock,client_message, local_path, remote_path);
        } else {
                printf("Invalid command is coming from Handle Different command portion\n");
        }

        // Closing the socket:
        close(client_sock);

  }

  close(socket_desc);
  return 0;
}

