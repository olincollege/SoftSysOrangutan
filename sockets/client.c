// C program for the Client Side
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

// inet_addr
#include <arpa/inet.h>
#include <unistd.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>


void start_game(){
	printf("this is a test"); 
}

char* get_username(){
	printf("Please enter your username\n");
	char* username;
	scanf("%s", username);
	return username;
}
// Function to send data to
// server socket.
void* clienthread(void* args)
{
	int network_socket;

	// Create a stream socket
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	// Initialise port number and address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(8888);

	// Initiate a socket connection
	int connection_status = connect(network_socket,(struct sockaddr*)&server_address,
									sizeof(server_address));

	// Check for connection error
	if (connection_status < 0) {
		puts("Error\n");
		return 0;
	}

	printf("Connection established\n");
	char* username = get_username();
	printf("your username is %s\n", username);
	// Send data to the socket
	write(network_socket, &username, sizeof(username));
	sleep(20);
	// Close the connection
	close(network_socket);
	pthread_exit(NULL);

	return 0;
}

// Driver Code
int main()
{

	pthread_t tid;

	// Create thread
	pthread_create(&tid, NULL, clienthread, NULL);
	sleep(20);

	// Suspend execution of
	// calling thread
	pthread_join(tid, NULL);
}
