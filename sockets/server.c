
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0
#define PORT 8888

static int max_clients = 30; 
int master_socket; 
char *welcome_message = "Please wait for other players to be ready \r\n";

void free_memory(char** socket_usernames, int needs_freeing[]){
	/* Free up all memory used to store usernames */

	for (int i = 0; i < max_clients; i++){
		if(needs_freeing[i])
			free(socket_usernames[i]);
	}
}

void send_message(int client_socket[], char message[]){
	/* Send a message to every client socket */

	for (int i = 0 ; i < max_clients ; i++){
		send(client_socket[i], message , strlen(message) , 0);
	}
}

int catch_signal(int sig, void(*handler)(int)){
	/* Handle interrupt signal from os */

	struct sigaction action;
	action.sa_handler = handler; 
	sigemptyset(&action.sa_mask); 
	action.sa_flags = 0; 
	return sigaction (sig, &action, NULL); 
}

void handle_shutdown(int sig){
	/* Handle shutdown signal*/
	if(master_socket)
		close(master_socket);
	printf("\nGoodbye!\n");
	exit(0);
}

void handle_incoming_connection(struct sockaddr_in* address, int* addrlen, int * new_socket){
	/* Accept new connections and send new client a welcome message*/

	if ((*new_socket = accept(master_socket,(struct sockaddr *)address, (socklen_t*)addrlen))<0){
		perror("accept");
		exit(EXIT_FAILURE);
	}
		
	//inform user of socket number - used in send and receive commands
	printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , 
	*new_socket , inet_ntoa(address ->sin_addr) , ntohs(address -> sin_port));

	//send new connection greeting message
	if(send(*new_socket, welcome_message, strlen(welcome_message), 0) != strlen(welcome_message))
	{
		perror("send");
	}
		
	puts("Welcome message sent successfully");
}

void add_child_sockets(int client_socket[], int* sd, int* max_sd, fd_set * readfds){
	/* Add child sockets to set */

	for (int i = 0 ; i < max_clients ; i++)
	{
		//socket descriptor
		*sd = client_socket[i];
			
		//if valid socket descriptor then add to read list
		if(*sd > 0)
			FD_SET(*sd , readfds);
			
		//highest file descriptor number, need it for the select function
		if(*sd > *max_sd)
			*max_sd = *sd;
	}
}

void create_master_socket(int *master_socket, struct sockaddr_in* address){
	/* Create and bind the master socket*/

	int opt = TRUE;

	//create a master socket
	if( (*master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	//set master socket to allow multiple connections 
	if( setsockopt(*master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
	//type of socket created
	address -> sin_family = AF_INET;
	address -> sin_addr.s_addr = INADDR_ANY;
	address -> sin_port = htons( PORT );
		
	//bind the socket to localhost port 8888
	if (bind(*master_socket, (struct sockaddr *)address, sizeof(*address))<0){
		printf("socket bind failed...\n%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);
		
	//try to specify maximum of 3 pending connections for the master socket
	if (listen(*master_socket, 3) < 0){
        printf("Max number of pending connections reached, please try again later");
		exit(EXIT_FAILURE);
	}

}
	
int main(int argc , char *argv[])
{
	fd_set readfds; //set of socket descriptors
	struct sockaddr_in address;
	char buffer[1025]; //data buffer of 1K
    int i, max_sd, sd, valread, activity, new_socket;
	int client_socket[30] = { 0 };
	char *socket_usernames[30]; //stores usernames of each client
	int needs_freeing[30] = { 0 }; //tracks which usernames need to be freed
	int num_playing = 0; //number of players ready to play
	int num_connected = 0;  //number of clients that connected
	int num_finished = 0; //number of players who completed the game
	int addrlen = sizeof(address);

	char end_message[2000] = "Game has ended! Here are the results: \n"; 
	char start_message[] = "start"; 

	if(catch_signal(SIGINT, handle_shutdown) == -1){
		perror("Can't set interrupt handler");
	}

    create_master_socket(&master_socket, &address);
	puts("Waiting for connections ...");
	
	int accepting_username = 0;
	int playing = 1; //tracks whether game is in session
		
	while(playing)
	{
		FD_ZERO(&readfds);	//clear the socket set
		FD_SET(master_socket, &readfds); //add master socket to set
		max_sd = master_socket;
		
		add_child_sockets(client_socket, &sd, &max_sd, &readfds);

		//wait for an activity on one of the sockets , timeout is NULL, so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
	
		if ((activity < 0) && (errno!=EINTR)){
			printf("select error");
		}
			
		//If something happened on the master socket, then its an incoming connection
		if (FD_ISSET(master_socket, &readfds)){
			handle_incoming_connection(&address, &addrlen, &new_socket);
			
			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++){
				//if position is empty
				if(client_socket[i] == 0 ){
					client_socket[i] = new_socket;
					num_connected += 1; 
					printf("Adding to list of sockets as %d\n" , i);
					break;
				}
			}
		}
			
		//Check for IO operation on some other socket
		for (i = 0; i < max_clients; i++){
			sd = client_socket[i];
				
			if (FD_ISSET(sd , &readfds)){
				valread = read( sd , buffer, 1024); 
				buffer[valread] = '\0'; //set the string terminating NULL byte on the end of the data read
				
				//Check if it was for closing, and also read the incoming message
				if (valread == 0){
					//Somebody disconnected , get their details and print
					getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" ,
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					num_connected -= 1; 
					client_socket[i] = 0;
					if(needs_freeing[i])
						free(socket_usernames[i]);
		
				} else if(accepting_username){
					//if previous message indicated next message is username, add current message as username 
					socket_usernames[i] = malloc(strlen(buffer) + 21);
					needs_freeing[i] = 1; 
					strcpy(socket_usernames[i],buffer); 
					printf("username set to %s\n", socket_usernames[i]);
					accepting_username = 0; 
				} else if (strstr(buffer, "username")) {
					//indicate that next message contains username 
					accepting_username = 1; 
				} else if (strstr(buffer, "ready")) {
					//player i is ready to start game
					num_playing += 1; 
					printf("%s is ready\n", socket_usernames[i]);
					if(num_playing == num_connected){
						printf("starting game\n");
						send_message(client_socket, start_message);
					}; 
				} else if (strstr(buffer, "time:")) {
					//handle incoming time from client i 
					char new_score[200]; 
					printf("time recieved from %s\n", socket_usernames[i]); 
					sprintf(new_score, "%s%s seconds\n", socket_usernames[i], buffer); 
					strcat(end_message, new_score);
					num_finished += 1; 

					if(num_finished == num_playing){
						//if all players sent in time, end game
						free_memory(socket_usernames, needs_freeing); 
						send_message(client_socket, end_message);
						playing = 0;  
					}
				}
			}
		}
	}

	puts("Thank you for playing! Shutting down in 20 seconds"); 
	sleep(20);
		
	return 0;
}
