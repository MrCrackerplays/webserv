#ifndef SERVER_HPP
# define SERVER_HPP
# include <string>
# include <ostream>

class Server {
protected:
	int			_port;
	std::string	_host;
	std::string	_name;
	//add error page(s), client body size limit, list of routes that have 1..n rules
	//check https://github.com/AirisX/nginx_cookie_flag_module#synopsis for an idea on cookies
	//check RFC 3875
public:
	Server(int port, std::string host, std::string name = "");
	Server(Server const & src);

	~Server(void);

	Server & operator=(Server const & rhs);

	int					getPort() const;
	const std::string&	getHost() const;
	const std::string&	getName() const;
private:
	Server(void);
};

std::ostream& operator<<(std::ostream& out, const Server& f1);

#endif
