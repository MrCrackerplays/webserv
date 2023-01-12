//
//  main.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 09/01/2023.
//

#include <iostream>
#include "getPort.hpp"

//Parse the port number and other necessary command line arguments.
//Set up the address and port for the server using the getaddrinfo function.
//Create a socket using the socket function.
//Bind the socket to the address and port using the bind function.
//Set the socket to listen for incoming connections using the listen function.
//Initialize the file descriptor sets for poll, epoll, or kqueue as appropriate.
//Set up the timeout variable for poll or kqueue as appropriate.
//Enter a loop to handle incoming connections and requests.
//Inside the loop, call poll, epoll_wait, or kqueue as appropriate to wait for events on the file descriptors.
//Check for events on the listening socket (incoming connection).
//Accept the incoming connection using the accept function.
//Add the new client socket to the file descriptor sets as appropriate.
//Update the max_fd or kq variable as appropriate.
//Check for events on the client sockets (incoming data).
//Read data from the client socket using the recv function.
//Parse the request and determine the necessary action to take.
//Perform the necessary action, possibly using additional functions such as execve, dup, dup2, pipe, fork, and fcntl.
//Send a response to the client using the send function.
//Close the client socket and remove it from the file descriptor sets as appropriate.
//Repeat the loop to handle additional incoming connections and requests.
//Close the listening socket and exit the program.

int main(int argc, const char * argv[]) {
	
	if (argc != 2){
		std::cout << "not enough arguments" << std::endl;
	}

	
	
	
	return 0;
}
