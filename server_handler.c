// server_handler.c

#include "server_handler.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 8196


// Function to handle the GET command
void handle_get_command(int client_sock, char* remote_path, char* local_path) {

    printf("Handling GET command\n");

    char server_message[BUFFER_SIZE];

    // Attempt to open the requested file
    FILE* file = fopen(remote_path, "r");
    if (file == NULL) {
        perror("Error opening GET file on server");
        return;
    }

    // Respond to client
    sprintf(server_message, "GET %s", remote_path);
    send(client_sock, server_message, strlen(server_message), 0);

    // Send file data to the client
    size_t bytes_read;
    while ((bytes_read = fread(server_message, 1, sizeof(server_message), file)) > 0) {
        if (send(client_sock, server_message, bytes_read, 0)<0) {
                perror("Error sending GET file data");
                fclose(file);
                return;
        }
    }

    fclose(file);

    printf("GET File sent to client: %s\n", local_path);
}


void handle_write_command(int client_sock,  char* local_path, char* remote_path)
{

    printf("Handling WRITE command\n");

    char server_message[BUFFER_SIZE];
    memset(server_message, '\0', sizeof(server_message));

    // Respond to client:
    strcpy(server_message, "Server received WRITE command.");


    if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
        perror("Error sending response to client\n");
        return;
    }


    // Receive file data from the client
    FILE* file = fopen(remote_path, "wb");

    if (file == NULL) {
        perror("Error opening file");
        return;
    }


     // Check if the file already exists
     if (access(remote_path, F_OK) != -1) {
        // File exists, create a new versioned filename
        int version = 1;
        char versioned_path[BUFFER_SIZE];

        do {
            snprintf(versioned_path, sizeof(versioned_path), "%s_%d", remote_path, version);
            version++;
        } while (access(versioned_path, F_OK) != -1);

        fclose(file);  // Close the original file
        file = fopen(versioned_path, "wb");
        printf("Saving as version: %s\n", versioned_path);
      } else {

         // File doesn't exist, create a new one
         file = fopen(remote_path, "wb");
         printf("Saving as: %s\n", remote_path);
       }

     if (file == NULL) {
        perror("Error opening WRITE file");
        return;
    }


    //Receive and save file content
    char client_message[BUFFER_SIZE];
    ssize_t bytes_received;


    while ((bytes_received = recv(client_sock, client_message, sizeof(client_message), 0))>0){

        fwrite(client_message, 1, bytes_received, file);

      }

    
    fclose(file);


    printf("WRITE Function in Server:  File received and saved as: %s\n", remote_path);
    }
        /*else {
        // Invalid WRITE command format
        printf("Invalid WRITE command format\n");


        // Respond to the client about the error
        //char server_message[BUFFER_SIZE];
       // strcpy(server_message, "Server encountered an error in the WRITE command format.");
       // if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
       //     printf("Can't send response to client\n");
       // }
    }
    */



// Function to handle the RM command
void handle_delete_command(int client_sock, char *remote_path) {
    printf("Attempting to delete: %s\n", remote_path);


    // Construct the full path to the file on the server
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s", remote_path);


    if (remove(full_path) == 0) {
        // Deletion successful
        send(client_sock, "DELETE SUCCESSFUL", sizeof("DELETE SUCCESSFUL"), 0);
        printf("File or folder deleted on the server: %s\n", full_path);

    } else {
        // Deletion failed
        perror("Error deleting file or folder");
        send(client_sock, "DELETE ERROR", sizeof("DELETE ERROR"), 0);
    }
}
