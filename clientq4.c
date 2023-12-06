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
#include <pthread.h>


#define BUFFER_SIZE 8196
#define PORT 2000


// Function prototypes
void receive_and_save_file(int socket_desc, const char *local_path);
int write_file_to_server(int socket_desc, const char *local_path, const char *remote_path);
int get_file_from_server(int socket_desc, const char *remote_path, const char *local_path);
int delete_file_on_server(int socket_desc, const char *remote_path);



/*
 *Function to receive and save file content on the client side
 *
 *This function is designed to be called by the client
 *to receive a file sent by the server
 *and save it to the specified local path
 *
 */

void receive_and_save_file(int socket_desc, const char *local_path) {

    //Open the local file for writing   
    FILE *file = fopen(local_path, "w");
    if (file == NULL) {
        perror("Error opening local file for writing");
        return;
    }


    //Declares a buffer to store data received from the server
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    //Receive data from the server and write to the local file
    while ((bytes_received = recv(socket_desc, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    //Closes the file after all data has been received and written
    fclose(file);

    //Prints a message indicating that the file has been received and saved locally
    printf("File received and saved locally: %s\n", local_path);
}


/*
 *Function to send a file to the server
 *
 *This function is responsible for sending a file from the client to the server.
 *Parameters:
 *socket_desc: The socket descriptor for the communication
 *loal_path: The path to the local file on the client machine
 *remote_path: The desired path for the file on the server
 *
 */

int write_file_to_server(int socket_desc, const char *local_path, const char *remote_path) {
    FILE *file = fopen(local_path, "r");
    if (file == NULL) {
        perror("Error opening local file");
        return -1;
    }

    // Send the file name to the server:
    if (send(socket_desc, remote_path, strlen(remote_path), 0) < 0) {
        printf("Unable to send file name\n");
        fclose(file);
        return -1;
    }

    
    // Read and send the file content to the server:
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(socket_desc, buffer, bytes_read, 0) < 0) {
            perror("Error sending file data");
            fclose(file);
            return -1;
        }
    }

    //Close the loal file
    fclose(file);

    //Print a success message
    printf("File sent to the server: %s\n", remote_path);

    return 0;
}


/*
 *Function to request a file from the server
 *
 *
 *
 *
 *
 *
 */
int get_file_from_server(int socket_desc, const char *remote_path, const char *local_path) {
    // Send the GET command to the server:
    if (send(socket_desc, "GET", 3, 0) < 0) {
        printf("Unable to send GET command\n");
        return -1;
    }

    // Send the file path to the server:
    if (send(socket_desc, remote_path, strlen(remote_path), 0) < 0) {
        printf("Unable to send file path\n");
        return -1;
    }


    // Receive and save the file content on the client side
    receive_and_save_file(socket_desc, local_path);

    return 0;
}


// Function to delete a file on the server
int delete_file_on_server(int socket_desc, const char *remote_path) {
    // Send the DELETE command to the server:
    if (send(socket_desc, "DELETE", 6, 0) < 0) {
        printf("Unable to send DELETE command\n");
        return -1;
    }

    // Send the file path to the server:
    if (send(socket_desc, remote_path, strlen(remote_path), 0) < 0) {
        printf("Unable to send file path\n");
        return -1;
    }

    return 0;
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
  //server_addr.sin_addr.s_addr = inet_addr("SERVER_IP_ADDRESS");  


    //Send connection request to the server:
    if (connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Unable to connect\n");
        return -1;
    }

    printf("Connected with server successfully\n");


    // Call functions based on the command
    if (strcmp(argv[1], "WRITE") == 0) {
        if (argc == 4) {
            write_file_to_server(socket_desc, argv[2], argv[3]);
        } else {
            fprintf(stderr, "Usage: %s WRITE local-file-path remote-file-path\n", argv[0]);
            return -1;
        }
    } else if (strcmp(argv[1], "GET") == 0) {
        if (argc == 4 || argc == 5) {
            get_file_from_server(socket_desc, argv[2], (argc == 5) ? argv[4] : argv[3]);
        } else {
            fprintf(stderr, "Usage: %s GET remote-file-path [local-file-path]\n", argv[0]);
              return -1;
        }
    } else if (strcmp(argv[1], "DELETE") == 0) {
        if (argc == 4) {
            delete_file_on_server(socket_desc, argv[2]);
        } else {
            fprintf(stderr, "Usage: %s DELETE remote-file-path\n", argv[0]);
            return -1;
        }
    } else {
        fprintf(stderr, "Invalid command. Supported commands: WRITE, GET, DELETE\n");
        return -1;

    }


    // Send the message to the server:
    if (send(socket_desc, client_message, strlen(client_message), 0) < 0) {
        printf("Unable to send message");
    }


    

    // Receive the server's response:
    if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0) {
        printf("Error while receiving server's msg\n");
        close(socket_desc);
        return -1;
    }

        printf("Server's response: %s\n", server_message);


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



        // Receive the server's response:
        if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
                perror("Error while receiving server's msg");
                close(socket_desc); // Close the socket on error
                return -1;
        }

         printf("Server's response: %s\n",server_message);

        // Close the socket:
        close(socket_desc);


        return 0;
        }

}
                                                                                                          286,1         Bot
        
                                                                                                            
                                                                                                            



   
                                                                                                            
                                                                                                            

                                                                                                                                                                                
                                                                                                            
                                                                                                            
