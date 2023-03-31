//
//  utilsMethods.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 08/03/2023.
//

#include "utilsMethods.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//https://www.geeksforgeeks.org/http-headers-content-type/
std::string getContentType(std::string& filename){
	//can be upper case
	std::map<std::string, std::string> extensionsMap;
	extensionsMap.insert(std::make_pair("html", "text/html"));
	extensionsMap.insert(std::make_pair("htm", "text/html"));
	extensionsMap.insert(std::make_pair("txt", "text/plain"));
	extensionsMap.insert(std::make_pair("jpg", "image/jpeg"));
	extensionsMap.insert(std::make_pair("jpeg", "image/jpeg"));
	extensionsMap.insert(std::make_pair("png", "image/png"));
	extensionsMap.insert(std::make_pair("gif", "image/gif"));
	extensionsMap.insert(std::make_pair("pdf", "application/pdf"));

	std::string contentType;
	size_t pos = filename.rfind(".");
	if (pos != std::string::npos){
		std::string extension = filename.substr(pos + 1); //compare with lower case
		std::map<std::string, std::string>::iterator it = extensionsMap.find(extension);
		if (it != extensionsMap.end()){
			return it->second;
		}
	}
	return "text/plain"; //default
}

std::string getHeaderByKey(std::map<std::string, std::vector<std::string> >& headers, const std::string& key){
	
	std::map<std::string, std::vector<std::string> >::iterator mapIt;
	std::vector<std::string> neededVector;
	std::vector<std::string>::iterator vectorIt;
	std::string result;

	mapIt = headers.find(key);
	neededVector = mapIt->second;
	for (vectorIt = neededVector.begin(); vectorIt != neededVector.end(); vectorIt++) {
		std::string dereferencedString = *vectorIt;
		result += dereferencedString;
	}
	return result;
	
}

bool isDirectory(std::string& path) {
	
	struct stat info;
	if (stat(path.c_str(), &info) != 0) {
		// error 404?
		return false;
	}
	if (info.st_mode & S_IFDIR) {
		return true;
	} else {
		return false;
	}
}

bool isFile(std::string& path) {
	struct stat info;
	if (stat(path.c_str(), &info) != 0) {
		// error 404?
		return false;
	}
	if (info.st_mode & S_IFREG) {
		return true;
	} else {
		return false;
	}
}

bool ifFileExsist(std::string& path){
	
	if (access(path.c_str(), F_OK) == 0)
		return true;
	return false;
}

bool ifFileReadable(std::string& path){
	
	if (access(path.c_str(), R_OK) == 0)
		return true;
	return false;
}

bool ifFileExecutable(std::string& path){
	
	if (access(path.c_str(), X_OK) == 0)
		return true;
	return false;
}

bool ifFileWritable(std::string& path){
	
	if (access(path.c_str(), W_OK) == 0)
		return true;
	return false;
}
