//
//  spawnProcess.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 05/03/2023.
//

#include "spawnProcess.hpp"
#include "envpGenerate.hpp"
#include "parseRequest.hpp"
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

void	makeNonBlocking(int fd){
	int flagsForFd = fcntl(fd, F_GETFL, 0);
	if (flagsForFd < 0){
		throw std::runtime_error("Socket : fcntl");
	}
	if (fcntl(fd, F_SETFL, flagsForFd | O_NONBLOCK) < 0){
		throw std::runtime_error("Socket : fcntl");
	}
}


std::string	spawnProcess(parsRequest request, std::string& portNumSocket, std::string& hostNameSocket){
	
	//check if the cgi is actually executable before execve
	
	int pipeFD[2];
	pid_t childPid;
	char **envp;
	char *args[2];
	std::string response = "";
	
	if (pipe(pipeFD) == -1){
		throw std::runtime_error("spawnProcess : pipe");
	}
	//make pipes NON-BLOCKABLE
	makeNonBlocking(pipeFD[0]);
	makeNonBlocking(pipeFD[1]);

	childPid = fork();
	if (childPid < 0){
		throw std::runtime_error("spawnProcess : pipe");
	}
	else if (childPid == 0){ //we are in child process
		
		args[0] = (char *)request.physicalPathCgi.c_str();
		args[1] = NULL;
		if (access(args[0], R_OK | X_OK) < 0){
			throw std::runtime_error("spawnProcess : access"); //not sure
		}
		envp = envpGenerate(request, portNumSocket, hostNameSocket);
		if (envp == NULL){
			throw std::runtime_error("spawnProcess : malloc"); }
		
		//manage FD pipes
		dup2(pipeFD[0], STDIN_FILENO); //dup read into STDIN
		close(pipeFD[0]); //close read after dup
		close(pipeFD[1]); //close write.
		
		
		execve(args[0], const_cast<char **>(args), const_cast<char **>(envp));
		throw std::runtime_error("spawnProcess : execve");
	}
	else { //parent
		
		//check what pipes I need to close
		write(pipeFD[1], request.body.c_str(), request.body.length());
		
		int status;
		wait(&status);
		const int BUFFER_SIZE = 1024;
		char buffer[BUFFER_SIZE];
		size_t nbytes = 0;
		while ((nbytes = read(pipeFD[0], buffer, BUFFER_SIZE)) > 0){
			response.append(buffer, nbytes);
		}
		close(pipeFD[0]);
		close(pipeFD[1]);
	}
	return response;
}


