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
	char **envp;
	pid_t childPid;
	int statusChild;
	bool childExited;

	std::vector<struct pollfd> vCGI; 
	// vCGI[0] - pipeFdIn[1] 	POLLOUT - write-into-child end
	// vCGI[1] - pipeFdOut[0]	POLLIN  - read-from-child end
	int vCGIsize;
	int pipeFdIn[2];
	int pipeFdOut[2];
	cgiState state;
} CGIInfo;


//void	freeEnvp(char **envp);
void	closePipes(int* pipeFdIn, int* pipeFdOut);
char	**envpGenerate(parsRequest request, std::string portNumberSocket, std::string hostNameSocket);
bool	makeNonBlocking(int fd);
void	initPipesCreatePollFDstruct(std::vector<struct pollfd> &vPipesCGI, int* pipeFdIn, int* pipeFdOut);



pid_t	launchChild(CGIInfo &info, parsRequest &request, std::string& portNumSocket, std::string& hostNameSocket);

ssize_t	writeChild(const char* data, size_t dataLen, int* pipeFdIn);


void	waitChild(int &statusChild, pid_t childPid, bool &childExited);


ssize_t	readChild(int* pipeFdOut, std::string &reply);

#endif /* spawnProcess_hpp */
