// C program for the Client Side
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>

// inet_addr
#include <arpa/inet.h>
#include <unistd.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include "../maze/maze.h"

int network_socket;

void start_game(){
	printf("this is a test"); 
}

int catch_signal(int sig, void(*handler)(int)){
	struct sigaction action;
	action.sa_handler = handler; 
	sigemptyset(&action.sa_mask); 
	action.sa_flags = 0; 
	return sigaction (sig, &action, NULL); 
}

void handle_shutdown(int sig){
	if(network_socket)
		close(network_socket);
	printf("\nGoodbye!\n");
	exit(0);
}
// Function to send data to
// server socket.
void* clienthread(void* args)
{
	int wait_for_game = 1; 

	// Create a stream socket
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	// Initialise port number and address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	//server_address.sin_addr.s_addr = inet_addr("192.168.35.226");
	server_address.sin_port = htons(8888);

	// Initiate a socket connection
	int connection_status = connect(network_socket,(struct sockaddr*)&server_address,
									sizeof(server_address));

	// Check for connection error
	if (connection_status < 0) {
		puts("Error, unable to connect\n");
		return 0;
	}

	printf("Connection established\n");

	char input[1025]; 
	printf("Please enter your username\n");
	scanf("%s", input);
	printf("your username is %s\n", input);
	
	char flag[] = "username";
	// Send data to the socket
	write(network_socket, &flag, sizeof(flag));
	sleep(1);
	write(network_socket, &input, sizeof(input));

	while(strncmp(input, "ready", 5 != 0)){
		bzero(input, sizeof(input)); 
		printf("Please type 'ready' when all players are ready to start\n");
		scanf("%s", input);
	}

	write(network_socket, &input, sizeof(input));

	char buff[1025];

	while(wait_for_game){
		bzero(buff, sizeof(buff));
		read(network_socket, buff, sizeof(buff));
		printf("From Server : %s", buff);
		if ((strncmp(buff, "start", 5)) == 0) {
			wait_for_game = 0; 
		}
	}
	int time_taken = run_maze();
	
	sleep(20);
	// Close the connection
	close(network_socket);
	pthread_exit(NULL);

	return 0;
}

// Driver Code
int main()
{	

	if(catch_signal(SIGINT, handle_shutdown) == -1){
		perror("Can't set interrupt handler");
	}

	pthread_t tid;

	// Create thread
	pthread_create(&tid, NULL, clienthread, NULL);
	
	sleep(20);


	// Suspend execution of
	// calling thread
	pthread_join(tid, NULL);
}
