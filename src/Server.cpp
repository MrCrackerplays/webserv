#include <Server.hpp>
#include <iostream>

Server::Server(int port, std::string host,
				size_t client_body_limit, std::string name)
	: _port(port), _host(host), _name(name), _client_body_limit(client_body_limit), _error_pages()
{
}
Server::Server(Server const &src)
{
	*this = src;
}

Server::~Server(void) {}

Server &Server::operator=(Server const &rhs)
{
	if (this == &rhs)
		return (*this);
	this->_port = rhs._port;
	this->_host = rhs._host;
	this->_name = rhs._name;
	this->_client_body_limit = rhs._client_body_limit;
	this->_error_pages = rhs._error_pages;
	this->_paths = rhs._paths;
	return (*this);
}

void	Server::addLocation(std::string path, Location &location)
{
	// this->_paths[path] = location;
	std::cout << location.getRequireCookie() << std::endl;
	this->_paths.insert(std::pair<std::string, Location>(path, location));
	std::cout << location.getRequireCookie() << std::endl;
}

void	Server::addError(std::string error_code, std::string path) {
	this->_error_pages.insert(std::pair<std::string, std::string>(error_code, path));
}

size_t Server::getClientBodyLimit(std::string path) const
{
	size_t result = this->_client_body_limit;
	try
	{
		result = getClosestLocation(path).getClientBodyLimit();
	}
	catch(const std::exception& e)
	{}
	return (result);
}

const std::string Server::getErrorPage(std::string error_code, std::string path) const
{
	std::string result = "root/errors/error.html";
	std::map<std::string, std::string>::const_iterator found = this->_error_pages.find(error_code);
	if (found != this->_error_pages.end())
		result = found->second;
	try
	{
		std::map<std::string, std::string> error_pages = getClosestLocation(path).getErrorPages();
		found = error_pages.find(error_code);
		if (found != error_pages.end())
			result = found->second;
	}
	catch(const std::exception& e)
	{}
	return (result);
}

const Location &	Server::getClosestLocation(std::string path) const {
	std::map<std::string, Location>::const_iterator found = this->_paths.find(path);
	if (found != this->_paths.end())
		return (found->second);
	while (path.length() > 0) {
		std::string newpath = path.substr(0, path.rfind("/"));
		if (newpath == path)
			throw std::exception();
		path = newpath;
		found = this->_paths.find(path);
		if (found != this->_paths.end())
			return (found->second);
	}
	throw std::exception();
}

std::string Server::response(std::string request) const
{
	//magic
	return (request);
}

std::ostream &operator<<(std::ostream &out, const Server &serv)
{
	out << serv.getHost() << ":" << serv.getPort();
	if (serv.getName() != "")
		out << "(" << serv.getName() << ")";
	if (serv.getClientBodyLimit() != 0)
		out << " client-limit:" << serv.getClientBodyLimit();
	try
	{
		out << " location=" << serv.getClosestLocation("/shii");
	}
	catch(const std::exception& e)
	{}
	return (out);
}
