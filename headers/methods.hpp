//
//  methods.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#ifndef methods_hpp
#define methods_hpp

#include "Server.hpp"
#include "constructResponse.hpp"
#include "parseRequest.hpp"
#include <sstream>
#include <map>
#include <vector>
#include <string>

std::string methods(parsRequest &request, std::map<std::string, std::vector<Server> > &servers, std::string &port, std::string &host, bool &CGI);
response responseStructConstruct(std::map<std::string, std::vector<Server> > &servers, std::string& hostPort, std::string body, parsRequest& request);

#endif /* methods_hpp */
