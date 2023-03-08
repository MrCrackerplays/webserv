//
//  spawnProcess.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 05/03/2023.
//

#include "spawnProcess.hpp"
#include "envpGenerate.hpp"
#include <string>
#include <iostream>
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
	//char **envp;
	char *args[2];
	std::string response = "";
	
	if (pipe(pipeFD) == -1){
		throw std::runtime_error("spawnProcess : pipe");
	}
	//make pipes NON-BLOCKABLE
	makeNonBlocking(pipeFD[0]);
	makeNonBlocking(pipeFD[1]);
//	envp = envpGenerate(request, portNumSocket, hostNameSocket);
//	if (envp == NULL){
//		throw std::runtime_error("spawnProcess : malloc"); }

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
		//TEST ENVP
		char *envp[] = {NULL};
		
		//manage FD pipes
		dup2(pipeFD[0], STDIN_FILENO); //dup read into STDIN
		dup2(pipeFD[1], STDOUT_FILENO); //dup write into STDOUT
		close(pipeFD[0]); //close read after dup
		close(pipeFD[1]); //close write.
		execve(args[0], args, envp);
		throw std::runtime_error("spawnProcess : execve");
	}
	else { //parent
		
		//check what pipes I need to close
		write(pipeFD[1], request.body.c_str(), request.body.length());
		
		int status;
		wait(&status);//Wno hang - flag for waitpid
		const int BUFFER_SIZE = 1024;
		char buffer[BUFFER_SIZE];
		size_t nbytes = 0;
		while ((nbytes = read(pipeFD[0], buffer, BUFFER_SIZE)) > 0){
			response += buffer;
			std::cout << response << std::endl;
			break;
		}
		close(pipeFD[0]);
		close(pipeFD[1]);
		
		
		
		
		std::cout << "in parent" << std::endl << "parent pipes after: " << pipeFD[0] << ", " << pipeFD[1] << std::endl;
	}
	return response;
}



void	spawnProcessTwo(parsRequest request, std::string& portNumSocket, std::string& hostNameSocket){
	
	std::string reply;
	int pipeFdIn[2];
	int pipeFdOut[2];
	pid_t childPid;
	char *args[2] = {(char *)request.physicalPathCgi.c_str(), NULL};
	char *envp[] = {NULL};
	char buff[1024];
	
	makeNonBlocking(pipeFdIn[0]);
	makeNonBlocking(pipeFdIn[1]);
	makeNonBlocking(pipeFdOut[0]);
	makeNonBlocking(pipeFdOut[1]);
	
	
	if (pipe(pipeFdIn) == -1 || pipe(pipeFdOut) == -1){
		throw std::runtime_error("spawnProcess : pipe");
	}
	
	childPid = fork();
	if (childPid < 0){
		throw std::runtime_error("spawnProcess : pipe");
	}
	else if (childPid == 0){ //we are in child process
		std::cerr << "in child " << std::endl;
		//child
		close(pipeFdIn[1]);
		dup2(pipeFdIn[0], STDIN_FILENO);
		close(pipeFdIn[0]);
		
		dup2(pipeFdOut[1], STDOUT_FILENO);
		close(pipeFdOut[1]);
		close(pipeFdOut[0]);
		execve(args[0], args, envp);
		throw std::runtime_error("spawnProcess : execve");
	}
	else { //parent
		
		write(pipeFdIn[1], request.body.c_str(), request.body.length());
		close(pipeFdIn[0]);
		
		int status;
		waitpid(childPid, &status, 0);
		
		close(pipeFdOut[1]);
		while (read(pipeFdOut[0], buff, 1024)){
			reply += buff;
			std::cout << buff << std::endl;
		}
	}

}


