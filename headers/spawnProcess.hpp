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

	NO_PIPES,
	PIPES_INIT,

	WRITE_READY,
	// WRITE,
	WRITE_DONE,

	READ_READY,
	// READ,
	READ_DONE,

	ERROR,
	//TIMEOUT // not used
	
} cgiState;

struct CGIInfo {

	int code;
	char **envp;
	pid_t childPid;
	int statusChild;

	std::vector<pollfd> vCGI; // pipeFdIn[0] - POLLIN // pipeFdOut[1] - POLLOUT
	int vCGIsize;
	int pipeFdIn[2];
	int pipeFdOut[2];
	cgiState state;
};


void	freeEnvp(char **envp);
void	closePipes(int* pipeFdIn, int* pipeFdOut);
char	**envpGenerate(parsRequest request, std::string portNumberSocket, std::string hostNameSocket);
bool	makeNonBlocking(int fd);
void	initPipesCreatePollFDstruct(std::vector<pollfd> &vPipesCGI, int* pipeFdIn, int* pipeFdOut);



pid_t	launchChild(CGIInfo &info, parsRequest &request, std::string& portNumSocket, std::string& hostNameSocket);

size_t	writeInChild(const char* data, size_t dataLen, int* pipeFdIn);


void	waitForChild(int &statusChild, pid_t childPid);


size_t	readFromChild(int* pipeFdOut, std::string &reply);

#endif /* spawnProcess_hpp */
