//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
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
char *welcome_message = "welcome! Please enter a username \r\n";
int master_socket; 

int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}

int catch_signal(int sig, void(*handler)(int)){
	struct sigaction action;
	action.sa_handler = handler; 
	sigemptyset(&action.sa_mask); 
	action.sa_flags = 0; 
	return sigaction (sig, &action, NULL); 
}

void handle_shutdown(int sig){
	if(master_socket)
		close(master_socket);
	printf("\nGoodbye!\n");
	exit(0);
}

void handle_incoming_connection(struct sockaddr_in* address, int* master_socket, int* addrlen, int * new_socket){
	if ((*new_socket = accept(*master_socket,(struct sockaddr *)address, (socklen_t*)addrlen))<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
		
	//inform user of socket number - used in send and receive commands
	printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , 
	*new_socket , inet_ntoa(address ->sin_addr) , ntohs(address -> sin_port));

	//send new connection greeting message
	if(send(*new_socket, welcome_message, strlen(welcome_message), 0) != strlen(welcome_message) )
	{
		perror("send");
	}
		
	puts("Welcome message sent successfully");
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
	fd_set readfds; //set of socket descriptors
	struct sockaddr_in address;
	char buffer[1025]; //data buffer of 1K
    int i, max_sd, sd, valread, activity, new_socket;
	int client_socket[max_clients];
	int addrlen = sizeof(address);

	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

	if(catch_signal(SIGINT, handle_shutdown) == -1){
		perror("Can't set interrupt handler");
	}

    create_master_socket(&master_socket, &address);
		
	//accept the incoming connection
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
			handle_incoming_connection(&address, &master_socket, &addrlen, &new_socket);
			
			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if(client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);
						
					break;
				}
			}
		}
			
		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
				
			if (FD_ISSET(sd , &readfds))
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
					printf("server read %s\n", buffer);
					send(sd , buffer , strlen(buffer) , 0 );
				}
			}
		}

		// puts("If ready, type 'start' to start the game, else enter any input to refresh server.");
		// scanf("%s", gamemaster_input);

		// if(strcmp(gamemaster_input, "start") == 0){
		// 	puts("Starting game"); 
		// 	accepting_connections = 0; 
		// }
	}
		
	return 0;
}
