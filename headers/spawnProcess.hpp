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

std::string	spawnProcess(parsRequest request, std::string& portNumSocket, std::string& hostNameSocket, int &statusChild);

void	freeEnvp(char **envp);
void	closePipes(int* pipeFdIn, int* pipeFdOut);

char **envpGenerateNew(parsRequest request, std::string portNumberSocket, std::string hostNameSocket, std::string save_location);

bool	makeNonBlocking(int fd);
pid_t	forkProcess();
void	inChildProcess(int* pipeFdIn, int* pipeFdOut, char **envp, char *path);

void	writeInChild(const char* data, size_t len, int* pipeFdIn, int* pipeFdOut, char **envp);
void	waitForChild(int &statusChild, pid_t childPid, int* pipeFdIn, int* pipeFdOut, char **envp);


void	readFromChild(int* pipeFdIn, int* pipeFdOut, std::string &reply, char **envp);

//called from Socket:
void	initPipesCreatePollFDstruct(std::vector<pollfd> &vPipesCGI, int* pipeFdIn, int* pipeFdOut)



pid_t	launchChild(parsRequest &request, std::string& portNumSocket, std::string& hostNameSocket, std::vector<pollfd> &vCGI, int* pipeFdIn, int* pipeFdOut, char **envp);


#endif /* spawnProcess_hpp */
