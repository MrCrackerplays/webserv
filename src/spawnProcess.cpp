//
//  spawnProcess.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 05/03/2023.
//

#include "spawnProcess.hpp"
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
//https://www.rfc-editor.org/rfc/rfc3875

char **envpGenerate(parsRequest request, std::string portNumberSocket, std::string hostNameSocket){
	
	char **envp;
	
	envp = (char **)malloc(sizeof(char *) * 17);
	if (envp == NULL)
		return NULL;
	
	std::string str0 = "SERVER_SOFTWARE=webserv/1.0";
	envp[0] = (char *)str0.c_str();
	
	std::string str1 = "GATEWAY_INTERFACE=CGI/1.1";
	envp[1] = (char *)str1.c_str();
	size_t pos = request.urlPath.find('/');
	if (pos == std::string::npos){
		pos = 0;
	}
	std::string hostName = request.urlPath.substr(0, pos);
	std::string str2 = "SERVER_NAME=" + hostName;
	envp[2] = (char *)str2.c_str();
	std::string str3 = "SERVER_PROTOCOL=" + request.httpVers;
	envp[3] = (char *)str3.c_str();
	std::string str4 = "SERVER_PORT=" + portNumberSocket;
	envp[4] = (char *)str4.c_str();
	std::string str5 = "REQUEST_METHOD=" + request.methodString;
	envp[5] = (char *)str5.c_str();
	std::string str6 = "QUERY_STRING=" + request.queryString;
	envp[6] = (char *)str6.c_str();
	
	
	//FIX - check if the cont type is correct name for header
	std::string contType = getHeaderByKey(request.headers, "content-type");
	std::string str7 = "CONTENT_TYPE=" + contType; //content-type in the header
	envp[7] = (char *)str7.c_str();
	//FIX
	
	std::string str8 = "CONTENT_LENGTH=" + std::to_string(request.contentLenght);
	envp[8] = (char *)str8.c_str();
	pos = request.urlPath.find('/');
	if (pos == std::string::npos){
		pos = 0;
	}
	std::string scriptN = request.urlPath.substr(pos+1, request.urlPath.length());
	std::string str9 = "SCRIPT_NAME=" + scriptN;
	envp[9] = (char *)str9.c_str();
	std::string str10 = "PATH_INFO=" + request.hostNameHeader;
	envp[10] = (char *)str10.c_str();
	std::string str11 = "PATH_TRANSLATED="  + request.hostNameHeader;
	envp[11] = (char *)str11.c_str();
	std::string str12 = "REMOTE_HOST=";
	envp[12] = (char *)str12.c_str();
	std::string str13 = "REMOTE_ADDR=";
	envp[13] = (char *)str13.c_str();
	std::string str14 = "AUTH_TYPE=";
	envp[14] = (char *)str14.c_str();
	std::string str15 = "REMOTE_USER=";
	envp[15] = (char *)str15.c_str();
	std::string str16 = "REMOTE_IDENT=";
	envp[16] = (char *)str16.c_str();
	envp[17] = NULL;
	
	for (int i = 0; i < 17; i++) {
		std::cout << envp[i] << std::endl;
	}
	
	return envp;
}


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
	
	std::string reply;
	int pipeFdIn[2];
	int pipeFdOut[2];
	pid_t childPid;
	char *args[2] = {(char *)request.physicalPathCgi.c_str(), NULL};
	char *envp[] = {NULL}; //temp untill envp is tested
	//char **envp = envpGenerate(request, portNumSocket, hostNameSocket);
	char buff[1024];
	
	
	if (pipe(pipeFdIn) == -1 || pipe(pipeFdOut) == -1){
		throw std::runtime_error("spawnProcess : pipe");
	}
	
	makeNonBlocking(pipeFdIn[0]);
	makeNonBlocking(pipeFdIn[1]);
	makeNonBlocking(pipeFdOut[0]);
	makeNonBlocking(pipeFdOut[1]);
	
	childPid = fork();
	if (childPid < 0){
		throw std::runtime_error("spawnProcess : pipe");
	}
	else if (childPid == 0){ //we are in child process
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
		size_t res = 1;
		while (res > 0){
			res = read(pipeFdOut[0], buff, 1024);
			if (res < 0)
				throw std::runtime_error("spawnProcess : read");
			else if (res == 0){
				reply.append(buff);
				break;
			} else {
				reply.append(buff);
			}
			memset(buff, 0, 1024);
		}
		std::cerr << "END, buff: " << reply << std::endl;
	}
	return reply;
}


