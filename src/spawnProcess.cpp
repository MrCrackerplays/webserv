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

//possible? 
//Null termination: Each string in the array is not null-terminated. The c_str() method of a std::string object returns a pointer to a null-terminated array of characters, but the function casts this pointer to a char* without copying the null terminator. This could lead to problems if the execve() function tries to read beyond the end of a string. To fix this, the function should allocate a new char array for each string, copy the contents of the std::string into the array (including the null terminator), and then store the pointer to the array in the envp array.

char **envpGenerate(parsRequest request, std::string portNumberSocket, std::string hostNameSocket){
	
	char **envp;
	
	envp = new char*[18];
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
	//std::string hostName = request.urlPath.substr(0, pos);
	std::string str2 = "SERVER_NAME=" + hostNameSocket;
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
	
	std::string str8 = "CONTENT_LENGTH=" + std::to_string(request.requestBodyLen);
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
	
//	for (int i = 0; i < 18; i++) {
//		std::cout << envp[i] << std::endl;
//	}
	
	return envp;
}


bool	makeNonBlocking(int fd){
	int flagsForFd = fcntl(fd, F_GETFL, 0);
	if (flagsForFd < 0){
		return false;
	}
	if (fcntl(fd, F_SETFL, flagsForFd | O_NONBLOCK) < 0){
		return false;
	}
	return true;
}

std::string	spawnProcess(parsRequest request, std::string& portNumSocket, std::string& hostNameSocket, int &statusChild){
	
	std::string reply;
	pid_t childPid;
	int pipeFdIn[2];
	int pipeFdOut[2];
	char *args[2] = {(char *)request.physicalPathCgi.c_str(), NULL};
	char **envp = envpGenerate(request, portNumSocket, hostNameSocket);
	if (envp == NULL){
		throw std::runtime_error("spawnProcess : new");
	}
	
	//initianing 2 pipes and making all ends of both pipes non-blocking
	if (pipe(pipeFdIn) == -1 || pipe(pipeFdOut) == -1){
		delete [] envp;
		throw std::runtime_error("spawnProcess : pipe");
	}
	
	if (!makeNonBlocking(pipeFdIn[0]) || !makeNonBlocking(pipeFdIn[1]) || !makeNonBlocking(pipeFdOut[0]) || !makeNonBlocking(pipeFdOut[1])){
		delete [] envp;
		throw std::runtime_error("Socket : fcntl");
	}
	
	//spawn a child process
	childPid = fork();
	if (childPid < 0){ //fork failed
		close(pipeFdIn[0]);
		close(pipeFdIn[1]);
		close(pipeFdOut[0]);
		close(pipeFdOut[1]);
		delete [] envp;
		throw std::runtime_error("Socket : fork");
	}
	else if (childPid == 0){ //in child process
		
		if (dup2(pipeFdIn[0], STDIN_FILENO) < 0){
			delete [] envp;
			exit(1);
		}
		if (dup2(pipeFdOut[1], STDOUT_FILENO) < 0){
			delete [] envp;
			exit(1);
		}
		close(pipeFdIn[0]);
		close(pipeFdIn[1]);
		close(pipeFdOut[0]);
		close(pipeFdOut[1]);
		execve(args[0], args, envp);
		delete [] envp;
		exit(1); //HOW do I end child process in case of execve failure without exit?
	}
	else { //in parent process
		
		//write in child block
		const char* data = request.requestBody.c_str();
		size_t len = request.requestBody.length();
		while (len > 0) {
			ssize_t n = write(pipeFdIn[1], data, len);
			if (n < 0) {
				
				close(pipeFdIn[0]);
				close(pipeFdIn[1]);
				close(pipeFdOut[0]);
				close(pipeFdOut[1]);
				delete [] envp;
				throw std::runtime_error("spawnProcess: write");
			}
			data += n;
			len -= n;
		}
		close(pipeFdIn[1]);
		close(pipeFdIn[0]);
		
		//waiting for child to proceed
		int status;
		if (waitpid(childPid, &status, 0) < 0){
			delete [] envp;
			throw std::runtime_error("spawnProcess: waitpid");
		}
			
		if (WIFEXITED(status)){
			statusChild = WEXITSTATUS(status);
		} else {
			statusChild = -1;
			close(pipeFdOut[1]);
			close(pipeFdOut[0]);
			delete [] envp;
			throw std::runtime_error("spawnProcess : execve");
			//return reply; //reply or throw?
		}
		close(pipeFdOut[1]);

		//read from child block
		size_t res = 1;
		char buff[1024];
		memset(buff, 0, 1024);
		while (res > 0){
			res = read(pipeFdOut[0], buff, 1023);
			if (res < 0){
				close(pipeFdOut[0]);
				delete [] envp;
				throw std::runtime_error("spawnProcess : read");
			}
			else if (res == 0){
				buff[res] = '\0';
				reply.append(buff, res);
				break;
			} else {
				buff[res] = '\0';
				reply.append(buff, res);
			}
			memset(buff, 0, 1024);
		}
		close(pipeFdOut[0]);
	}
	delete [] envp;
	return reply;
}


