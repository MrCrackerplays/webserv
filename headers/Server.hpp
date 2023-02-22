#ifndef SERVER_HPP
# define SERVER_HPP
# include <string>
# include <ostream>
# include <map>
# include <Location.hpp>

class Server {
protected:
	int			_port;
	std::string	_host;
	size_t		_client_body_limit;
	std::vector<std::string>			_names;
	std::map<std::string, std::string>	_error_pages;
	std::map<std::string, Location>		_paths;
public:
	Server(int port, std::string host,
			size_t client_body_limit = 0,
			std::vector<std::string> names = std::vector<std::string>());
	Server(Server const & src);

	~Server(void);

	Server & operator=(Server const & rhs);

	int					getPort(void) const { return (this->_port);}
	const std::string&	getHost(void) const { return (this->_host);}

	const std::vector<std::string>&	getNames(void) const { return (this->_names);}

	void				addName(std::string name);
	void				addLocation(std::string path, Location &location);
	void				addError(std::string error_code, std::string path);
	void				setClientBodyLimit(size_t cbl) {this->_client_body_limit = cbl;}

	std::string			generateResponse(std::string request) const;

	size_t				getClientBodyLimit(void) const { return (this->_client_body_limit);}
	size_t				getClientBodyLimit(std::string path) const;
	const std::string	getErrorPage(std::string error_code, std::string path) const;
	const Location &	getClosestLocation(std::string path) const;
	Location *			getLocation(std::string path);
	size_t				getLocationCount(void) const { return (this->_paths.size());}

	void				printLocations(std::ostream& out, std::string separator) const;
private:
	Server(void);
};

std::ostream& operator<<(std::ostream& out, const Server& serv);

#endif
