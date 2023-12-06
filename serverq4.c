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
#include <pthread.h>


#define BUFFER_SIZE 8196
#define ROOT_FOLDER "/home/mbinii_sha/Practicum"
#define PORT 2000


// Function prototypes
void handle_get_command(int client_sock, const char *remote_path, const char *local_path);
void handle_write_command(int client_sock, const char *local_path, const char *remote_path);
void handle_delete_command(int client_sock, const char *remote_path);


// Function to handle a client in a separate thread
void *handle_client(void *arg) {
    int client_sock = *((int *)arg);

    // Receive client's message and handle different commands
    char client_message[BUFFER_SIZE];
    memset(client_message, '\0', sizeof(client_message));

    printf("Msg from client: %s\n", client_message);

    if (recv(client_sock, client_message, sizeof(client_message), 0) < 0) {
        perror("Couldn't receive client's message");
        close(client_sock);
        pthread_exit(NULL);
    }

    // Handle different commands here
    if (strncmp(client_message, "GET", 3) == 0) {
        char *remote_path = strtok(client_message + 4, " ");
        char *local_path = strtok(NULL, " ");
        handle_get_command(client_sock, remote_path, local_path);
    } else if (strncmp(client_message, "WRITE", 5) == 0) {
        char *local_path = strtok(client_message + 6, " ");
        char *remote_path = strtok(NULL, " ");
        handle_write_command(client_sock, local_path, remote_path);
    } else if (strncmp(client_message, "RM", 2) == 0) {
        char *remote_path = strtok(client_message + 3, " ");
        handle_delete_command(client_sock, remote_path);
    } else {
        printf("Invalid command is coming from Server's Handle Different command portion\n");
    }

    // Close the client socket when done
    close(client_sock);

    pthread_exit(NULL);
}


void handle_write_command(int client_sock, const char *local_path, const char *remote_path) {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', sizeof(buffer));


    // Construct the full path to the file on the server
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s", remote_path);


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

// Function to handle the RM command
void handle_delete_command(int client_sock, const char *remote_path) {
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
        send(client_sock, "DELETE ERROR", sizeof("DELETE ERROR"), 0);
        perror("Error deleting file or folder");
    }
}



int main(void) {
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


        // Create a new thread to handle the client
        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, (void *)&client_sock) != 0) {
            perror("Error creating thread");
            close(client_sock);
            continue; // Continue to the next iteration to accept other connections
        }


        // Detach the thread so that it cleans up resources automatically
        pthread_detach(tid);


        // Receive client's message:
        if (recv(client_sock, client_message, sizeof(client_message), 0) < 0) {
            perror("Couldn't receive client's message");
            close(client_sock);
            continue; // Continue to the next iteration to accept other connections
        }

         printf("Msg from client: %s\n", client_message);
      }

  close(socket_desc);
  return 0;
}




