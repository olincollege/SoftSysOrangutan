# Maze Runner
### Simrun Mutha and Kelly Yen

## Goals
Our MVP for this project is to build a simple, two player maze runner game that runs on a local network using a C implementation of TCP. Both players can connect to the game via their terminals, and race to complete the maze before the other player finishes. Once one player finishes the maze, the game ends and the time is displayed to both players. 

If we’re able to complete the aforementioned MVP, a few stretch goals might include implementing a more advanced GUI or generating random mazes. We could also implement more advanced game features, such as a progress bar showing how close an opponent is to winning, or allowing for more than two players to play at a time. 


## Learning Goals
**Simrun**: I want to learn about how to share data over local networks by using sockets and networks. I also want to get better at organizing code in various files to improve structure and readability. Additionally, I want to get more comfortable with some of the basic C concepts we learned earlier in the semester related to pointers and memory.

**Kelly**: I’d like to learn more about network programming and protocols, as those are areas I have little experience in. I’d also like to use this project to get better at working on collaborative software projects and develop better coding habits. 


## Resources
So far, we have been able to find resources for the parts of the project we have been working on which include creating the maze and understanding how to setup a network connection. Here are some of the resources we have been using in order to work on our project. 
1. TCP server client implementation: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
2. Ncurses documentation: https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
3. More information about network programming:https://beej.us/guide/bgnet/html/
4. Tutorial about socket programming: https://www.geeksforgeeks.org/socket-programming-cc/
5. Multithreading socket programming: https://www.geeksforgeeks.org/handling-multiple-clients-on-server-with-multithreading-using-socket-programming-in-c-cpp/


## Current Steps
1. Work on upgrading the maze generation to be randomly generated instead of hardcoded. We know this will be done when we can print out various mazes. *(Simrun)*.
2. Follow one of the above listed tutorials on networks and sockets. We know this will be done when we have established communication between our two laptops. *(Kelly)*.
3. Follow the tuturial on multithreaded socket programming listed above to understand how we can have multiple people play this game. We know this will be done when we have established a connection between at least three laptops. *(Simrun & Kelly)*.


**Trello Board:** https://trello.com/invite/b/pY9kfBAh/179fcb2ed01b7815f08ed58d782e324c/softsys-final

**Github URL:** https://github.com/olincollege/SoftSysOrangutan