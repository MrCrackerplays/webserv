#include <Server.hpp>

Server::Server(int port, std::string host, std::map<std::string, std::string> error_pages,
				size_t client_body_limit, std::string name)
	: _port(port), _host(host), _name(name), _error_pages(error_pages), _client_body_limit(client_body_limit)
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

void Server::addLocation(std::string path, Location location)
{
	this->_paths.insert(std::pair<std::string, Location>(path, location));
}

int Server::getPort() const
{
	return (this->_port);
}

const std::string &Server::getHost() const
{
	return (this->_host);
}

const std::string &Server::getName() const
{
	return (this->_name);
}

size_t Server::getClientBodyLimit(std::string path) const
{
	//I don't like the way this looks, will change it
	size_t result = this->_client_body_limit;
	std::map<std::string, Location>::const_iterator found = this->_paths.find(path);
	if (found != this->_paths.end() && found->second._client_body_limit != 0)
		result = found->second._client_body_limit;
	return result;
}

const std::string Server::getErrorPage(std::string path, std::string error_code) const
{
	//I don't like the way this looks, will change it
	std::map<std::string, std::string>::const_iterator found = this->_error_pages.find(error_code);
	std::string result = "root/errors/error.html";
	if (found != this->_error_pages.end())
		result = found->second;
	std::map<std::string, Location>::const_iterator locationfound = this->_paths.find(path);
	if (locationfound != this->_paths.end()) {
		found = locationfound->second._error_pages.find(error_code);
		if (found != locationfound->second._error_pages.end())
			result = found->second;
	}
	return result;
}

std::string Server::response(std::string request) const
{
	//magic
	return ("");
}

std::ostream &operator<<(std::ostream &out, const Server &f1)
{
	out << f1.getHost() << ":" << f1.getPort();
	if (f1.getName() != "")
		out << "(" << f1.getName() << ")";
	return (out);
}
