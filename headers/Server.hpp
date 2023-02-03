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
	std::string	_name;
	size_t		_client_body_limit;
	std::map<std::string, std::string>	_error_pages;
	std::map<std::string, Location>		_paths;

	//add error page(s), client body size limit, list of routes that have 1..n rules
	//check https://github.com/AirisX/nginx_cookie_flag_module#synopsis for an idea on cookies
	//check RFC 3875
public:
	Server(int port, std::string host,
			size_t client_body_limit = 0,
			std::string name = "");
	Server(Server const & src);

	~Server(void);

	Server & operator=(Server const & rhs);

	int					getPort(void) const { return (this->_port);}
	const std::string&	getHost(void) const { return (this->_host);}
	const std::string&	getName(void) const { return (this->_name);}

	void				addLocation(std::string path, Location &location);
	void				addError(std::string error_code, std::string path);

	std::string			generateResponse(std::string request) const;
// protected:
	size_t				getClientBodyLimit(void) const { return (this->_client_body_limit);}
	size_t				getClientBodyLimit(std::string path) const;
	const std::string	getErrorPage(std::string error_code, std::string path) const;
	const Location &	getClosestLocation(std::string path) const;
private:
	Server(void);
};

std::ostream& operator<<(std::ostream& out, const Server& serv);

#endif
