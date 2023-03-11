//
//  utilsMethods.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 08/03/2023.
//

#ifndef utilsMethods_hpp
#define utilsMethods_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <vector>

std::string getHeaderByKey(std::map<std::string, std::vector<std::string> >& headers, const std::string& key);
bool isDirectory(std::string& path);
bool isFile(std::string& path);
bool ifFileExsist(std::string& path);
bool ifFileReadable(std::string& path);
bool ifFileExecutable(std::string& path);

#endif /* utilsMethods_hpp */
