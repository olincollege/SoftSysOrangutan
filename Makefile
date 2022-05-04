all: server main client.o maze.o

server: sockets/server.c
	gcc sockets/server.c -o server

main: maze.o client.o 
	gcc -o client maze.o client.o maze/maze_array.c -lcurses -pthread

client.o: sockets/client.c
	gcc -Wall -c sockets/client.c -pthread

maze.o: maze/maze.c maze/maze_array.c maze/maze.h
	gcc -Wall -c maze/maze.c maze/maze.h maze/maze_array.c -lcurses