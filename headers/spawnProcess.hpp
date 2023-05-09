//
//  spawnProcess.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 05/03/2023.
//

#ifndef spawnProcess_hpp
#define spawnProcess_hpp

#include "parseRequest.hpp"
#include <poll.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

typedef enum {

	NO_PIPES = 0,
	PIPES_INIT = 1,

	WRITE_READY = 2,
	// WRITE,
	WRITE_DONE = 3,


	READ_READY = 4,
	// READ,
	READ_DONE = 5,

	ERROR = 6
	//TIMEOUT // not used
	
} cgiState;

typedef struct {

	int code;
	pid_t childPid;
	int statusChild;
	bool childExited;
	size_t offset;

	std::vector<struct pollfd> vCGI; 
	size_t contentLenghtCGI;
	// vCGI[0] - pipeFdIn[1] 	POLLOUT - write-into-child end
	// vCGI[1] - pipeFdOut[0]	POLLIN  - read-from-child end
	int vCGIsize;
	int pipeFdIn[2];
	int pipeFdOut[2];
	cgiState state;
} CGIInfo;


void	closePipes(int* pipeFdIn, int* pipeFdOut);
bool	makeNonBlocking(int fd);
void	initPipesCreatePollFDstruct(std::vector<struct pollfd> &vPipesCGI, int* pipeFdIn, int* pipeFdOut);



pid_t	launchChild(CGIInfo &info, parsRequest &request, std::string& portNumSocket, std::string& hostNameSocket);

// ssize_t	writeChild(const char* data, size_t dataLen, int* pipeFdIn);
// ssize_t writeChild(const std::vector<char>& rowData, size_t &offset, int* pipeFdIn);
ssize_t writeChild(const std::vector<char>& rowData, size_t& offset, int* pipeFdIn);

void	waitChild(int &statusChild, pid_t childPid, bool &childExited);


ssize_t	readChild(int* pipeFdOut, std::string &reply);

#endif /* spawnProcess_hpp */
