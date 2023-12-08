/*
 * client.c -- TCP Socket Client
 * 
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */


#include "client_handler.h"


int main(int argc, char* argv[]){

    if (argc <3 ||
        (strcmp(argv[1], "GET") != 0 &&
        strcmp(argv[1], "WRITE") != 0
        && strcmp(argv[1], "RM") != 0 &&
        strcmp(argv[1], "LS") != 0)) {
    printf("Usage:\n");
    printf("       %s GET remote-file-path local-file-path\n", argv[0]);
    printf("       %s WRITE local-file-path remote-file-path\n", argv[0]);
    printf("  %s RM remote-file-path\n", argv[0]);
    printf("  %s LS remote-file-path\n", argv[0]);
    return -1;
}
{
  int socket_desc;
  struct sockaddr_in server_addr;
  char server_message[BUFFER_SIZE], client_message[BUFFER_SIZE];


  // Clean buffers:
  memset(server_message,'\0',sizeof(server_message));
  memset(client_message,'\0',sizeof(client_message));


  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);


  if(socket_desc < 0){
    printf("Unable to create socket\n");
    return -1;
  }


  printf("Socket created successfully\n");


  // Set port and IP the same as server-side:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr("10.162.0.2");
  //server_addr.sin_addr.s_addr = inet_addr("SERVER_IP_ADDRESS");  


    //Send connection request to the server:
    if (connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Unable to connect\n");
        return -1;
    }

    printf("Connected with server successfully\n");


    // Send the command to the server
    if (strcmp(argv[1], "RM") == 0) {
        // RM command
        if (strlen(argv[2]) >= BUFFER_SIZE) {
                printf("Error: Filename is too long\n");
                return -1;
        }
        // Remove newline character from argv[2]
        argv[2][strcspn(argv[2], "\n")] = '\0';
        printf("Filename: %s\n", argv[2]);
        sprintf(client_message, "RM %s", argv[2]);


        if (send(socket_desc, client_message, strlen(client_message), 0) < 0) {
                printf("Error sending RM command\n");
                close(socket_desc);
                return -1;
        }


        // Receive the server's response for the "RM" command
        if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0) {
                printf("Error while receiving server's RM response\n");
                close(socket_desc);
                return -1;
        }

        printf("Server's RM response: %s\n", server_message);

    } else if (strcmp(argv[1], "WRITE") == 0) {
        // Send the "WRITE" command and file data to the server
        sprintf(server_message, "WRITE %s %s", argv[2], argv[3]);

        if (send(socket_desc, server_message, strlen(server_message), 0) < 0) {
                perror("Unable to send WRITE message");
                return -1;
    }

        // Send file data to the server
        send_file_data(socket_desc, argv[2], argv[3]);


        // Receive the server's response for the "WRITE" command
        if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0) {
                perror("Error while receiving server's WRITE response\n");
                close(socket_desc);
                return -1;
        }

        printf("Server's WRITE response: %s\n", server_message);


    

    } else if (strcmp(argv[1], "GET") == 0) {
    // Send the "GET" command to the server
    sprintf(server_message, "GET %s %s", argv[2], argv[3]);

    if (send(socket_desc, server_message, strlen(server_message), 0) < 0) {
        printf("Unable to send GET message");
        return -1;
    }

    // Receive file data from the server for the "GET" command
    receive_file_data(socket_desc, argv[3]);

    // Receive the server's response for the "GET" command
    if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0) {
        printf("Error while receiving server's GET response\n");
        close(socket_desc);
        return -1;
    }
    printf("Server's GET response: %s\n", server_message);


    // Add the LS (List Versions) command
    } else if (strcmp(argv[1], "LS") == 0) {
        send_version_command(socket_desc, argv[2]);
        receive_version_info(socket_desc, argv[2]);
    }


    else {
    // Invalid command (should not reach here with the previous validation)
    printf("Invalid command\n");
    return -1;
    }

    // Close the socket:
    close(socket_desc);


     return 0;
  }

}
