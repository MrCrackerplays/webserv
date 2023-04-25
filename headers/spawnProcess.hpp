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

struct CGIInfo {

	int code;
	char **envp;
	pid_t childPid;

	std::vector<pollfd> vCGI;
	int* pipeFdIn;
	int* pipeFdOut;
};


void	freeEnvp(char **envp);
void	closePipes(int* pipeFdIn, int* pipeFdOut);
char	**envpGenerate(parsRequest request, std::string portNumberSocket, std::string hostNameSocket);
bool	makeNonBlocking(int fd);
void	initPipesCreatePollFDstruct(std::vector<pollfd> &vPipesCGI, int* pipeFdIn, int* pipeFdOut);

pid_t	launchChild(CGIInfo info, parsRequest &request, std::string& portNumSocket, std::string& hostNameSocket);

#endif /* spawnProcess_hpp */
