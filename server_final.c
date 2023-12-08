/*
 * server.c -- TCP Socket Server
 * 
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 * 
 * adapted from:
 *   Chatgpt
 * 
 */


/*
 * Import libraries for handling input/output
 * string, sockets and threads
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "server_handler.h"


#define BUFFER_SIZE 8196
#define ROOT_FOLDER "/home/mbinii_sha/Practicum"
#define PORT 2000


int main(void) {

    int socket_desc, client_sock;
    socklen_t client_size;
    struct sockaddr_in server_addr, client_addr;
    char server_message[BUFFER_SIZE], client_message[BUFFER_SIZE];


    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));


    // Create a TCP socket. socket_desc will store the socket descriptor.
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);


    //Check if the socket creation was successful.
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


      // Listen for clients. Set the socket to listen for incoming connections.
      if(listen(socket_desc, 1) < 0){
        printf("Error while listening\n");
        return -1;
      }


      printf("\nListening for incoming connections.....\n");


      // Start an infinite loop to continuously accept incoming connections.
      while (1) {
        /* Accept an incoming connection and obtain a new socket descriptor 
         * (client_sock) for communication with the client.
         */
         client_size = sizeof(client_addr);
         client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);


         //Check if the acceptance of the connection was successful.
         if (client_sock < 0) {
            perror("Can't accept");
           // return -1;
           continue;
        }

         // Create a new thread for each client
        pthread_t thread;
        struct ThreadArgs* thread_args = (struct ThreadArgs*)malloc(sizeof(struct ThreadArgs));
        thread_args->client_sock = client_sock;
        thread_args->client_addr = client_addr;

        if (pthread_create(&thread, NULL, handle_client, (void*)thread_args) != 0) {
            perror("Error creating thread for client");
            close(client_sock);
            free(thread_args);
        } else {
            pthread_detach(thread); // Detach the thread to allow its resources to be released when it terminates
        }
    }

/*
        //Print information about the connected client.
         printf("Client connected at IP: %s and port: %i\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));


        // Receive client's message:
        if (recv(client_sock, client_message, sizeof(client_message), 0) < 0) {
            perror("Couldn't receive client's message");
            close(client_sock);
            continue; // Continue to the next iteration to accept other connections
        }


         printf("Msg from client: %s\n", client_message);
         
        
         // Respond to client:
        //strcpy(server_message, "Hello! I am a server's messenger");

        if (send(client_sock, server_message, strlen(server_message), 0) < 0){
                printf("Can't send\n");
                return -1;
        }

*/

        //Handle different commands here

                // Check for the "WRITE" command
/*        if (strncmp(client_message, "WRITE", 5) == 0) {
            char local_path[BUFFER_SIZE], remote_path[BUFFER_SIZE];
           // sscanf(client_message, "WRITE %s %s", local_path, remote_path);
            handle_write_command(client_sock,  local_path, remote_path);
        }

        // Check for the "GET" command
        else if (strncmp(client_message, "GET", 3) == 0) {
            char local_path[BUFFER_SIZE], remote_path[BUFFER_SIZE];
            //sscanf(client_message, "GET %s %s", remote_path, local_path);
            handle_get_command(client_sock, remote_path, local_path);
        }

        else if (strncmp(client_message, "RM", 2) == 0) {
                char local_path[BUFFER_SIZE], remote_path[BUFFER_SIZE];
                sscanf(client_message + 2, "%s", remote_path);
                handle_delete_command(client_sock, remote_path);
        }

        // Handle unexpected commands or messages
        else {
                printf("Unexpected command: %s\n", client_message);
    }
*/

      /*        
        if (send(client_sock, server_message, strlen(server_message), 0) < 0){
                printf("Can't send\n");
                return -1;
        }


        // Close the client socket
        close(client_sock);
      }
      */

  close(socket_desc);
  return 0;
}
