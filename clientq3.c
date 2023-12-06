/*
 * client.c -- TCP Socket Client
 * 
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */


#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


#define BUFFER_SIZE 8196
#define PORT 2000


// Function to receive and save file content on the client side
void receive_and_save_file(int socket_desc, const char *local_path) {
    FILE *file = fopen(local_path, "w");
    if (file == NULL) {
        perror("Error opening local file for writing");
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;


    // Receive data from the server and write to the local file
    while ((bytes_received = recv(socket_desc, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }


    fclose(file);
    printf("File received and saved locally: %s\n", local_path);
}



int main(int argc, char *argv[]){

  //Validate command-line arguments
  //if (argc != 4) {
  if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s COMMAND local-file-path remote-file-path\n", argv[0]);
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
  //server_addr.sin_addr.s_addr = inet_addr("129.10.122.39"); //linux Khoury IP address
  //server_addr.sin_addr.s_addr = inet_addr("SERVER_IP_ADDRESS");  


  // Set up the command with paths:
  //  snprintf(client_message, sizeof(client_message), "%s %s %s", argv[1], argv[2], argv[3]);


  // Set up the WRITE command with paths:
if (strcmp(argv[1], "WRITE") == 0) {
    if (argc == 4) {
        snprintf(client_message, sizeof(client_message), "WRITE %s %s", argv[2], argv[3]);
    } else {
        fprintf(stderr, "Usage: %s WRITE local-file-path remote-file-path\n", argv[0]);
        return -1;
    }
}

// Set up the GET command with paths:
else if (strcmp(argv[1], "GET") == 0) {
    if (argc == 4 || argc == 5) {
        snprintf(client_message, sizeof(client_message), "GET %s %s", argv[2], (argc == 5) ? argv[4] : argv[3]);
    } else {
        fprintf(stderr, "Usage: %s GET remote-file-path [local-file-path]\n", argv[0]);
        return -1;
    }
}

// Set up the RM command with paths:
else if (strcmp(argv[1], "RM") == 0) {
    if (argc == 3) {
        snprintf(client_message, sizeof(client_message), "RM %s", argv[2]);
    } else {
        fprintf(stderr, "Usage: %s RM remote-file-or-folder\n", argv[0]);
        return -1;
    }
}
else {
    fprintf(stderr, "Invalid command. Supported commands: WRITE, GET, RM\n");
    return -1;
}


//Set up the WrITE and GET command with paths
 // if (argc == 4) {
   // snprintf(client_message, sizeof(client_message), "WRITE %s %s", argv[2], argv[3]);
 // } else {
   //     snprintf(client_message, sizeof(client_message), "GET %s %s", argv[2], argv[3]);
 // }


  // Send connection request to server:
  if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
    printf("Unable to connect\n");
    return -1;
  }

  printf("Connected with server successfully\n");


  // Send the message to server:
  if(send(socket_desc, client_message, strlen(client_message), 0) < 0){
    printf("Unable to send message\n");
    return -1;
  }

  // Receive and save the file content on the client side
    //receive_and_save_file(socket_desc, argv[3]); // Pass the local file path as an argument
  if (strcmp(argv[1], "GET") == 0) {
        receive_and_save_file(socket_desc, (argc == 5) ? argv[4] : argv[3]);
    }


  // Receive the server's response:
  if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
    printf("Error while receiving server's msg\n");
    return -1;
  }

  printf("Server's response: %s\n",server_message);

  //If the command is "WRITE," send the file to the server
  if (strcmp(argv[1], "WRITE") == 0) {
        FILE *file = fopen((argc == 5) ? argv[4] : argv[3], "r");
  //if (strncmp(argv[1], "WRITE", 5) == 0) {
        // Open the local file for reading:
      //  FILE *file = fopen(argv[3], "r");
        if (file == NULL) {
            perror("Error opening local file");
                       close(socket_desc); //added line          
            return -1;
        }

        // Send the command to the server:
      //  if (send(socket_desc, client_message, strlen(client_message), 0) < 0) {
        //    printf("Unable to send message\n");
          //  fclose(file);
           // close(socket_desc);
           // return -1;
       // }

        // Send the file name to the server:
        if (send(socket_desc, (argc == 5) ? argv[4] : argv[3], strlen((argc == 5) ? argv[4] : argv[3]), 0) < 0) {
        //if (send(socket_desc, argv[3], strlen(argv[3]), 0) < 0) {
            printf("Unable to send file name\n");
            fclose(file);
            close(socket_desc);
            return -1;
         }

        // Read and send the file content to the server:
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            if (send(socket_desc, buffer, bytes_read, 0) < 0) {
                perror("Error sending file data");
                fclose(file);
                close(socket_desc);
                return -1;
            }
        }

        fclose(file);
        //printf("File sent to the server: %s\n", argv[3]);
        printf("File sent to the server: %s\n", (argc == 5) ? argv[4] : argv[3]);

  // Close the socket:
  close(socket_desc);

  return 0;
  }
}
}
