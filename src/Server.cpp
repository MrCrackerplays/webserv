#include "Server.hpp"

Server::Server(std::string port, std::string host,
				size_t client_body_limit, std::vector<std::string> names)
	: _port(port), _host(host), _client_body_limit(client_body_limit), _names(names), _error_pages()
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
	this->_names = rhs._names;
	this->_client_body_limit = rhs._client_body_limit;
	this->_error_pages = rhs._error_pages;
	this->_paths = rhs._paths;
	return (*this);
}
void	Server::addName(std::string name) {
	this->_names.push_back(name);
}

void	Server::addLocation(std::string path, Location &location)
{
	this->_paths.insert(std::pair<std::string, Location>(path, location));
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

Location *	Server::getLocation(std::string path) {
	std::map<std::string, Location>::iterator found = this->_paths.find(path);
	if (found != this->_paths.end())
		return (&(found->second));
	return (NULL);
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

void	Server::printLocations(std::ostream& out, std::string separator) const {
	for (std::map<std::string, Location>::const_iterator it = this->_paths.begin(); it != this->_paths.end(); it++) {
		if (it != this->_paths.begin())
			out << separator;
		out << "Location[" << it->first << "] " << it->second;
	}
}

std::string Server::generateResponse(std::string request) const
{
	//magic
	return (request);
}

std::ostream &operator<<(std::ostream &out, const Server &serv)
{
	out << serv.getHost() << ":" << serv.getPort();
	if (!serv.getNames().empty()) {
		const std::vector<std::string> & names = serv.getNames();
		out << "(";
		for (size_t i = 0; i < names.size(); i++) {
			if (i > 0)
				out << ", ";
			out << names[i];
		}
		out << ")";
	}
	if (serv.getClientBodyLimit())
		out << " max_body: " << serv.getClientBodyLimit();
	if (serv.getLocationCount() != 0) {
		out << "\n\t";
		serv.printLocations(out, "\n\t");
	}
	return (out);
}
