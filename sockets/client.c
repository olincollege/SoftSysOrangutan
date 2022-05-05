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

int connect_to_server(struct sockaddr_in* server_address){
	/* Connect to server socket */

	server_address -> sin_family = AF_INET;
	server_address -> sin_addr.s_addr = INADDR_ANY;
	//server_address -> sin_addr.s_addr = inet_addr("192.168.35.226");
	server_address -> sin_port = htons(8888);

	// Initiate a socket connection
	int connection_status = connect(network_socket,(struct sockaddr*)server_address, sizeof(*server_address));

	// Check for connection error
	if (connection_status < 0) {
		puts("Error, unable to connect\n");
		return 0;
	}

	printf("Connection established\n");
	return 1;
}

void get_username(){
	/* Get username from user input */

	char user_input[1025];
	char flag[] = "username";

	printf("Please enter your username\n");
	scanf("%s", user_input);
	printf("your username is %s\n", user_input);
	
	// Send flag to server indicating next message is username
	write(network_socket, &flag, sizeof(flag));
	sleep(1);
	// Send username to server 
	write(network_socket, &user_input, sizeof(user_input));
}

void wait_for_user_ready(){
	/* Wait for user to type 'ready' into terminal, then send ready signal to server */

	char user_input[1025];

	while(strncmp(user_input, "ready", 5) != 0){
		bzero(user_input, sizeof(user_input)); 
		printf("Please type 'ready' when all players are ready to start\n");
		scanf("%s", user_input);
	}
	write(network_socket, &user_input, sizeof(user_input));
}

void wait_for_server_ready(int c){
	/* Wait for a specific signal from the server before exiting function.
	int c indicates which signal to wait for */

	char buff[1025];

	while(1){
		bzero(buff, sizeof(buff));
		read(network_socket, buff, sizeof(buff));
		printf("From Server : %s\n", buff);
		if (strstr(buff, "start") && c == 1) {
			//upon recieving start signal, exit function
			return; 
		} else if (strstr(buff, "Game has ended!") && c == 2){
			//upon recieving game ended signal, exit function
			return; 
		}
	}
}

void run_game(){
	/* Draw maze, wait for user to complete the maze, and send time to server */
	int time_taken;
	char time[10];
	char buff[1025];
	
	time_taken = run_maze(); 
	sprintf(time, "%d", time_taken);
	strcpy(buff,"\'s time: "); 
	strcat(buff, time);
	printf("Sending time to server! Please wait for other players to finish\n"); 
	write(network_socket, &buff, sizeof(buff));
}

int catch_signal(int sig, void(*handler)(int)){
	/* Handle interruptions from os */
	struct sigaction action;
	action.sa_handler = handler; 
	sigemptyset(&action.sa_mask); 
	action.sa_flags = 0; 
	return sigaction (sig, &action, NULL); 
}

void handle_shutdown(int sig){
	/* handle shutdown signal */
	if(network_socket)
		close(network_socket);
	printf("\nGoodbye!");
	exit(0);
}

void* clienthread(void* args)
{
	// Create a stream socket
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	// Initialise port number and address
	struct sockaddr_in server_address;

	if(connect_to_server(&server_address) == 0){
		return 0; 
	};

	get_username();
	wait_for_user_ready(); 
	wait_for_server_ready(1); // wait for server to indicate all users are ready
	run_game(); // start game
	wait_for_server_ready(2); // wait for server to indicate all users finished maze, ending the game

	close(network_socket); // Close the connection
	pthread_exit(NULL);

	return 0;
}

int main()
{	
	if(catch_signal(SIGINT, handle_shutdown) == -1){
		perror("Can't set interrupt handler");
	}

	pthread_t tid;

	// Create thread
	pthread_create(&tid, NULL, clienthread, NULL);

	// Suspend execution of calling thread
	pthread_join(tid, NULL);
}
