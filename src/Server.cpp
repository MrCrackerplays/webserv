#include <Server.hpp>

Server::Server(int port, std::string host, std::string name) : _port(port), _host(host), _name(name) {
}
Server::Server(Server const & src) {
	*this = src;
}

Server::~Server(void) {}

Server & Server::operator=(Server const & rhs) {
	if (this == &rhs)
		return (*this);
	this->_port = rhs._port;
	this->_host = rhs._host;
	this->_name = rhs._name;
	return (*this);
}

int	Server::getPort() const {
	return (this->_port);
}

const std::string&	Server::getHost() const {
	return (this->_host);
}

const std::string&	Server::getName() const {
	return (this->_name);
}

std::ostream& operator<<(std::ostream& out, const Server& f1) {
	out << f1.getHost() << ":" << f1.getPort();
	if (f1.getName() != "")
		out << "(" << f1.getName() << ")";
	return (out);
}
