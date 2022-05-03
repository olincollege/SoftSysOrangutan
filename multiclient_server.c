//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
	
#define TRUE 1
#define FALSE 0
#define PORT 8888

static int max_clients = 30; 
char *welcome_message = "welcome! Please enter a username \r\n";

// void handle_incoming_connection(int **client_socket, struct sockaddr_in* address, int* master_socket, int* addrlen, int * new_socket){
// 	if ((*new_socket = accept(*master_socket,(struct sockaddr *)address, (socklen_t*)addrlen))<0)
// 	{
// 		perror("accept");
// 		exit(EXIT_FAILURE);
// 	}
		
// 	//inform user of socket number - used in send and receive commands
// 	printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , 
// 	*new_socket , inet_ntoa(address ->sin_addr) , ntohs(address -> sin_port));

// 	//send new connection greeting message
// 	if(send(*new_socket, welcome_message, strlen(welcome_message), 0) != strlen(welcome_message) )
// 	{
// 		perror("send");
// 	}
		
// 	puts("Welcome message sent successfully");
		
// 	//add new socket to array of sockets
// 	for (int i = 0; i < max_clients; i++)
// 	{
// 		//if position is empty
// 		if( *client_socket[i] == 0 )
// 		{
// 			*client_socket[i] = *new_socket;
// 			printf("Adding to list of sockets as %d\n" , i);
				
// 			break;
// 		}
// 	}
// }

void start_game(int sig){
	puts("Game started");
	return; 
}

void add_child_sockets(int client_socket[], int* sd, int* max_sd, fd_set * readfds){
	//add child sockets to set
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
    int opt = TRUE;
	//struct sockaddr_in address;

	//create a master socket
	if( (*master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(*master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
	//type of socket created
	address -> sin_family = AF_INET;
	address -> sin_addr.s_addr = INADDR_ANY;
	address -> sin_port = htons( PORT );
		
	//bind the socket to localhost port 8888
	if (bind(*master_socket, (struct sockaddr *)address, sizeof(*address))<0)
	{
		printf("socket bind failed...\n%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);
		
	//try to specify maximum of 3 pending connections for the master socket
	if (listen(*master_socket, 3) < 0)
	{
        printf("Max number of pending connections reached, please try again later");
		exit(EXIT_FAILURE);
	}

}
	
int main(int argc , char *argv[])
{
    		
	//set of socket descriptors
	fd_set readfds;

	struct sockaddr_in address;

	char buffer[1025]; //data buffer of 1K

    int i, max_sd, sd, valread, activity, addrlen, new_socket, master_socket;
	int client_socket[max_clients];
	// int *ptr = &client_socket;
	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}
    create_master_socket(&master_socket, &address);
		
	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");
	int accepting_connections = 1; 

	char gamemaster_input[100]; 
		
	while(accepting_connections)
	{
		//clear the socket set
		FD_ZERO(&readfds);
	
		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
		
		add_child_sockets(client_socket, &sd, &max_sd, &readfds);

		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
	
		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}
			
		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			
			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
		
			//send new connection greeting message
			if(send(new_socket, welcome_message, strlen(welcome_message), 0) != strlen(welcome_message) )
			{
				perror("send");
			}
				
			puts("Welcome message sent successfully");
				
			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);
						
					break;
				}
			}
			puts("If ready, press space bar to start game");
		}
			
		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
				
			if (FD_ISSET( sd , &readfds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				if ((valread = read( sd , buffer, 1024)) == 0)
				{
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" ,
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					client_socket[i] = 0;
				}
					
				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end
					//of the data read
					buffer[valread] = '\0';
					send(sd , buffer , strlen(buffer) , 0 );
				}
			}
		}
		scanf("%s", gamemaster_input);
		if(strcmp(gamemaster_input, "start") == 0){
			puts("Starting game"); 
			accepting_connections = 0; 
		}
	}
		
	return 0;
}
