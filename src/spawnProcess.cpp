//
//  spawnProcess.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 05/03/2023.
//

#include "spawnProcess.hpp"
#include <string>
#include <unistd.h>
#include <sys/wait.h>

//dataForProcess initiateData(std::string hostName, std::string portNumber, parsRequest request){
//	dataForProcess data;
//
//	data.portNumber = portNumber;
//	data.hostName = hostName;
//	data.request = request;
//
//	return data;
//}



void	spawnProcess(){
	

	
	
	

// Parse the CGI path from the HTTP request and store it in `cgi_path`
// ...

// Create a pipe
//int pipe_fd[2];
//if (pipe(pipe_fd) == -1) {
//	perror("pipe");
//	exit(EXIT_FAILURE);
//}
//
//// Create a child process
//pid_t pid = fork();
//if (pid == -1) {
//	perror("fork");
//	exit(EXIT_FAILURE);
//}
//
//if (pid == 0) {
//	// Child process
//
//	// Close the write end of the pipe
//	close(pipe_fd[1]);
//
//	// Redirect the read end of the pipe to stdin
//	if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
//		perror("dup2");
//		exit(EXIT_FAILURE);
//	}
//
//	// Execute the CGI script
//	if (execl(cgi_path, cgi_path, NULL) == -1) {
//		perror("execl");
//		exit(EXIT_FAILURE);
//	}
//} else {
//	// Parent process
//
//	// Close the read end of the pipe
//	close(pipe_fd[0]);
//
//	// Write the HTTP response to the write end of the pipe
//	// ...
//
//	// Wait for the child process to exit
//	int status;
//	if (waitpid(pid, &status, 0) == -1) {
//		perror("waitpid");
//		exit(EXIT_FAILURE);
//	}
//}


}


