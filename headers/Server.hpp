#ifndef SERVER_HPP
# define SERVER_HPP
# include <string>
# include <ostream>
# include <map>

class Server {
protected:
	int			_port;
	std::string	_host;
	std::string	_name;
	size_t		_client_body_limit;
	std::map<std::string, std::string> _error_pages;

	//add error page(s), client body size limit, list of routes that have 1..n rules
	//check https://github.com/AirisX/nginx_cookie_flag_module#synopsis for an idea on cookies
	//check RFC 3875
public:
	Server(int port, std::string host, std::map<std::string, std::string> error_pages, size_t client_body_limit = 0, std::string name = "");
	Server(Server const & src);

	~Server(void);

	Server & operator=(Server const & rhs);

	int					getPort(void) const;
	const std::string&	getHost(void) const;
	const std::string&	getName(void) const;
	size_t				getClientBodyLimit(void) const;
	const std::string	getErrorPage(std::string error_code) const;
private:
	Server(void);
};

std::ostream& operator<<(std::ostream& out, const Server& f1);

#endif
